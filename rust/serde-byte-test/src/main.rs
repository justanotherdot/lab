use crossbeam::channel::{bounded, unbounded};
use csv::ReaderBuilder;
use parking_lot::{Condvar, Mutex};
use std::collections::BTreeMap;
use std::error::Error;
use std::path::PathBuf;
use std::sync::Arc;
use structopt::StructOpt;

//use assert_no_alloc::*;
//#[cfg(debug_assertions)]
//#[global_allocator]
//static A: AllocDisabler = AllocDisabler;

#[derive(Clone, Debug)]
enum Field {
    Unknown,
    String,
    Integer,
    Float,
}

pub fn example_par_x(data: &PathBuf, capabilities: usize) -> Result<(), Box<dyn Error>> {
    crossbeam::scope(|scope| {
        let mut rdr = ReaderBuilder::new().quoting(false).from_path(data).unwrap();
        let hs = rdr.headers().unwrap().clone();
        use crossbeam::channel::{Receiver, Sender};
        let (tx, rx): (Sender<csv::ByteRecord>, Receiver<csv::ByteRecord>) = bounded(1024);
        let handles: Vec<_> = (0..capabilities)
            .map(|tid| {
                let rx = rx.clone();
                let hs = hs.clone();
                scope.spawn(move |_| {
                    let mut vs = vec![None; hs.len()];
                    while let Ok(record) = rx.recv() {
                        for (i, v) in record.iter().enumerate() {
                            vs[i] = parse(&v);
                        }
                    }
                    eprintln!("[tid {}] thread end", tid);
                    vs
                })
            })
            .collect();
        let mut record = csv::ByteRecord::new();
        while !rdr.is_done() {
            rdr.read_byte_record(&mut record).unwrap();
            tx.send(record.clone()).unwrap();
        }
        drop(tx);
        dbg!(handles
            .into_iter()
            .flat_map(|h| h
                .join()
                .unwrap()
                .into_iter()
                .enumerate()
                .collect::<Vec<_>>())
            .map(|(i, x)| (hs.get(i), x))
            .collect::<BTreeMap<_, _>>());
    })
    .unwrap();
    dbg!("[main] done.");
    Ok(())
}

pub fn example_par(data: &PathBuf, capabilities: usize) -> Result<(), Box<dyn Error>> {
    let mut rdr = ReaderBuilder::new().quoting(false).from_path(data).unwrap();
    let hs = rdr.headers().unwrap().clone();
    let rdr0 = Mutex::new(rdr);
    let cvar_pair0 = Arc::new((rdr0, Condvar::new()));
    let block_size = 512;
    let (tx0, rx) = unbounded();
    crossbeam::scope(|scope| {
        for _tid in 0..capabilities {
            let cvar_pair = cvar_pair0.clone();
            let tx = tx0.clone();
            scope.spawn(move |_| {
                let mut record = csv::ByteRecord::new();
                let (rdr1, cvar) = &*cvar_pair;
                loop {
                    let mut rdr = rdr1.lock();
                    if !rdr.is_done() {
                        let mut k = 0;
                        while k < block_size {
                            rdr.read_byte_record(&mut record).unwrap();
                            for (i, v) in record.iter().enumerate() {
                                tx.send((i, parse(&v))).unwrap();
                            }
                            k += 1;
                        }
                        cvar.notify_one();
                        if !rdr.is_done() {
                            cvar.wait(&mut rdr);
                        }
                    } else {
                        break;
                    }
                }
            });
        }
    })
    .unwrap();
    drop(tx0);
    let mut vs = vec![None; hs.len()];
    while let Ok((i, f)) = rx.recv() {
        vs[i] = f;
    }
    dbg!(vs
        .into_iter()
        .enumerate()
        .map(|(i, x)| (hs.get(i), x))
        .collect::<BTreeMap<_, _>>());
    Ok(())
}

// approx. 0.8-1ms on avg.
pub fn read_disk(data: &PathBuf) -> Result<Vec<u8>, Box<dyn Error>> {
    use std::convert::TryInto;
    use std::fs::File;
    use std::io::{self, BufRead, BufReader, Read};
    let file = File::open(data)?;
    let meta = file.metadata()?;
    let mut reader = BufReader::new(file);
    let mut contents = Vec::with_capacity(meta.len().try_into()?);
    reader.read_to_end(&mut contents);
    //let lines = reader.lines().collect::<Result<Vec<_>, _>>()?;
    Ok(contents)
}

pub fn read_disk_into_byte_record_lines(data: &PathBuf) -> Result<(), Box<dyn Error>> {
    use std::convert::TryInto;
    use std::fs::File;
    use std::io::{self, BufRead, BufReader, Read};
    let file = File::open(data)?;
    let mut reader = BufReader::new(file);
    let lines = reader.lines().collect::<Result<Vec<_>, _>>()?;
    for line in lines {
        let row: Vec<_> = line.split(',').collect();
        csv::ByteRecord::from(row);
    }
    Ok(())
}

// approx. 3-4ms on avg.
pub fn read_disk_csv(data: &PathBuf) -> Result<csv::ByteRecord, Box<dyn Error>> {
    let mut rdr = ReaderBuilder::new().quoting(false).from_path(data)?;
    let mut record = csv::ByteRecord::new();
    while !rdr.is_done() {
        rdr.read_byte_record(&mut record).unwrap();
    }
    Ok(record)
}

pub fn example(data: &PathBuf) -> Result<(), Box<dyn Error>> {
    let data = read_disk(data)?;
    let mut rdr = ReaderBuilder::new()
        .quoting(false)
        .from_reader(data.as_slice());
    let hs = rdr.headers()?.clone();
    let mut vs = vec![None; hs.len()];
    let mut record = csv::ByteRecord::new();
    while !rdr.is_done() {
        rdr.read_byte_record(&mut record).unwrap();
        for (i, v) in record.iter().enumerate() {
            vs[i] = parse(&v);
        }
    }
    dbg!(vs
        .into_iter()
        .enumerate()
        .map(|(i, x)| (hs.get(i), x))
        .collect::<BTreeMap<_, _>>());
    Ok(())
}

#[inline(always)]
fn parse(bytes: &[u8]) -> Option<Field> {
    //assert_no_alloc(|| {
    let string = match std::str::from_utf8(bytes) {
        Ok(v) => v,
        Err(_) => return Some(Field::Unknown),
    };
    if string.parse::<i64>().is_ok() {
        return Some(Field::Integer);
    };
    if string.parse::<f64>().is_ok() {
        return Some(Field::Float);
    };
    Some(Field::String)
    //})
}

#[derive(Debug, StructOpt)]
#[structopt(
    name = "serde-byte-test",
    about = "A demo for byte deserialization via `csv`."
)]
struct Opt {
    #[structopt(parse(from_os_str))]
    input: PathBuf,
}

#[allow(unused)]
macro_rules! dhat {
    () => {
        use dhat::{Dhat, DhatAlloc};
        #[global_allocator]
        static ALLOCATOR: DhatAlloc = DhatAlloc;
        let _dhat = Dhat::start_heap_profiling();
    };
}

fn main() {
    //dhat!();
    let opt = Opt::from_args();

    // roughly 170ms to read roughly 1GiB.
    // by contrast, `cat`ing the same file
    // takes only about 170-200ms.
    //let now = std::time::Instant::now();
    //read_disk(&opt.input).unwrap();
    //dbg!(now.elapsed());

    // ditto to the above, hence
    // producing a csv::ByteRcord is negligible.
    //let now = std::time::Instant::now();
    //read_disk_into_byte_record_lines(&opt.input).unwrap();
    //dbg!(now.elapsed());

    // roughly 3s to read roughly 1GiB.
    //let now = std::time::Instant::now();
    //read_disk_csv(&opt.input).unwrap();
    //dbg!(now.elapsed());

    let now = std::time::Instant::now();
    //example_par(&opt.input, 4).unwrap();
    //example_par_x(&opt.input, 4).unwrap();
    example(&opt.input).unwrap();
    dbg!(now.elapsed());
}

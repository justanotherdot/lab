use std::io;
trait WriteInt {
    fn write_i32(&mut self, i: i32) -> io::Result<()>;
}

// Write i32's in little endian order
impl<W: ?Sized + io::Write> WriteInt for W {
  fn write_i32(&mut self, i: i32) -> io::Result<()> {
        let mut buf: [u8; 4] = [0; 4];
        buf[0] = (i & 0xFF) as u8;
        buf[1] = ((i >> 8) & 0xFF) as u8;
        buf[2] = ((i >> 16) & 0xFF) as u8;
        buf[3] = ((i >> 24) & 0xFF) as u8;

        // Big endian.
        let mut buf01: [u8; 4] = [0; 4];
        buf01[0] = ((i >> 24) & 0xFF) as u8;
        buf01[1] = ((i >> 16) & 0xFF) as u8;
        buf01[2] = ((i >> 8) & 0xFF) as u8;
        buf01[3] = (i & 0xFF) as u8;

        println!("{:?}", buf01);
        println!("{:?}", buf);

        try!(self.write(&buf));
        Ok(())
    }
}

fn test() -> io::Result<()> {
    let mut csr = io::Cursor::new(vec![]);
    csr.write_i32(0x12345678)
}

fn main() {
    match test() {
        Ok(_) => {
            println!("Write was successful")
        }
        _ => {
            println!("Write failed");
        }
    }
}

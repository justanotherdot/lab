use tokio::net::{TcpListener, TcpStream};

struct Server {
    listener: TcpListener,
}

#[derive(Debug)]
enum Error {
    IoError(std::io::Error),
}

impl From<std::io::Error> for Error {
    fn from(err: std::io::Error) -> Self {
        Error::IoError(err)
    }
}

impl Server {
    async fn start(address: &str, port: &str) -> Result<(), Error> {
        let listener = TcpListener::bind(format!("{}:{}", address, port)).await?;
        Server { listener }.listen().await
    }

    async fn listen(self) -> Result<(), Error> {
        loop {
            let (socket, _) = self.listener.accept().await?;
            tokio::spawn(async move { Server::echo(socket).await });
        }
    }

    async fn echo(socket: TcpStream) -> Result<(), Error> {
        let mut buf = [0; 2048];
        loop {
            let n = socket.try_read(&mut buf)?;
            if n == 0 {
                break;
            }
            println!(
                "[server] read {} bytes of socket: {:?}",
                n,
                std::str::from_utf8(&buf[0..n])
            );
            let header = format!(
                "HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Connection: close
Content-Length: {}

",
                n
            );
            socket.try_write(header.as_bytes())?;
            socket.try_write(&buf[0..n])?;
        }
        Ok(())
    }
}

#[tokio::main]
async fn main() {
    Server::start("localhost", "8080")
        .await
        .unwrap_or_else(|e| {
            eprintln!("[server] failed to start server: {:?}", e);
            std::process::exit(1);
        });
}

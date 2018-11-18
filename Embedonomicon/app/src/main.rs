#![no_std]
#![no_main]

use cortex_m_semihosting::{
    debug,
    hio::{self, HStdout},
};

use log::{error, warn, Log};
use rt::entry;

struct Logger {
    hstdout: HStdout,
}

impl Log for Logger {
    type Error = ();

    fn log(&mut self, address: u8) -> Result<(), ()> {
        self.hstdout.write_all(&[address])
    }
}

entry!(main);

fn main() -> ! {
    let hstdout = hio::hstdout().unwrap();
    let mut logger = Logger { hstdout };

    warn!(logger, "Hello, world!");

    error!(logger, "Goodbye");

    debug::exit(debug::EXIT_SUCCESS);

    loop {}
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn HardFault(_ef: *const u32) -> ! {
    // do something interesting here
    loop {}
}
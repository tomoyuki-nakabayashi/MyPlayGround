#![feature(core_intrinsics)]
#![no_std]
#![no_main]

use core::intrinsics;

use rt::entry;

entry!(main);

fn main() -> ! {
    // this executes the undefined instruction (UDF) and causes a HardFault exception
    unsafe { intrinsics::abort() }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn HardFault(_ef: *const u32) -> ! {
    // do something interesting here
    loop {}
}
#![feature(panic_implementation)]
#![no_std]
#![feature(unique)]
#![feature(const_fn)]
#![feature(const_unique_new)]
#![feature(ptr_internals)]

mod vga_buffer;

extern crate volatile;

#[no_mangle]
pub extern fn rust_main() {
    vga_buffer::print_something();

  loop{}
}

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
#[no_mangle]
pub fn panic(info: &PanicInfo) -> ! {
    loop {}
}
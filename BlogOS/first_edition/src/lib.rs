#![feature(panic_implementation)]
#![no_std]
#![feature(unique)]
#![feature(const_fn)]
#![feature(const_unique_new)]
#![feature(ptr_internals)]

#[macro_use]
mod vga_buffer;

extern crate volatile;
extern crate spin;

#[no_mangle]
pub extern fn rust_main() {
    vga_buffer::clear_screen();
    println!("Hello World{}", "!");

    loop{}
}

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
#[no_mangle]
pub fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
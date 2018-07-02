#![feature(panic_implementation)]
#![no_std]  // Don't link the Rust standard library
#![no_main]  // Disable all Rust-level entry points

mod vga_buffer;

use core::panic::PanicInfo;

#[panic_implementation]
#[no_mangle]
pub fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]  // Don't mangle the name of the function
pub extern "C" fn _start() -> ! {
    // This function is the entry point, since the linker looks for a function
    // named `_start` by default.
    vga_buffer::print_something();

    loop {}
}

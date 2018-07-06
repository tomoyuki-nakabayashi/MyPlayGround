#![feature(panic_implementation)]
#![no_std]  // Don't link the Rust standard library
#![no_main]  // Disable all Rust-level entry points
#[macro_use]
extern crate lazy_static;

mod vga_buffer;

extern crate volatile;
extern crate spin;

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
    use core::fmt::Write;
    vga_buffer::WRITER.lock().write_str("Hello again").unwrap();
    write!(vga_buffer::WRITER.lock(), ", some numbers: {} {}", 42, 1.337).unwrap();

    loop {}
}

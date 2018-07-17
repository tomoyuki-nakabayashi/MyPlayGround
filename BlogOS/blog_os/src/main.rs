#![feature(panic_implementation)]
#![no_std]  // Don't link the Rust standard library
#![cfg_attr(not(test), no_main)]  // Disable all Rust-level entry points
#![cfg_attr(test, allow(dead_code, unused_macros, unused_imports))]

#[cfg(test)]
extern crate std;

#[cfg(test)]
extern crate array_init;

#[macro_use]
extern crate lazy_static;

extern crate volatile;
extern crate spin;
extern crate uart_16550;
extern crate x86_64;

#[macro_use]
mod serial;

#[macro_use]
mod vga_buffer;

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
#[no_mangle]
pub fn panic(info: &PanicInfo) -> ! {
    println!("{}", info);
    loop {}
}

#[cfg(not(test))]
#[no_mangle]  // Don't mangle the name of the function
pub extern "C" fn _start() -> ! {
    // This function is the entry point, since the linker looks for a function
    // named `_start` by default.
    println!("Hello World{}", "!");  // prints to vga buffer
    serial_println!("Hello Host{}", "!");

    unsafe { exit_qemu(); }

    loop {}
}

pub unsafe fn exit_qemu() {
    use x86_64::instructions::port::Port;

    let mut port = Port::<u32>::new(0xf4);
    port.write(0);
}

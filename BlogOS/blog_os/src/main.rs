#![feature(panic_implementation)]
#![no_std]  // Don't link the Rust standard library
#![cfg_attr(not(test), no_main)]  // Disable all Rust-level entry points
#![cfg_attr(test, allow(dead_code, unused_macros, unused_imports))]

// Add the library as dependency (same crate name as executable)
#[macro_use]
extern crate blog_os;
extern crate bootloader_precompiled;

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_handler]
#[no_mangle]
pub fn panic(info: &PanicInfo) -> ! {
    println!("{}", info);
    loop {}
}

// This function is the entry point, since the linker looks for a function
// named `_start` by default.
#[cfg(not(test))]
#[no_mangle]  // Don't mangle the name of the function
pub extern "C" fn _start() -> ! {
    println!("Hello World{}", "!");  // prints to vga buffer

    blog_os::gdt::init();
    blog_os::interrupts::init_idt();

    unsafe { blog_os::interrupts::PICS.lock().initialize() };

    println!("It did not crash!");
    loop {}
}
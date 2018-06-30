#![feature(lang_items)]
#![no_std]  // Don't link the Rust standard library
#![no_main]  // Disable all Rust-level entry points

#[lang = "panic_fmt"]
fn panic_fmt() -> ! { loop {} }

#[no_mangle]  // Don't mangle the name of the function
pub extern "C" fn _start() -> ! {
    // This function is the entry point, since the linker looks for a function
    // named `_start` by default.
    loop {}
}
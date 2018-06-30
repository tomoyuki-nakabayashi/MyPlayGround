#![feature(lang_items)]
#![no_std]
#![no_main]

#[lang = "panic_fmt"]
fn panic_fmt() -> ! { loop {} }

#[no_mangle]
pub extern "C" fn _start() -> ! {
    loop {}
}
#![feature(panic_implementation)]
#![feature(abi_x86_interrupt)]
#![no_std]  // Don't link the Rust standard library
#![cfg_attr(not(test), no_main)]  // Disable all Rust-level entry points
#![cfg_attr(test, allow(dead_code, unused_macros, unused_imports))]

// Add the library as dependency (same crate name as executable)
#[macro_use]
extern crate blog_os;

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
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
    init_idt();

    fn stack_overflow() {
        stack_overflow(); // for each recursion, the return address is pushed
    }

    // trigger a stack overflow
    stack_overflow();

    println!("It did not crash!");
    loop {}
}

#[macro_use]
extern crate lazy_static;

extern crate x86_64;
use x86_64::structures::idt::{InterruptDescriptorTable, ExceptionStackFrame};

lazy_static! {
    static ref IDT: InterruptDescriptorTable = {
        let mut idt = InterruptDescriptorTable::new();
        idt.breakpoint.set_handler_fn(breakpoint_handler);
        unsafe {
            idt.double_fault.set_handler_fn(double_fault_handler)
                .set_stack_index(blog_os::gdt::DOUBLE_FAULT_IST_INDEX);
        }
        idt
    };
}

pub fn init_idt() {
    IDT.load();
}

extern "x86-interrupt" fn breakpoint_handler (
    stack_frame: &mut ExceptionStackFrame) 
{
    println!("EXCEPTION: BREAKPOINT\n{:#?}", stack_frame);
}

extern "x86-interrupt" fn double_fault_handler(
    stack_frame: &mut ExceptionStackFrame, _error_code: u64)
{
    println!("EXCEPTION: DOUBLE FAULT\n{:#?}", stack_frame);
    loop {}
}

fn main() {
    println!("1 + 2 = {}", 1u32 + 2);
    println!("1 - 2 = {}", 1i32 - 2);

    println!("true And false is {}", true && false);
    println!("true Or false is {}", true || false);
    println!("Not true is {}", !true);

    println!("0011 And 0101 is {:04b}", 0b0011u32 & 0b0101);

    println!("One million is witten as {}", 1_000_000u32);
}

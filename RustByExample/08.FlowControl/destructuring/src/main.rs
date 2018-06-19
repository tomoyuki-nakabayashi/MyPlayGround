// `allow` required to silence warnings because only one variant is used.
#[allow(dead_code)]
enum Color {
    // These 3 are specified solely by their name.
    Red,
    Blue,
    Green,
    // These likewise tie `u32` tuples to different names: color models.
    RGB(u32, u32, u32),
    HSV(u32, u32, u32),
}

fn main() {
    // tuples
    let pair = (0, -2);
    println!("Tell me about {:?}", pair);
    // Match can be used to destructure a tuple
    match pair {
        // Destructure the second
        (0, y) => println!("First is `0` and `y` is `{:?}`", y),
        (x, 0) => println!("`x` is `{:?}` and last is `0`", x),
        _      => println!("it doesn't matter what they are"),
        // `_` means don't bind the value to a variable
    }

    // enums
    let color = Color::RGB(122, 17, 40);
    println!("What color is it?");
    // An `enum` can be destructured using a `match`.
    match color {
        Color::Red => println!("The color is Red!"),
        Color::Blue => println!("The color is Blue!"),
        Color::Green => println!("The color is Green!"),
        Color::RGB(r, g, b) => println!("Red: {}, green: {}, and blue: {}", r, g, b),
        Color::HSV(h, s, v) => println!("Hue: {}, saturation: {}, value {}", h, s, v),
        // Don't need another arm because all variants have been examined.
    }

    // pointers/ref
    // Assign a reference of type `i32`. The `&` signifies there is a reference being assigned.
    let reference = &4;
    match reference {
        // If `reference`s is pattern matched against `&val`, it results in a comparison like:
        // `&i32`, `&val`
        // ^ We see that if the matching `&`s are dropped, then the `i32` should be assigned to `val`.
        &val => println!("Got a value via destructuring: {:?}", val),
    }

    // To avoid the `&`, you dereference before matching.
    match *reference {
        val => println!("Got a value via dereferencing: {:?}", val),
    }

    // What if you don't start with a reference? `reference` was a `&`
    // because the right side was already a reference.
    // This is not a reference because the right side is not one.
    let _not_a_reference = 3;

    // Rust provides `ref` for exactly this purpose. It modifies the assignment so that a
    // reference is created for the element; this reference is assigned.
    let ref _is_a_reference = 3;

    // Accordingly, by defining 2 values without references, references can be retrieved via `ref` and `ref mut`.
    let value = 5;
    let mut mut_value = 6;

    // Use `ref` keyword to create a reference
    match value {
        ref r => println!("Got a reference to a value: {:?}", r),
    }

    // Use `ref mut` similarly
    match mut_value {
        ref mut m => {
            // Got a reference. Gotta dereference it before we can add anything to it.
            *m += 10;
            println!("We added 10. `mut_value`: {:?}", m);
        }
    }

    // structs
    struct Foo { x: (u32, u32), y: u32 }

    // destructure members of the struct
    let foo = Foo { x: (1, 2), y: 3 };
    let Foo { x: (a, b), y } = foo;

    println!("a = {}, b = {}, y = {}", a, b, y);

    // you can destructure structs and rename the variables,
    // the order is not important
    let Foo { y: i, x: j } = foo;
    println!("i = {:?}, j = {:?}", i, j);

    // and you can also ignore some variables:
    let Foo { y, .. } = foo;
    println!("y = {}", y);

    // this will give an error: pattern does not mention field `x`
    // let Foo { y } = foo;
}

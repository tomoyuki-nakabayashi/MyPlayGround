fn main() {
    // This binding lives in the main function
    let long_lived_binding = 1;

    // This is a block, and has a smaller scope than the main function
    {
        let short_lived_binding = 2;
        println!("inner short: {}", short_lived_binding);

        // This binding *shadows* the outer one
        let long_lived_binding = 5_f32;

        println!("inner long: {}", long_lived_binding);
    }

    // Error! `short_lived_binding` doesn't exist in this scope
}

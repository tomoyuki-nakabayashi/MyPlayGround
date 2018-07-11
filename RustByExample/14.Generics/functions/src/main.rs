struct A;
struct S(A);
struct SGen<T>(T);

// The following functions all take ownership of the variable passed into them
// and immediately go out of scope, freeing the variable.

// Define a function `reg_fn` that takes an argument `_s` of the `S`.
// This has no `<T>` so this is not a generic function.
fn reg_fn(_s: S) {}

//Define a function `gen_spec

fn main() {
    println!("Hello, world!");
}

use std::fmt;

#[derive(Debug)]
struct Structure(i32);

impl fmt::Display for Structure {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let Structure(num) = self;
        write!(f, "{}", num)
    }
}

#[derive(Debug)]
struct Deep(Structure);

impl fmt::Display for Deep {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let Deep(internal) = self;
        internal.fmt(f)
    }
}

fn main() {
    println!("{:?} months in a year.", 12);
    println!("{1:?} {0:?} is the {actor:?} name.",
        "Slater",
        "Christian",
        actor="actor's");

    println!("Now {:?} will print!", Structure(3));
    println!("Now {:?} will print!", Deep(Structure(7)));

    println!("{} will print by fmt::Display.", Structure(3));
    println!("{} will print by fmt::Display.", Deep(Structure(7)));
}

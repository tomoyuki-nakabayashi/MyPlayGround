fn main() {
    println!("{} days", 31);
    println!("{0}, this is {1}. {1}, this is {0}", "Alice", "Bos");

    println!("{subject} {verb} {object}",
            object="the lazy dog",
            subject="the quick brown fox",
            verb="jump over");
    
    println!("{} of {:b} people know binary, the other half don't", 1, 2);
    println!("{number:>width$}", number=1, width=6);
    println!("{number:>0width$}", number=1, width=6);

    println!("My name is {0}, {1} {0}", "Bond", "James");

    struct Structure{number: i32};
    impl std::fmt::Display for Structure {
        fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result{
            write!(f, "{}", self.number)
        }
    }
    println!("This struct `{}` won't print...", Structure{number:3});
}

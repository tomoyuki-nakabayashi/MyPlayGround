fn main() {
    let mut x: i32 = 1;
    x = 7;
    let x = x; // xはイミュータブルになって7に束縛されました

    let y = 4;
    let y = "I can also be bound to text!"; // yは違う型になりました
}

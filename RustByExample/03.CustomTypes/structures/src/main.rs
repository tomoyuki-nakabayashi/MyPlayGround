#[derive(Debug)]
struct Person<'a> {
    name: &'a str,
    age: u8,
}

struct Nil;

#[derive(Debug)]
struct Pair(i32, f32);

struct Point {
    x: f32,
    y: f32,
}

#[allow(dead_code)]
struct Rectangle {
    p1: Point,
    p2: Point,
}

fn rect_area(rect: &Rectangle) -> f32 {
    let Rectangle{ p1: Point{ x: x1, y: y1 }, p2: Point{ x: x2, y: y2 } } = rect;
    let area = (x2 - x1) * (y2 - y1);
    area.abs()
}

fn square(point: &Point, len: f32) -> Rectangle {
    let lcp = Point{ x: point.x, y: point.y };
    let rbp = Point{ x: lcp.x + len, y: lcp.y + len };
    Rectangle{ p1: lcp, p2: rbp}
}

fn main() {
    let name = "Peter";
    let age = 27;
    let peter = Person {name, age};

    println!("{:?}", peter);

    let point: Point = Point {x: 0.3, y: 0.4};

    println!("point coordinates: ({}, {})", point.x, point.y);

    let new_point = Point {x: 0.1, ..point};
    println!("second point: ({}, {})", new_point.x, new_point.y);

    let Point { x: my_x, y: my_y} = point;

    let _rectangle = Rectangle {
        p1: Point {x: my_y, y: my_x},
        p2: point,
    };

    let _nil = Nil;

    let pair = Pair(1, 0.1);
    println!("pair contains {:?} and {:?}", pair.0, pair.1);

    let Pair(integer, decimal) = pair;
    println!("pair contains {:?} and {:?}", integer, decimal);
    println!("pair was not moved {:?}", pair);

    println!("the area of rectangle is {}", rect_area(&_rectangle));

    let point = Point { x: 1.0, y: 2.0 };
    let sq = square(&point, 5.0);
    println!("created squeare ({} {}), ({} {})", sq.p1.x, sq.p1.y, sq.p2.x, sq.p2.y);
}

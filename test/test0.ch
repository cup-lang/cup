fn main() {
    print("Welcome to the guessing game! 🎲");

    var number = rand() * 100;

    loop {
        var guess = scan("Take a guess!");
        guess = parse(guess);
     
        if guess < number {
            print("Try higher!");
        }
        else if guess > number {
            print("Try lower!");
        }
        else {
            print("Congratuliations! You win!");
    
            break;
        }
    }

    // match msg {
    //     Message::Hello(id) => print(id);
    // }

    return 0;
}

struct SomeStruct {
    foo: i32,
    bar: f32,
}

// enum SomeEnum {
//     Option1(i32, f32),
//     Option2(x: i32, y: f32)
// }
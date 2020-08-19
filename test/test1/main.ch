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

    return 0;
}
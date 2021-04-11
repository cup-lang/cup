```````````````````````````````````````````````` use
` use foo::bar;
```````````````````````````````````````````````` mod
` mod foo { };
```````````````````````````````````````````````` comp, field
` comp SomeComp<T, TT> {
`     foo: i32,
`     bar: i32,
` }
```````````````````````````````````````````````` enum, option
` enum SomeEnum<T, TT> {
`     Option0,
`     Option1(foo: i32, bar: f32),
`     Option2,
` }
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(foo: string, bar: int)
```````````````````````````````````````````````` prop
` prop SomeProp<T, TT> { }
```````````````````````````````````````````````` def
` def SomeProp<T, TT> for SomeObject<T, TT> { }
```````````````````````````````````````````````` sub_def, arg
` sub foo<T, TT>(bar: i32) -> i32 {
`     if 0 { } elif 1 { } else { };
`     loop { };
`     while 2 { };
`     for i, i < 1, i += 1 { };
`     each i, foo { };
`     match foo { 0 { }, 1 { }, };
`     ret ~foo 1;
`     next ~foo;
`     jump ~foo;
`     try ~foo bar();
` }
```````````````````````````````````````````````` var_def
` bar: i32 = 1;
` bar := 1;

` comp arr<T> {
`     pointer: ptr<T>,
`     size: int,
` }
` 
` comp string = arr<u8>;

` #req("stdint.h") #req("stdio.h") mod _ {};
` 
` sub a<T>() -> int { }
` 
` sub main() -> int {
`     ` b := a<i32>();
`     ` c := a<f32>();
` 
`     ` d := f32(1);
`     ` e := i32(1.2);
` 
`     printf("hello world \n"); 
` }

sub abc() -> int {
    ` vec<u8>::new(16);
    ` for i, i < argc, i += 1 { };
    ` f32(1);
    ` vec<T> { a = 1, b = 2 };
    ` match foo { };
    ` '`'
    ` fix bad parsing of flow control
    ` fix left to right for assign
}
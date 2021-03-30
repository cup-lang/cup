```````````````````````````````````````````````` use
` use foo::bar;
```````````````````````````````````````````````` mod
` mod foo { };
```````````````````````````````````````````````` comp, field
` comp SomeComp<T, TT, TTT> {
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

comp Abc<T> {
    a: T,
    b: int,
}

` comp,enum,prop,def,sub_def
`1. When generating generate a version for every generic combination used 

` #req("stdint.h") #req("stdio.h") mod _ {};

` sub main() -> int {
    ` generics
    ` casting

    a: Abc<i32>;

    ` a: int = 123;
    ` printf("Hello Cup: %i\n", a);
` }
```````````````````````````````````````````````` use
` use foo::bar;
```````````````````````````````````````````````` mod
` mod foo { };
```````````````````````````````````````````````` comp, field
` comp SomeComp<T, TT> {
`     i32 foo,
`     i32 bar,
` };
```````````````````````````````````````````````` enum, option
` enum SomeEnum<T, TT> {
`     Option0,
`     Option1(i32 foo, f32 bar),
`     Option2,
` };
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(string foo, int bar);
```````````````````````````````````````````````` prop
` prop SomeProp<T, TT> { };
```````````````````````````````````````````````` def
` def SomeProp<T, TT> for SomeObject<T, TT> { };
```````````````````````````````````````````````` sub_def, arg
` sub foo<T, TT>(i32 bar) -> i32 {
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
` };
```````````````````````````````````````````````` var_def
` var bar = 1;

comp SomeComp<T, TT> {
    i32 foo,
    i32 bar,
};

` sub foo (int a, int b) {
`     bit::and(a, b);
` };

`sub foo() -> int {
    ` add 'var'

    ` var c = std:ptr<int>().a;
    
    ` if 0 { } elif 1 { } else { };
    ` loop { };
    ` while 2 { };
    ` for i, i < 1, i += 1 { };
    ` each i, foo { };
    ` match foo { 0 { }, 1 { }, };
    ` ret ~foo 1;
    ` next ~foo;
    ` jump ~foo;
    ` try ~foo bar();
`}
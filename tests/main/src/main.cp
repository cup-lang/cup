```````````````````````````````````````````````` use
` use foo;
` use foo::bar; ``TODO``
```````````````````````````````````````````````` mod
` mod foo { }
```````````````````````````````````````````````` comp, field
` comp SomeComp {
`     foo: i32,
`     bar: i32,
` }
```````````````````````````````````````````````` enum, option
` enum SomeEnum {
`     Option0,
`     Option1(
`         foo: i32, 
`         bar: f32,
`     ),
`     Option2,
` }
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(foo: string, bar: int);
```````````````````````````````````````````````` prop
` prop SomeProp { }
```````````````````````````````````````````````` def
` def SomeProp for SomeObject { }
```````````````````````````````````````````````` sub_def, arg
` sub foo(bar: i32) -> i32, f32 {
`     if 0 { } elif 1 { } else { }
`     loop { }
`     while 1 { }
`     for i, i, i { }
`     each i, foo { }
`     match foo { case 0 { }, case 1 { }, }
`     back ~foo 1, 1.0;
`     next ~foo;
`     jump ~foo;
` }
```````````````````````````````````````````````` var_def
` bar: i32 = 1;
` bar := 1;

sub a() {
}
```````````````````````````````````````````````` use
` use foo;
` use foo::bar; ``TODO``
```````````````````````````````````````````````` mod
` mod foo { }
```````````````````````````````````````````````` struct, field
` struct SomeStruct {
`     foo: i32,
`     bar: i32,
` }
```````````````````````````````````````````````` union, field
` union SomeUnion {
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
sub foo(bar: i32) -> i32 {
    if !1 {

    }

    ` if 0 { } elif 1 { } else { }
    ` loop { }
    ` while 1 { }
    ` for i, i, i { }
    ` each i, foo { }
    ` match foo { case 0 { }, case 1 { }, }
    ` back `bar foo;
    ` next ~label;
    ` jump ~label;
}
```````````````````````````````````````````````` var_def
` bar: i32 = 1;
` bar := 1;

`` add ~labels to parser
`` add variables to parser
`` a < b < c
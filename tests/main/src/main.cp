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
enum SomeEnum {
    Option0,
    Option1(
        foo: i32, 
        bar: f32,
    ),
    Option2,
}
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(foo: string, bar: int);
```````````````````````````````````````````````` prop
` prop SomeProp { }
```````````````````````````````````````````````` def
` def SomeProp for SomeObject { }
```````````````````````````````````````````````` sub_def, arg
` sub foo() -> i32 {
` 
` }
```````````````````````````````````````````````` var_def
` bar: i32 = 1;

`` fix number literals (?in tags?)
`` add parseType()
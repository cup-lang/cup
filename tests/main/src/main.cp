```````````````````````````````````````````````` use
use foo::bar;
```````````````````````````````````````````````` mod
` mod foo { }
```````````````````````````````````````````````` struct, field
` struct SomeStruct {
`     foo: i32,
`     foo: i32,
` }
```````````````````````````````````````````````` union, field
` union SomeUnion {
`     foo: i32,
`     foo: i32,
` }
```````````````````````````````````````````````` enum, option
` enum SomeEnum {
`     Option0,
`     Option1(
`         a: i32, 
`         b: f32,
`     ),
`     Option2,
` }
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(s: string, b: int);
```````````````````````````````````````````````` prop
` prop SomeProp { }
```````````````````````````````````````````````` def
` def SomeProp for SomeObject { }
```````````````````````````````````````````````` sub_def, arg
` sub some_sub() -> i32 {
` 
` }
```````````````````````````````````````````````` var_def
` var some_var: i32;

`` don't allow keywords in paths
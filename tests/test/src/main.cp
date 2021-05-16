```````````````````````````````````````````````` use
` use foo:bar;
```````````````````````````````````````````````` mod
` mod foo { };
```````````````````````````````````````````````` comp, field
` comp SomeComp {
`     i32 foo,
`     i32 bar,
` };
```````````````````````````````````````````````` enum, option
` enum SomeEnum {
`     Option0,
`     Option1(i32 foo, f32 bar),
`     Option2,
` };
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(string foo, int bar);
```````````````````````````````````````````````` prop
` prop SomeProp { };
```````````````````````````````````````````````` def
` def SomeProp<T, TT> for SomeObject<T, TT> { };
```````````````````````````````````````````````` sub_def, arg
` foo:bar xyz() {};
` sub foo(i32 bar) {
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
` foo:bar xyz = 1;
` var bar = 1;

`` treat simple enums as C-enums
` return Command_None;
`` 'as' casting
` int32(0)

` FIX: match multiple case values (op chaining)
` TODO: add deref counterpart assign operators
` TODO: add 'as' keyword for casting
` TODO: def a for b -> def a, b

` TODO: comp foo {}; -> comp foo ();
` TODO: SomeComp {} -> SomeComp ()
` TODO: remove "def x" 
` TODO: add 'has' keyword
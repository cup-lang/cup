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

int main() {
    vec<i32> abc;
    abc.foo(12);
    ` vec<i32>:foo(abc$, 12);
};

` int main() {
`     vec_i32 abc;
`     vec_i32_foo(&abc, 12);
` }

#gen("T")
comp vec {
    ptr<T> buf,
};

#gen("T")
def vec<T> {
    #self
    sub foo(int bar) {
        this.buf = xxx;
    };
};

``` #self
`` generate sub_defs differently
` ^ adding a argument
` ^ replacing 'this' with derefed pointer
`` generate '.' ops differently
` ^ get the type of the variable

` mem:size is sizeof
` comps/enums before subs

`` v0.1
` TODO: enums
` TODO: 'match' generation

`` v0.2
` TODO: SomeComp {} -> SomeComp ()
` TODO: comp foo {}; -> comp foo ();
` TODO: add 'has' keyword
` TODO: def a for b -> def a, b
` TODO: #self
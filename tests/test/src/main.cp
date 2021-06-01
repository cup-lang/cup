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

#req("stdlib.h")
mod mem {
    #bind("malloc") sub alloc() {};
    #bind("realloc") sub realloc() {};
    #bind("sizeof") sub size() {}; 
};

int main() {
    vec<i32> foo;
    foo.push(12);
};

#gen("T")
comp vec {
    ptr<T> buf,
    int size,
    int cap,
};

#gen("T")
def vec<T> {
    vec<T> new(int cap) {
        ret vec<T> {
            buf = mem:alloc(mem:size<T>() * cap),
            size = 0,
            cap = cap,
        };
    };

    #self
    sub push(T item) {
        this.buf[this.size] = item;
        this.size += 1;

        if this.size == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
    };
};

` fix binds
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
```````````````````````````````````````````````` use
` use foo:bar;
```````````````````````````````````````````````` mod
` mod foo:bar;
```````````````````````````````````````````````` comp, field
` comp SomeComp (
`     i32 foo,
`     i32 bar,
` ) {};
```````````````````````````````````````````````` enum, option
` enum SomeEnum (
`     i32 foo,
`     i32 bar,
`     
`     Option0,
`     Option1(i32 foo, f32 bar),
`     Option2(),
` ) {};
```````````````````````````````````````````````` tag_def, arg
` tag test_tag(string foo, int bar) { };
```````````````````````````````````````````````` prop
` prop SomeProp { };
```````````````````````````````````````````````` def
` def SomeProp<T, TT>, SomeObject<T, TT> { };
```````````````````````````````````````````````` sub_def, arg
` foo:bar xyz() {};
` sub foo(i32 bar) {
`     foo:bar xyz = 1;
`     var bar = 1;
`     if 0 { } elif 1 { } else { };
`     loop { };
`     while 2 { };
`     for i, i < 1, i += 1 { };
`     each i in foo { };
`     match foo { 0 { }, 1 { }, };
`     ret ~foo 1;
`     next ~foo;
`     jump ~foo;
`     try ~foo bar();
` };
```````````````````````````````````````````````` var_def
` foo:bar xyz = 1;
` var bar = 1;
````````````````````````````````````````````````

vec<u8> foo;

#req("stdint.h")
int main(vec<u8> bar) {
    foo.push('1');
    foo.push('2');
    foo.push('3');

    bar.push('1');
    bar.push('2');
    bar.push('3');

    vec<f32> bar = vec<f32> {
        buf = buf,
        len = len,
        cap = cap,
    };
};

#gen("T")
comp vec<T> (
    ptr<T> buf,
    int len,
    int cap,
);

#gen("T")
def vec<T> {
    #self
    sub push(T item) {
        this.buf[this.len] = item;
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
    };
};

#req("stdlib.h")
mod mem {
    #bind("realloc")
    sub realloc();
};

` add comp inst
` fix globals value
` add match/case

` free exprs & generic replaced paths
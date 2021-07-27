#gen("T")
comp vec<T> (
    ptr<T> buf,
    int len,
    int cap,
);

#gen("T")
def vec<T> {
    vec<T> new_with_cap(int cap) {
        ret new vec<T> {
            buf = mem:alloc(mem:size<T>() * cap),
            len = 0,
            cap = cap,
        };
    };

    #self
    ptr<T> array_get(int index) {
        ret this.buf + index;
    };

    #self
    sub push(T item) {
        (this.buf + this.len)@ = item;
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
    };
};

#req("stdlib.h")
mod mem {
    #bind("malloc")
    sub alloc();

    #bind("realloc")
    sub realloc();

    #bind("sizeof")
    sub size();
};

#gen("T")
comp ptr<T> ();

comp str (
    ptr<u8> buf,
    int len,
);

#gen("T")
enum opt<T> (
    Some(T thing),
    None,
);
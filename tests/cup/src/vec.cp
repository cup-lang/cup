#gen("T")
comp vec {
    ptr<T> buf,
    int len,
    int cap,
};

#gen("T")
def vec<T> {
    vec<T> new(int cap) {
        ret vec<T> {
            buf = mem:alloc(mem:size<T>() * cap),
            len = 0,
            cap = cap,
        };
    };

    #self
    sub push(T item) {
        this.buf[this.len] = item;
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
    };

    #self
    sub empty() {
        this.buf[0] = this.len = 0;
    };
};

sub push_char(ptr<vec<u8>> v, u8 c) {
    v@.buf[v@.len] = c;
    v@.len += 1;

    if v@.len == v@.cap {
        v@.cap *= 2;
        v@.buf = mem:realloc(v@.buf, mem:size<u8>() * v@.cap);
    };

    v@.buf[v@.len] = 0;
};
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
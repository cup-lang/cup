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
    sub push_back(T item) {
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };

        mem:copy(this.buf + 1, this.buf, mem:size<T>() * (this.len - 1));
        this.buf[0] = item;
    };

    #self
    sub join(ptr<T> other) {
        int other_len = str:len(other);
        if other_len == 0 {
            ret;
        };
        int old_len = this.len;
        this.len += other_len;
        while this.len >= this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
        str:copy(this.buf + old_len, other);
    };

    #self
    sub join_vec(vec<T> other) {
        if other.len == 0 {
            ret;
        };
        int old_len = this.len;
        this.len += other.len;
        while this.len >= this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
        mem:copy(this.buf + old_len, other.buf, other.len * mem:size<T>());
    };

    #self
    sub join_back(vec<T> other) {
        if other.len == 0 {
            ret;
        };
        int old_len = this.len;
        this.len += other.len;
        while this.len >= this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
        mem:copy(this.buf + other.len, this.buf, mem:size<T>() * old_len);
        mem:copy(this.buf, other.buf, mem:size<T>() * other.len);
    };

    #self
    sub empty() {
        this.buf[0] = this.len = 0;
    };
};
#gen("T")
comp arr(
    ptr<T> buf,
    int len,
);

#gen("T")
def arr<T> {
    arr<T> new(int len) {
        ret arr<T>(
            buf = mem:alloc(mem:size<T>() * len),
            len = len,
        );
    };
};
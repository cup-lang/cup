comp Location {
    int line,
    int column,
};

Location get_location(ptr<u8> file, int index) {
    Location loc;
    loc.line = 1;
    loc.column = 1;
    for i = 0, (i) < index, i += 1 {
        if file[i] == '\n' {
            loc.line += 1;
            loc.column = 1;
        } else {
            loc.column += 1;
        };
    };
    ret loc;
};

sub print_snippet(arr<u8> file, Location location) {
    fmt:print(" %i | ", location.line);
    int i = location.line;
    int length = 2;
    while i != 0 {
        length += 1;
        i /= 10;
    };
    int line_index = 1;
    ~l for i = 0, (i) < file.len, i += 1 {
        u8 c = file.buf[i];
        if c == '\n' {
            line_index += 1;
            if line_index > location.line {
                ret ~l;
            };
        } elif line_index == location.line {
            putchar(c);
        };
    };
    putchar('\n');
    for i = 0, (i) < length, i += 1 {
        putchar(' ');
    };
    putchar('|');
    for i = 0, (i) < location.column, i += 1 {
        putchar(' ');
    };
    set_color(Color:Red);
    putchar('^');
    set_color(Color:Reset);
    putchar('\n');
};

#rest
sub throw(File file, int index, ptr<u8> error) {
    va_list args;
    va_start(args, error);

    Location loc = get_location(file.data.buf, index);
    fmt:print("%s:%i:%i: ", file.name, loc.line, loc.column);
    set_color(Color:Red);
    fmt:print("error:");
    set_color(Color:Reset);
    fmt:print(" ");
    fmt:vprint(error, args);
    fmt:print("\n");
    print_snippet(file.data, loc);
    exit(1);

    va_end(args);
};
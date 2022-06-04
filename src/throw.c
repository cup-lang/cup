typedef struct Location {
    int line;
    int column;
} Location;

Location get_location(Str data, int index) {
    Location loc;
    loc.line = 1;
    loc.column = 1;
    for (int i = 0; i < index; ++i) {
        if (data.buf[i] == '\n') {
            ++loc.line;
            loc.column = 1;
        } else {
            ++loc.column;
        }
    }
    return loc;
}

void print_snippet(Str data, Location location) {
    printf(" %i | ", location.line);
    int i = location.line;
    int length = 2;
    while (i) {
        ++length;
        i /= 10;
    }
    int line_index = 1;
    for (int i = 0; i < data.len; ++i) {
        char c = data.buf[i];
        if (c == '\n') {
            if (++line_index > location.line) {
                break;
            }
        } else if (line_index == location.line) {
            putchar(c);
        }
    }
    putchar('\n');
    for (int i = 0; i < length; ++i) {
        putchar(' ');
    }
    putchar('|');
    for (int i = 0; i < location.column; ++i) {
        putchar(' ');
    }
    COLOR(RED);
    putchar('^');
    COLOR(RESET);
    putchar('\n');
}

#define THROW(file, index, error, ...)                                 \
{                                                                      \
    Location loc = get_location(file.data, index);                     \
    printf("%s:%i:%i: ", file.path.buf, loc.line, loc.column);         \
    COLOR(RED);                                                        \
    printf("error: ");                                                 \
    COLOR(RESET);                                                      \
    printf(error, __VA_ARGS__);                                        \
    printf("\n");                                                      \
    print_snippet(file.data, loc);                                     \
    exit(1);                                                           \
}
// this line can't be empty because C
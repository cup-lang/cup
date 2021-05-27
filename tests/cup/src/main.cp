` TODO: use errors
` FIX: "(a) < 1" -> "a < 1"
` FIX: "ret (a + 1)" -> "ret a + 1"
` FIX: use "var"

#req("stdio.h")
#req("stdint.h")
#req("string.h")
#req("ctype.h")
#req("time.h")
mod _ {};

#req("stdlib.h")
mod mem {
    #bind("malloc") sub alloc() {};

    #bind("sizeof") sub size() {}; 
};

ptr<u8> file_name = none;
int file_size;

int main(int argc, ptr<ptr<u8>> argv) {
    #os("win") console = GetStdHandle(STD_OUTPUT_HANDLE);

    Command command = get_command(argc, argv);
    ~m match command {
        Command:None {
            set_color(Color:Red);
            printf("error: ");
            set_color(Color:Reset);
            printf("no such command: '%s", argv[1]);
            for i = 2, (i) < argc, i += 1 {
                printf(" %s", argv[i]);
            };
            printf("'\n\nSee 'cup help' for the list of available commands.\n");
            ret 1;
        },
        Command:Run { ret ~m; },
        Command:Build { ret ~m; },
        Command:Check { ret 1; },
        Command:NewPackage { ret 1; },
        Command:UpdatePackage { ret 1; },
        Command:AddPackage { ret 1; },
        Command:RemovePackage { ret 1; },
        Command:GenDocs { ret 1; },
        Command:GenBinds { ret 1; },
        Command:SelfUpdate { ret 1; },
        Command:SelfInstall { ret 1; },
        Command:SelfUninstall { ret 1; },
        Command:Help {
            printf("Cup Toolkit v0.0.1\n\n");
            printf("USAGE:\n    cup [COMMAND] [OPTIONS]");
            printf("\n\nCOMMANDS:");
            printf("\n    run                  Compile and run the current package");
            printf("\n    build                Compile the current package");
            printf("\n    check                Analyze the current package");
            printf("\n    new [PACKAGE]        Create a new package");
            printf("\n    update [PACKAGE]     Update given dependency");
            printf("\n    add [PACKAGE]        Adds given dependency");
            printf("\n    remove [PACKAGE]     Removes given dependency");
            printf("\n    gen docs             Generate documentation for the current package");
            printf("\n    gen binds [HEADER]   Generate bindings for a given C header file");
            printf("\n    self update          Update the Cup Toolkit");
            printf("\n    self install         Install the Cup Toolkit");
            printf("\n    self uninstall       Uninstall the Cup Toolkit");
            printf("\n\nSee 'cup help [COMMAND]' for more info about a specific command and it's available options.\n");
            ret 0;
        },
        Command:HelpRun {
            command_help("run", "Compile and run the current package", command_options:compile);
            ret 0;
        },
        Command:HelpBuild {
            command_help("build", "Compile the current package", command_options:compile);
            ret 0;
        },
        Command:HelpCheck {
            command_help("check", "Analyze the current package", command_options:empty);
            ret 0;
        },
        Command:HelpNewPackage {
            command_help("new", "Create a new package", command_options:empty);
            ret 0;
        },
        Command:HelpUpdatePackage {
            command_help("update", "Update given dependency", command_options:empty);
            ret 0;
        },
        Command:HelpAddPackage {
            command_help("add", "Adds given dependency", command_options:empty);
            ret 0;
        },
        Command:HelpRemovePackage {
            command_help("remove", "Removes given dependency", command_options:empty);
            ret 0;
        },
        Command:HelpGenDocs {
            command_help("gen docs", "Generate documentation for the current package", command_options:empty);
            ret 0;
        },
        Command:HelpGenBinds {
            command_help("gen binds", "Generate bindings for a given C header file", command_options:empty);
            ret 0;
        },
        Command:HelpSelfUpdate {
            command_help("self update", "Update the Cup Toolkit", command_options:empty);
            ret 0;
        },
        Command:HelpSelfInstall {
            command_help("self install", "Install the Cup Toolkit", command_options:empty);
            ret 0;
        },
        Command:HelpSelfUninstall {
            command_help("self uninstall", "Uninstall the Cup Toolkite", command_options:empty);
            ret 0;
        },
    };

    ptr<u8> output = none;
    for i = 2; (i) < argc; i += 1 {
        if argv[i][0] == '-' {
            if argv[i][1] == 'i' {
                if file_name == none {
                    file_name = get_option(i$, argc, argv);
                };
            } elif argv[i][1] == 'o' {
                if output == none {
                    output = get_option(i$, argc, argv);
                };
            };
        } else {
            set_color(Color:Red);
            printf("error: ");
            set_color(Color:Reset);
            printf("invalid option '%s'", argv[i]);
        };
    };

    `` Open the file
    ptr<FILE> file_point;
    if fopen_s(file_point$, file_name, "rb") {
        set_color(Color:Red);
        printf("error: ");
        set_color(Color:Reset);
        printf("no such file or directory: '%s'", file_name);
        ret 1;
    };

    `` Get the size of the file
    fseek(file_point, 0 as i32, SEEK_END);
    file_size = ftell(file_point);
    rewind(file_point);

    `` Allocate the buffer, read contents and close the file
    ptr<u8> file = mem:alloc(file_size);
    fread(file, file_size, 1, file_point);
    fclose(file_point);

    `` Tokenize the file
    vec<lexer:Token> tokens = lexer:lex();

    `` Parse the tokens
    vec<parser:Expr> ast = parser:parse(tokens);

    `` Generate output file
    if output != none {
        fopen_s(file_point$, output, "w");
    } else {
        fopen_s(file_point$, "out.c", "w");
    };
    fputs("#include <stdint.h>\n", file_point);
    gen:generate_vector(ast);
    fclose(file_point);

    system("cc test/test0/out.c -o test/test0/out");

    printf("Compilation successful (%.3lfs elapsed)\n", clock() as f64 / CLOCKS_PER_SEC);

    ret 0;
};

ptr<u8> get_option(ptr<int> index, int argc, ptr<ptr<u8>> argv) {
    if strlen(argv[index@]) > 2 {
        ret (argv[index@] + 1);
    } elif (argc > index@) {
        ret (argv[index@ += 1]);
    };
    ret none;
};

enum Color {
    Reset,
    Red,
};

#os("win")
sub set_color(Color color) {
    int color_code; 
    match color {
        Color:Reset { color_code = 7; },
        Color:Red { color_code = 12; },
    };

    SetConsoleTextAttribute(console, color_code);   
};

#os("linux")
sub set_color(Color color) {
    match color {
        Color:Reset { printf("\033[0m"); },
        Color:Red { printf("\033[0;31m"); },
    };
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

        if this.size == this.capacity {
            this.capacity *= 2;
            this.buf = mem:realloc(v.buf, mem:size<T>() * this.capacity);
        };
    };
};
#os("win") HANDLE console;
ptr<u8> file_name = none;
int file_size;

int main(int argc, ptr<ptr<u8>> argv) {
    #os("win") console = GetStdHandle(STD_OUTPUT_HANDLE);

    Command command = get_command(argc, argv);
    ~m match command {
        Command:None {
            set_color(Color:Red);
            fmt:print("error: ");
            set_color(Color:Reset);
            fmt:print("no such command: '%s", argv[1]);
            for i = 2, (i) < argc, i += 1 {
                fmt:print(" %s", argv[i]);
            };
            fmt:print("'\n\nSee 'cup help' for the list of available commands.\n");
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
            fmt:print("Cup Toolkit v0.0.1\n\n");
            fmt:print("USAGE:\n    cup [COMMAND] [OPTIONS]");
            fmt:print("\n\nCOMMANDS:");
            fmt:print("\n    run                  Compile and run the current package");
            fmt:print("\n    build                Compile the current package");
            fmt:print("\n    check                Analyze the current package");
            fmt:print("\n    new [PACKAGE]        Create a new package");
            fmt:print("\n    update [PACKAGE]     Update given dependency");
            fmt:print("\n    add [PACKAGE]        Adds given dependency");
            fmt:print("\n    remove [PACKAGE]     Removes given dependency");
            fmt:print("\n    gen docs             Generate documentation for the current package");
            fmt:print("\n    gen binds [HEADER]   Generate bindings for a given C header file");
            fmt:print("\n    self update          Update the Cup Toolkit");
            fmt:print("\n    self install         Install the Cup Toolkit");
            fmt:print("\n    self uninstall       Uninstall the Cup Toolkit");
            fmt:print("\n\nSee 'cup help [COMMAND]' for more info about a specific command and it's available options.\n");
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
            fmt:print("error: ");
            set_color(Color:Reset);
            fmt:print("invalid option '%s'", argv[i]);
        };
    };

    `` Open the file
    ptr<FILE> file_point = file:open(file_name, "rb");
    if file_point == none {
        set_color(Color:Red);
        fmt:print("error: ");
        set_color(Color:Reset);
        fmt:print("no such file or directory: '%s'", file_name);
        ret 1;
    };

    `` Get the size of the file
    file:seek(file_point, 0 as i32, SEEK_END);
    file_size = file:size(file_point);
    file:rewind(file_point);

    `` Allocate the buffer, read contents and close the file
    arr<u8> file = arr<u8>:new(file_size);
    file:read(file.buf, file_size, 1, file_point);
    file:close(file_point);

    `` Tokenize the file
    vec<Token> tokens = lex(file);
    print_tokens(tokens);

    `` Parse the tokens
    vec<Expr> ast = parse(tokens);

    `` Generate output file
    ` if output != none {
    `     file_point = file:open(output, "w");
    ` } else {
    `     file_point = file:open("out.c", "w");
    ` };
    ` generate_vector(ast);
    ` file:close(file_point);

    fmt:print("Compilation successful (%.3lfs elapsed)\n", (clock() as f64) / CLOCKS_PER_SEC);

    ret 0;
};

ptr<u8> get_option(ptr<int> index, int argc, ptr<ptr<u8>> argv) {
    if str:len(argv[index@]) > 2 {
        ret (argv[index@] + 1);
    } elif (argc > index@) {
        ret (argv[index@ += 1]);
    };
    ret none;
};

enum Color {
    Reset,
    Magenta,
    Green,
    Red,
};

#os("win")
sub set_color(Color color) {
    int color_code; 
    match color {
        Color:Reset { color_code = 7; },
        Color:Magenta { color_code = 5; },
        Color:Green { color_code = 10; },
        Color:Red { color_code = 12; },
    };

    SetConsoleTextAttribute(console, color_code);   
};

#os("linux")
sub set_color(Color color) {
    match color {
        Color:Reset { fmt:print("\033[0m"); },
        Color:Magenta { fmt:print("\033[35m"); },
        Color:Green { fmt:print("\033[32m"); },
        Color:Red { fmt:print("\033[0;31m"); },
    };
};
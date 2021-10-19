#os("win") HANDLE console;

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

    ptr<u8> input = none;
    ptr<u8> output = none;
    for i = 2, (i) < argc, i += 1 {
        if argv[i][0] == '-' {
            if argv[i][1] == 'i' {
                if input == none {
                    input = get_option(i$, argc, argv);
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
    if input == none {
        input = ".";
    };
    if output == none {
        output = "out.c";
    };

    vec<Expr> ast = vec<Expr>:new(8);
    init_analyzer();
    lex_parse_analyze_recursive(input, ast$);
    generate(output, ast);

    fmt:print("Compilation ");
    set_color(Color:Green);
    fmt:print("successful");
    set_color(Color:Reset);
    fmt:print(" (%.2lfs elapsed)\n", (clock() as f64) / CLOCKS_PER_SEC);

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

sub lex_parse_analyze_recursive(ptr<u8> path, ptr<vec<Expr>> ast) {
    ptr<DIR> dir = dir:open(path);
    if dir == none {
        set_color(Color:Red);
        fmt:print("error: ");
        set_color(Color:Reset);
        fmt:print("no such file or directory: '%s'\n", path);
        exit(1);
    };
    ptr<dirent> ent;
    while (ent = dir:read(dir)) != none {
        int new_length = str:len(ent@.d_name);
        if (new_length == 1) & (ent@.d_name[0] == '.') { }
        elif ((new_length == 2) & (ent@.d_name[0] == '.')) & (ent@.d_name[1] == '.') { }
        elif ent@.d_type == DT_DIR {
            int length = str:len(path);
            ptr<u8> new_path = mem:alloc(length + 1 + new_length + 1);
            mem:copy(new_path, path, length);
            new_path[length] = '/';
            mem:copy(new_path + length + 1, ent@.d_name, new_length);
            new_path[length + new_length + 1] = '\0';
            lex_parse_analyze_recursive(new_path, ast);
        };
    };
    dir:rewind(dir);
    vec<ToAnalyze> to_analyze = vec<ToAnalyze>:new(4);
    while (ent = dir:read(dir)) != none {
        int new_length = str:len(ent@.d_name);
        if (new_length == 1) & (ent@.d_name[0] == '.') { }
        elif ((new_length == 2) & (ent@.d_name[0] == '.')) & (ent@.d_name[1] == '.') { }
        else {
            int length = str:len(path);
            ptr<u8> new_path = mem:alloc(length + 1 + new_length + 1);
            mem:copy(new_path, path, length);
            new_path[length] = '/';
            mem:copy(new_path + length + 1, ent@.d_name, new_length);
            new_path[length + new_length + 1] = '\0';
            if ent@.d_type == DT_REG {
                ptr<FILE> file_point = file:open(new_path, "r");
                file:seek(file_point, 0 as i32, SEEK_END);
                arr<u8> data = arr<u8>:new(ftell(file_point) + 1);
                rewind(file_point);
                data.len -= 1;
                file:read(data.buf, data.len, 1, file_point);
                data.buf[data.len] = '\0';
                file:close(file_point);
                
                fmt:print("Compiling %s:\n", new_path);
                File file = File {
                    name = new_path,
                    data = data,
                };
                vec<Token> tokens = lex(file);
                ` print_tokens(tokens);
                vec<Expr> exprs = parse(file, tokens);
                mem:free(tokens.buf);
                ` print_exprs(exprs);
                analyze_global_vec(file, exprs);
                to_analyze.push(ToAnalyze {
                    file = file,
                    exprs = exprs,
                });
                
                vec<Expr>:push(ast, Expr {
                    tags = vec<Expr> {
                        len = 0,
                    },
                    label = none,
                    kind = ExprKind:Block(exprs),
                });
            };
        };
    };
    for i = 0, (i) < to_analyze.len, i += 1 {
        ToAnalyze analyze = to_analyze.buf[i];
        analyze_local_vec(analyze.file, analyze.exprs, true);
        mem:free(analyze.file.name);
        mem:free(analyze.file.data.buf);
    };
    mem:free(to_analyze.buf);
    dir:close(dir);
};

comp File {
    ptr<u8> name,
    arr<u8> data,
};

comp ToAnalyze {
    File file,
    vec<Expr> exprs,
};
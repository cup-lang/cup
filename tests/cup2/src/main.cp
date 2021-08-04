int main(int argc, ptr<ptr<u8>> argv) {
    #os("win") console = get_std_handle(std_output_handle);

    arr<str> args = arr<str>:new_with_len(argc);
    for i = 0, i < args.len, i += 1 {
        args[i] = str:new_from_cstr((argv + i)@);
    };

    Command command = get_command(args);
    ~m match command {
        Command:None {
            color:set(Color:Red);
            fmt:print("error: ");
            color:reset();
            fmt:print("no such command: '%s", args[1]);
            for i = 2, i < argc, i += 1 {
                fmt:print(" %s", args[i]);
            };
            fmt:print("'\n\nSee 'cup help' for the list of available commands.\n");
            ret 1;
        },
        Command:Run,
        Command:Build {
            ret ~m;
        },
        Command:Check,
        Command:NewPackage,
        Command:UpdatePackage,
        Command:AddPackage,
        Command:RemovePackage,
        Command:GenDocs,
        Command:GenBinds,
        Command:SelfUpdate,
        Command:SelfInstall,
        Command:SelfUninstall {
            ret 1;
        },
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

    opt<str> input = opt<str>:None;
    opt<str> output = opt<str>:None;
    for i = 2, i < args.len, i += 1 {
        if (cstr:cmp(args[i].buf, "-i") == 0) | (cstr:cmp(args[i].buf, "--input") == 0) {
            expect_option_value(input$, args, i += 1);
        } elif (cstr:cmp(args[i].buf, "-o") == 0) | cstr:cmp(args[i].buf, "--output") == 0 {
            expect_option_value(output$, args, i += 1);
        } else {
            color:set(Color:Red);
            fmt:print("error: ");
            color:reset();
            fmt:print("invalid option '%s'\n", args[i].buf);
            ret 1;
        };
    };
    if input == opt:None {
        input = opt<str>:Some(str:new_from_cstr("."));
    };
    if output == opt:None {
        output = opt<str>:Some(str:new_from_cstr("out.c"));
    };

    vec<Expr> ast = vec<Expr>:new_with_cap(8);
    lex_parse_analyze(input.unwrap(), ast$);

    fmt:print("Compilation ");
    color:set(Color:Green);
    fmt:print("successful");
    color:reset();
    fmt:print(" (%.2lfs elapsed)\n", time:now());

    ret 0;
};

sub expect_option_value(ptr<opt<str>> option, arr<str> args, int index) {
    if args.len > index {
        option@ = opt<str>:Some(args[index]);
    } else {
        throw("missing value after '%s' option", args[index - 1].buf);
    };
};

sub lex_parse_analyze(str path, ptr<vec<Expr>> ast) {
    ptr<dir> _dir = dir:open(path.buf);
    if _dir == 0 {
        throw("no such file or directory: '%s'\n", path.buf);
    };
    ptr<dir:ent> ent;
    ~d loop {
        ent = dir:read(_dir);
        if ent == 0 {
            ret ~d;
        };
        if ent@.d_type == dir:kind:dir {
            if (cstr:cmp(ent@.d_name, ".") == 0) | (cstr:cmp(ent@.d_name, "..") == 0) {
                next ~d;
            };
            str new_path = combine_paths(path, str:new_from_cstr(ent@.d_name));
            lex_parse_analyze(new_path, ast);
            mem:free(new_path.buf);
        };
    };
    dir:rewind(_dir);
    ~f loop {
        ent = dir:read(_dir);
        if ent == 0 {
            ret ~f;
        };
        if ent@.d_type == dir:kind:file {
            str new_path = combine_paths(path, str:new_from_cstr(ent@.d_name));
            ptr<file> file_point = file:open(new_path.buf, "r");
            file:seek(file_point, 0, file:seek_end);
            str data = str:new_with_len(file:tell(file_point) - 1);
            file:rewind(file_point);
            file:read(data.buf, data.len + 1, 1, file_point);
            file:close(file_point);

            fmt:print("Compiling %s:\n", new_path);
            File file = new File {
                name = new_path,
                data = data,
            };
            vec<Token> tokens = lex(file);
            ` print_tokens(tokens);
            ` vec<Expr> exprs = parse(file, tokens);
            ` mem:free(tokens.buf);
            ` print_exprs(exprs);
            ` analyze_global_vec(file, exprs);
            ` to_analyze.push(ToAnalyze {
            `     file = file,
            `     exprs = exprs,
            ` });
            ` 
            ` vec<Expr>:push(ast, Expr {
            `     tags = vec<Expr> {
            `         len = 0,
            `     },
            `     label = none,
            `     kind = ExprKind:Block(exprs),
            ` });
        };
    };
    dir:close(_dir);



    ` dir:rewind(dir);
    ` vec<ToAnalyze> to_analyze = vec<ToAnalyze>:new(4);
    ` for i = 0, i < to_analyze.len, i += 1 {
    `     ToAnalyze analyze = to_analyze.buf[i];
    `     analyze_local_vec(analyze.file, analyze.exprs, true);
    `     mem:free(analyze.file.name);
    `     mem:free(analyze.file.data.buf);
    ` };
    ` mem:free(to_analyze.buf);
};

str combine_paths(str a, str b) {
    str new_path = str:new_with_len(a.len + 1 + b.len);
    mem:copy(new_path.buf, a.buf, mem:size<u8>() * a.len);
    new_path[a.len] = '/';
    mem:copy(new_path.buf + a.len + 1, b.buf, mem:size<u8>() * b.len);
    new_path[new_path.len] = '\0';
    ret new_path;
};

comp File (
    str name,
    str data,
);

def File {
    #self #rest
    sub throw(int index, ptr<u8> error) {
        rest:args args;
        rest:start(args, error);

        int line = 1;
        int column = 1;
        for i = 0, i < index, i += 1 {
            if this.data[i] == '\n' {
                line += 1;
                column = 1;
            } else {
                column += 1;
            };
        };

        fmt:print("%s:%i:%i: ", this.name.buf, line, column);
        color:set(Color:Red);
        fmt:print("error:");
        color:reset();
        fmt:print(" ");
        fmt:vprint(error, args);
        fmt:print("\n");
        print_snippet(this.data, line, column);
        exit(1);

        rest:end(args);
    };
};

sub print_snippet(str file, int line, int column) {
    fmt:print(" %i | ", line);
    int length = 2;
    int l = line;
    while l != 0 {
        length += 1;
        l /= 10;
    };
    int line_index = 1;
    ~l for i = 0, i < file.len, i += 1 {
        u8 c = file[i];
        if c == '\n' {
            line_index += 1;
            if line_index > line {
                ret ~l;
            };
        } elif line_index == line {
            char:put(c);
        };
    };
    char:put('\n');
    for i = 0, i < length, i += 1 {
        char:put(' ');
    };
    char:put('|');
    for i = 0, i < column, i += 1 {
        char:put(' ');
    };
    color:set(Color:Red);
    char:put('^');
    color:reset();
    char:put('\n');
};
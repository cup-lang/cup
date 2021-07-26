int main(int argc, ptr<ptr<u8>> argv) {
    #os("win") console = get_std_handle(std_output_handle);

    arr<ptr<u8>> args = arr<ptr<u8>>:new_with_len(argc);
    ` for i = 0, i < args.len, i += 1 {
    `     args[i]@ = str:new_from_cstr(argv + i);
    ` };

    Command command = get_command(args);
    ~m match command {
        Command:None {
            set_color(Color:Red);
            fmt:print("error: ");
            set_color(Color:Reset);
            ` fmt:print("no such command: '%s", argv[1]);
            ` for i = 2, (i) < argc, i += 1 {
            `     fmt:print(" %s", argv[i]);
            ` };
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

    ` ptr<u8> input = none;
    ` ptr<u8> output = none;
    for i = 2, i < argc, i += 1 {
        ` if argv[i][0] == '-' {
        `     if argv[i][1] == 'i' {
        `         if input == none {
        `             input = get_option(i$, argc, argv);
        `         };
        `     } elif argv[i][1] == 'o' {
        `         if output == none {
        `             output = get_option(i$, argc, argv);
        `         };
        `     };
        ` } else {
        `     set_color(Color:Red);
        `     fmt:print("error: ");
        `     set_color(Color:Reset);
        `     fmt:print("invalid option '%s'", argv[i]);
        ` };
    };
    ` if input == none {
    `     input = ".";
    ` };
    ` if output == none {
    `     output = "out.c";
    ` };

    fmt:print("Compilation ");
    set_color(Color:Green);
    fmt:print("successful");
    set_color(Color:Reset);
    fmt:print(" (%.2lfs elapsed)\n", time:now());

    ret 0;
};

comp foo (
    ptr<u8> buf,
    int len,
);

def foo {
    foo new_from_cstr(ptr<u8> buf, int len) {
        ret new foo {
            buf = buf,
            len = len,
        };
    };

    #self
    u8 array_get(int index) {
        ret this.buf + index;
    };
};

sub b() {
    arr<foo> foo2 = arr<foo>:new_with_len(2);
};
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
            for i = 2, (i) < argc, i += 1 {
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
    match input {
        opt:None {
            input = opt<str>:Some(str:new_from_cstr("."));
        },
    };
    match output {
        opt:None {
            output = opt<str>:Some(str:new_from_cstr("out.c"));
        },
    };

    fmt:print("Compilation ");
    color:set(Color:Green);
    fmt:print("successful");
    color:reset();
    fmt:print(" (%.2lfs elapsed)\n", time:now());

    ret 0;
};

comp File (
    ptr<u8> name,
    arr<u8> data,
);

sub expect_option_value(ptr<opt<str>> option, arr<str> args, int index) {
    if args.len > index {
        option@ = opt<str>:Some(args[index]);
    } else {
        throw("missing value after '%s' option", args[index - 1].buf);
    };
};

#rest
sub throw(ptr<u8> error) {
    rest:args args;
    rest:start(args, error);

    color:set(Color:Red);
    fmt:print("error:");
    color:reset();
    fmt:print(" ");
    fmt:vprint(error, args);
    fmt:print("\n");
    exit(1);

    rest:end(args);
};
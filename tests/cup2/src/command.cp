sub command_help(ptr<u8> n, ptr<u8> d, ptr<u8> o) {
    fmt:print(d);
    fmt:print("\n\nUSAGE:\n    cup ");
    fmt:print(n);
    fmt:print(" [OPTIONS]\n\nOPTIONS:");
    fmt:print(o);
    fmt:print("\n");
};

mod command_options {
    ptr<u8> empty = "\n    No options are available for this command";

    ptr<u8> compile =
        "\n    -i, --input     Specify the input file name" +
        "\n    -o, --output    Specify the output file name" +
        "\n    -cg, --gcc     Use GCC as a compiler" +
        "\n    -cm, --msvc    Use MSVC as a compiler" +
        "\n    -cc, --clang   use Clang as a compiler" +
        "\n    -ct, --tcc     Use TinyCC as a compiler";
};

enum Command (
    None,
    Help,
    HelpRun,
    HelpBuild,
    HelpCheck,
    HelpNewPackage,
    HelpUpdatePackage,
    HelpAddPackage,
    HelpRemovePackage,
    HelpGenDocs,
    HelpGenBinds,
    HelpSelfUpdate,
    HelpSelfInstall,
    HelpSelfUninstall,
    Run,
    Build,
    Check,
    NewPackage,
    UpdatePackage,
    AddPackage,
    RemovePackage,
    GenDocs,
    GenBinds,
    SelfUpdate,
    SelfInstall,
    SelfUninstall,
);

Command get_command(arr<str> args) {
    if args.len == 1 {
        ret Command:Help;
    };

    bool is_command = false;
    dstr string = dstr:new_with_cap(16);
    ~l for i = 1, i < args.len, i += 1 {
        if i == 4 {
            ret ~l;
        };

        if args[i][0] == '-' {
            if is_command {
                ret ~l;
            };
            next ~l;
        };
        is_command = true;

        for c = 0, c < args[i].len, c += 1 {
            string.push(args[i][c]);
        };
    };

    ptr<u8> input = string.buf;

    if cstr:cmp(input, "help") == 0 {
        ret Command:Help;
    } elif cstr:cmp(input, "helprun") == 0 {
        ret Command:HelpRun;
    } elif cstr:cmp(input, "helpbuild") == 0 {
        ret Command:HelpBuild;
    } elif cstr:cmp(input, "helpcheck") == 0 {
        ret Command:HelpCheck;
    } elif cstr:cmp(input, "helpupdate") == 0 {
        ret Command:HelpUpdatePackage;
    } elif cstr:cmp(input, "helpadd") == 0 {
        ret Command:HelpAddPackage;
    } elif cstr:cmp(input, "helpremove") == 0 {
        ret Command:HelpRemovePackage;
    } elif cstr:cmp(input, "helpgendocs") == 0 {
        ret Command:HelpGenDocs;
    } elif cstr:cmp(input, "helpgenbinds") == 0 {
        ret Command:HelpGenBinds;
    } elif cstr:cmp(input, "helpselfupdate") == 0 {
        ret Command:HelpSelfUpdate;
    } elif cstr:cmp(input, "helpselfinstall") == 0 {
        ret Command:HelpSelfInstall;
    } elif cstr:cmp(input, "helpselfuninstall") == 0 {
        ret Command:HelpSelfUninstall;
    } elif cstr:cmp(input, "run") == 0 {
        ret Command:Run;
    } elif cstr:cmp(input, "build") == 0 {
        ret Command:Build;
    } elif cstr:cmp(input, "check") == 0 {
        ret Command:Check;
    } elif cstr:cmp(input, "update") == 0 {
        ret Command:UpdatePackage;
    } elif cstr:cmp(input, "add") == 0 {
        ret Command:AddPackage;
    } elif cstr:cmp(input, "remove") == 0 {
        ret Command:RemovePackage;
    } elif cstr:cmp(input, "gendocs") == 0 {
        ret Command:GenDocs;
    } elif cstr:cmp(input, "genbinds") == 0 {
        ret Command:GenBinds;
    } elif cstr:cmp(input, "selfupdate") == 0 {
        ret Command:SelfUpdate;
    } elif cstr:cmp(input, "selfinstall") == 0 {
        ret Command:SelfInstall;
    } elif cstr:cmp(input, "selfuninstall") == 0 {
        ret Command:SelfUninstall;
    };

    ret Command:None;
};
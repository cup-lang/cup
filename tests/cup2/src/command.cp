sub command_help(ptr<u8> n, ptr<u8> d, ptr<u8> o) {
    fmt:print(d);
    fmt:print("\n\nUSAGE:\n    cup ");
    fmt:print(n);
    fmt:print(" [OPTIONS]\n\nOPTIONS:");
    fmt:print(o);
    fmt:print("\n");
};

#inl mod command_options {
    ` ptr<u8> empty = "\n    No options are available for this command";

    ` ptr<u8> compile = "\n    -i, --input     Specify the input file name" +
    `               "\n    -o, --output    Specify the output file name" +
    `               "\n    -cg, --gcc     Use GCC as a compiler" +
    `               "\n    -cm, --msvc    Use MSVC as a compiler" +
    `               "\n    -cc, --clang   use Clang as a compiler" +
    `               "\n    -ct, --tcc     Use TinyCC as a compiler";
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

Command get_command(int argc, ptr<ptr<u8>> argv) {
    if argc == 1 {
        ret Command:Help;
    };

    bool is_command = false;
    vec<u8> vector = vec<u8>:new(16);
    ~l for i = 1, (i) < argc, i += 1 {
        if i == 4 {
            ret ~l;
        };
    
        if argv[i][0] == '-' {
            if is_command {
                ret ~l;
            };
            next ~l;
        };
        is_command = true;
    
        int length = str:len(argv[i]);
        for c = 0, (c) < length, c += 1 {
            vector.push(argv[i][c]);
        };
    };

    vector.push('\0');
    ptr<u8> input = vector.buf;

    if str:cmp(input, "help") == 0 {
        ret Command:Help;
    } elif str:cmp(input, "helprun") == 0 {
        ret Command:HelpRun;
    } elif str:cmp(input, "helpbuild") == 0 {
        ret Command:HelpBuild;
    } elif str:cmp(input, "helpcheck") == 0 {
        ret Command:HelpCheck;
    } elif str:cmp(input, "helpupdate") == 0 {
        ret Command:HelpUpdatePackage;
    } elif str:cmp(input, "helpadd") == 0 {
        ret Command:HelpAddPackage;
    } elif str:cmp(input, "helpremove") == 0 {
        ret Command:HelpRemovePackage;
    } elif str:cmp(input, "helpgendocs") == 0 {
        ret Command:HelpGenDocs;
    } elif str:cmp(input, "helpgenbinds") == 0 {
        ret Command:HelpGenBinds;
    } elif str:cmp(input, "helpselfupdate") == 0 {
        ret Command:HelpSelfUpdate;
    } elif str:cmp(input, "helpselfinstall") == 0 {
        ret Command:HelpSelfInstall;
    } elif str:cmp(input, "helpselfuninstall") == 0 {
        ret Command:HelpSelfUninstall;
    } elif str:cmp(input, "run") == 0 {
        ret Command:Run;
    } elif str:cmp(input, "build") == 0 {
        ret Command:Build;
    } elif str:cmp(input, "check") == 0 {
        ret Command:Check;
    } elif str:cmp(input, "update") == 0 {
        ret Command:UpdatePackage;
    } elif str:cmp(input, "add") == 0 {
        ret Command:AddPackage;
    } elif str:cmp(input, "remove") == 0 {
        ret Command:RemovePackage;
    } elif str:cmp(input, "gendocs") == 0 {
        ret Command:GenDocs;
    } elif str:cmp(input, "genbinds") == 0 {
        ret Command:GenBinds;
    } elif str:cmp(input, "selfupdate") == 0 {
        ret Command:SelfUpdate;
    } elif str:cmp(input, "selfinstall") == 0 {
        ret Command:SelfInstall;
    } elif str:cmp(input, "selfuninstall") == 0 {
        ret Command:SelfUninstall;
    };

    ret Command:None;
};
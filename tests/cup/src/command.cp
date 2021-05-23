sub command_help(ptr<u8> n, ptr<u8> d, ptr<u8> o) {
    printf(d);
    printf("\n\nUSAGE:\n    cup ");
    printf(n);
    printf(" [OPTIONS]\n\nOPTIONS:");
    printf(o);
    printf("\n");
};

#inl mod command_options {
    ptr<u8> empty = "\n    No options are available for this command";

    ptr<u8> compile = "\n    -i, --input     Specify the input file name" +
                   "\n    -o, --output    Specify the output file name" +
                   "\n    -cg, --gcc     Use GCC as a compiler" +
                   "\n    -cm, --msvc    Use MSVC as a compiler" +
                   "\n    -cc, --clang   use Clang as a compiler" +
                   "\n    -ct, --tcc     Use TinyCC as a compiler";
};

enum Command {
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
};

Command get_command(int argc, ptr<ptr<u8>> argv) {
    if argc == 1 {
        ret Command:None;
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
    
        int length = strlen(argv[i]);
        for c = 0, (c) < length, c += 1 {
            vector.push(argv[i][c]);
        };
    };

    vector.push('\0');
    ptr<u8> input = vector.buf;

    if strcmp(input, "help") == 0 {
        ret Command:Help;
    } elif strcmp(input, "helprun") == 0 {
        ret Command:HelpRun;
    } elif strcmp(input, "helpbuild") == 0 {
        ret Command:HelpBuild;
    } elif strcmp(input, "helpcheck") == 0 {
        ret Command:HelpCheck;
    } elif strcmp(input, "helpupdate") == 0 {
        ret Command:HelpUpdatePackage;
    } elif strcmp(input, "helpadd") == 0 {
        ret Command:HelpAddPackage;
    } elif strcmp(input, "helpremove") == 0 {
        ret Command:HelpRemovePackage;
    } elif strcmp(input, "helpgendocs") == 0 {
        ret Command:HelpGenDocs;
    } elif strcmp(input, "helpgenbinds") == 0 {
        ret Command:HelpGenBinds;
    } elif strcmp(input, "helpselfupdate") == 0 {
        ret Command:HelpSelfUpdate;
    } elif strcmp(input, "helpselfinstall") == 0 {
        ret Command:HelpSelfInstall;
    } elif strcmp(input, "helpselfuninstall") == 0 {
        ret Command:HelpSelfUninstall;
    } elif strcmp(input, "run") == 0 {
        ret Command:Run;
    } elif strcmp(input, "build") == 0 {
        ret Command:Build;
    } elif strcmp(input, "check") == 0 {
        ret Command:Check;
    } elif strcmp(input, "update") == 0 {
        ret Command:UpdatePackage;
    } elif strcmp(input, "add") == 0 {
        ret Command:AddPackage;
    } elif strcmp(input, "remove") == 0 {
        ret Command:RemovePackage;
    } elif strcmp(input, "gendocs") == 0 {
        ret Command:GenDocs;
    } elif strcmp(input, "genbinds") == 0 {
        ret Command:GenBinds;
    } elif strcmp(input, "selfupdate") == 0 {
        ret Command:SelfUpdate;
    } elif strcmp(input, "selfinstall") == 0 {
        ret Command:SelfInstall;
    } elif strcmp(input, "selfuninstall") == 0 {
        ret Command:SelfUninstall;
    };

    ret Command:None;
};
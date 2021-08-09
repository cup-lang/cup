sub analyze_global_vec(File file, vec<Expr> exprs) {
    for i = 0, i < exprs.len, i += 1 {
        analyze_global(file, exprs[i]$);
    };
};

sub analyze_global(File file, ptr<Expr> expr) {
    Path path;
    match expr@.kind {
        ExprKind:Block(vec<Expr> body) {
            analyze_global_vec(file, body);
            ret;
        },
        ExprKind:Mod(Path _path, vec<Expr> body) {
            ` int old_mods_len = mods.len;
            ` match _path@.kind {
            `     ExprKind:Path(__path) {
            `         mods.join_vec(__path);
            `     },
            ` };
            analyze_global_vec(file, body);
            ` mods.len = old_mods_len;
            ret;
        },
        ExprKind:Comp(Path _path) {
            ` vec<PathPart>:join_back(_path.parts$, mods);
            ` type_paths.push(_path@);
            path = _path;
        },
        ExprKind:Enum(Path _path) {
            ` vec<PathPart>:join_back(_path.parts$, mods);
            ` type_paths.push(_path@);
            ` enum_paths.push(expr@);
            path = _path;
        },
        ExprKind:Def(Path _prop, _, vec<Expr> body) {
            ` int old_mods_len = mods.len;
            ` match _prop@.kind {
            `     ExprKind:Path(_path) {
            `         mods.join_vec(_path);
            `     },
            ` };
            analyze_global_vec(file, body);
            ` mods.len = old_mods_len;
            ret;
        },
        ExprKind:SubDef(_, Path _path) {
            ` vec<PathPart>:join_back(_path.parts$, mods);
            ` sub_paths.push(_path@);
            path = _path;
        },
        ExprKind:VarDef(opt<Path> _type, Path _path) {
            ` vec<PathPart>:join_back(_path.parts$, mods);
            path = _path;
        },
        _ {
            ret;
        },
    };
};

sub analyze_local_vec(File file, vec<Expr> exprs) {
    for i = 0, i < exprs.len, i += 1 {
        analyze_local(file, exprs[i]$);
    };
};

sub analyze_local(File file, ptr<Expr> expr) {

};
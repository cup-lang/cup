comp MangledExpr (
    Expr expr,
    Path path,
    str name,
);

int mangle_index;
darr<darr<MangledExpr>> paths;
darr<PathPart> mods;

sub init_analyzer() {
    mangle_index = 0;
    paths = darr<darr<MangledExpr>>:new_with_cap(4);
    mods = darr<PathPart>:new_with_cap(4);
};

sub analyze_globals(File file, darr<Expr> exprs) {
    for i = 0, i < exprs.len, i += 1 {
        analyze_global(file, exprs[i]$);
    };
};

sub analyze_global(File file, ptr<Expr> expr) {
    opt<Tag> pub = opt<Tag>:None;
    opt<Tag> bind = opt<Tag>:None;
    bool gen = false;
    for i = 0, i < expr@.tags.len, i += 1 {
        Tag _tag = expr@.tags[i];
        if comp_path_to_str(_tag.path, "pub") {
            if pub != opt<Tag>:None {
                file.throw(_tag.index, "only one #pub is allowed per path");
            };
            pub = opt<Tag>:Some(_tag);
        } elif comp_path_to_str(_tag.path, "bind") {
            if bind != opt<Tag>:None {
                file.throw(_tag.index, "only one #bind is allowed per path");
            };
            bind = opt<Tag>:Some(_tag);
        } elif comp_path_to_str(_tag.path, "gen") {
            gen = true;
        };
    };

    ptr<Path> path;
    match expr@.kind {
        ExprKind:Block(darr<Expr> body) {
            analyze_globals(file, body);
            ret;
        },
        ExprKind:Mod(Path _path, darr<Expr> body) {
            try_apply_pub(pub, _path);
            int old_mods_len = mods.len;
            mods.join(_path.parts);
            analyze_globals(file, body);
            mods.len = old_mods_len;
        },
        ExprKind:Comp(#ref Path _path) {
            try_apply_pub(pub, _path@);
            darr<PathPart>:join_back(_path@.parts$, mods);
            path = _path;
        },
        ExprKind:Enum(#ref Path _path) {
            try_apply_pub(pub, _path@);
            darr<PathPart>:join_back(_path@.parts$, mods);
            path = _path;
        },
        ExprKind:Def(Path _prop, _, darr<Expr> body) {
            try_apply_pub(pub, _prop);
            int old_mods_len = mods.len;
            mods.join(_prop.parts);
            analyze_globals(file, body);
            mods.len = old_mods_len;
        },
        ExprKind:SubDef(_, #ref Path _path) {
            try_apply_pub(pub, _path@);
            darr<PathPart>:join_back(_path@.parts$, mods);
            path = _path;
        },
        ExprKind:VarDef(opt<Path> _type, #ref Path _path) {
            try_apply_pub(pub, _path@);
            darr<PathPart>:join_back(_path@.parts$, mods);
            path = _path;
        },
    };

    match pub {
        opt<Tag>:Some(Tag _tag) {
            match expr@.kind {
                ExprKind:Mod,
                ExprKind:Comp,
                ExprKind:Enum,
                ExprKind:Def,
                ExprKind:SubDef,
                ExprKind:VarDef {},
                _ {
                    file.throw(_tag.index, "#pub cannot be applied to this expr");
                },
            };
        },
    };

    match bind {
        opt<Tag>:Some(Tag _tag) {
            match expr@.kind {
                ExprKind:Comp,
                ExprKind:Enum,
                ExprKind:SubDef,
                ExprKind:VarDef {},
                _ {
                    file.throw(_tag.index, "#bind cannot be applied to this expr");
                },
            };
        },
    };

    if gen {
        ret;
    };

    match expr@.kind {
        ExprKind:Comp,
        ExprKind:Enum,
        ExprKind:SubDef,
        ExprKind:VarDef {
            if path@.parts.len <= paths.len {
                darr<MangledExpr> len_paths = paths[path@.parts.len - 1];
                for i = 0, i < len_paths.len, i += 1 {
                    if compare_paths(path@, len_paths[i].path, true) {
                        file.throw(path@.index, "path already defined");
                    };
                };
            };

            match bind {
                opt<Tag>:Some(Tag _tag) {
                    match expr@.kind {
                        ExprKind:Comp,
                        ExprKind:Enum,
                        ExprKind:SubDef,
                        ExprKind:VarDef {
                            match _tag.args[0].kind {
                                ExprKind:StringLit(str value) {
                                    add_mangled_expr(expr@, path@, value);
                                },
                            };
                        },
                    };
                },
                opt<Tag>:None {
                    add_mangled_expr(expr@, path@, new_mangle(mangle_index += 1, false, 0));
                },
            };
        },
    };
};

sub analyze_locals(File file, darr<Expr> exprs) {
    for i = 0, i < exprs.len, i += 1 {
        analyze_local(file, exprs[i]$);
    };
};

sub analyze_local(File file, ptr<Expr> expr) {

};

sub try_apply_pub(opt<Tag> pub, Path path) {
    if pub != opt<Tag>:None {
        for i = 0, i < path.parts.len, i += 1 {
            path.parts[i].pub = true;
        };
    };
};

str mangle(Path path, bool gens, bool _new) {
    ` if (path.parts.len == 1) & (path.parts[0].gens.len == 0) {
    `     for i = 0, i < mangled_local_names.len, i += 1 {
    `         if str:cmp(path.parts[0].name, mangled_local_names[i].local_name) == 0 {
    `             ret mangled_local_names[i].name;
    `         };
    `     };
    ` };
    if path.parts.len <= paths.len {
        darr<MangledExpr> len_paths = paths[path.parts.len - 1];
        ~l for i = 0, i < len_paths.len, i += 1 {
            ` if compare_paths(path, paths[i].path, gens) {
            `     if _new {
            `         ret none;
            `     } else {
            `         ptr<u8> name = paths[i].name;
            `         if pointer_count > 0 {
            `             int len = str:len(name);
            `             vec<u8> _name = vec<u8>:new(len + pointer_count + 1);
            `             mem:copy(_name.buf, name, len * mem:size<u8>());
            `             _name.len = len;
            `             for i = 0, (i) < pointer_count, i += 1 {
            `                 _name.push('*');
            `             };
            `             _name.buf[_name.len] = '\0';
            `             ret (_name.buf);
            `         } else {
            `             ret name;
            `         };
            `     };
            ` };
        };
    } else {
        while path.parts.len > paths.len {
            paths.push(darr<MangledExpr>:new_with_cap(16));
        };
    };

    int pointer_count = 0;
    Path ptr_path = path;
    while (ptr_path.parts[0].gens.len == 1) & comp_path_to_str(ptr_path, "ptr") {
        ptr_path = ptr_path.parts[0].gens[0];
        pointer_count += 1;
    };

    `` dstr name = new_mangle(mangle_index += 1, false);

    `` darr<MangledExpr>:push(paths[path.parts.len - 1]$, new MangledExpr {
    ``     ` expr = expr,
    ``     path = path,
    ``     name = name.to_str(),
    `` });

    `` if pointer_count > 0 {
    ``     str old_name = name.to_str();
    ``     name = dstr:new_with_cap(name.len + pointer_count);
    ``     name.join(old_name);
    ``     for i = 0, i < pointer_count, i += 1 {
    ``         name.push('*');
    ``     };
    `` };

    `` ret name.to_str();
};

str new_mangle(int index, bool local, int pointer_count) {
    dstr _name = dstr:new_with_cap(4);
    
    _name.push('_');
    if local {
        _name.push('_');
    };

    while index > 0 {
        int i = index % 62;
        if i < 10 {
            i += 48;
        } elif i < 36 {
            i += 55;
        } else {
            i += 61;
        };
        _name.push(i);
        index /= 62;
    };

    for i = 0, i < pointer_count, i += 1 {
        _name.push('*');
    };

    ret _name.to_str();
};

bool comp_path_to_str(Path path, ptr<u8> name) {
    ret (path.parts.len == 1) & (cstr:cmp(path.parts[0].name.buf, name) == 0);
};

bool compare_paths(Path a, Path b, bool gens) {
    if a.parts.len != a.parts.len {
        ret false;
    };
    
    ~l for i = 0, i < a.parts.len, i += 1 {
        PathPart aPart = a.parts[i];
        PathPart bPart = b.parts[i];
        if cstr:cmp(aPart.name.buf, bPart.name.buf) == 0 {
            if (gens == false) | compare_gens(aPart.gens, bPart.gens) {
                next ~l;
            };
        };
        ret false;
    };
    ret true;
};

bool compare_gens(darr<Path> a, darr<Path> b) {
    if a.len != b.len {
        ret false;
    };

    for i = 0, i < a.len, i += 1 {
        Path aPath = a[i];
        Path bPath = b[i];

        if compare_paths(aPath, bPath, true) == false {
            ret false;
        };
    };
    ret true;
};

sub add_mangled_expr(Expr expr, Path path, str name) {
    while path.parts.len > paths.len {
        paths.push(darr<MangledExpr>:new_with_cap(16));
    };
    darr<MangledExpr>:push(paths[path.parts.len - 1]$, new MangledExpr {
        expr = expr,
        path = path,
        name = name,
    });
};
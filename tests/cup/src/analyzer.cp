sub REMOVE_ME2() { vec<Expr>:new(0); vec<PathPart>:new(0); vec<GenericType>:new(0); };

vec<PathPart> mods;

comp MangledPath {
    vec<PathPart> path,
    ptr<u8> name,
};

comp MangledLocalName {
    ptr<u8> local_name,
    ptr<u8> name,
};

int mangle_index = 0;
vec<vec<MangledPath>> mangled_paths;
vec<MangledLocalName> mangled_local_names;

comp GenericType {
    ptr<u8> name,
    ptr<Expr> path,
    ptr<Expr> _type,
};

vec<GenericType> gen_types;

vec<Expr> enum_types;

vec<MangledPath> vars;

sub analyze(vec<Expr> ast) {
    mods = vec<PathPart>:new(4);
    mangled_paths = vec<vec<MangledPath>>:new(8);
    vec<MangledPath> core_binds = vec<MangledPath>:new(64);
    core_binds.push(make_core_bind("int", "int"));
    core_binds.push(make_core_bind("i32", "int32_t"));
    core_binds.push(make_core_bind("f32", "float"));
    core_binds.push(make_core_bind("u8", "uint8_t"));
    core_binds.push(make_core_bind("bool", "uint8_t"));
    core_binds.push(make_core_bind("b8", "uint8_t"));
    core_binds.push(make_core_bind("i8", "int8_t"));
    core_binds.push(make_core_bind("float", "double"));
    core_binds.push(make_core_bind("f64", "double"));
    core_binds.push(make_core_bind("uint", "unsigned int"));
    core_binds.push(make_core_bind("u32", "uint32_t"));
    core_binds.push(make_core_bind("i64", "int64_t"));
    core_binds.push(make_core_bind("i16", "int16_t"));
    core_binds.push(make_core_bind("u64", "uint64_t"));
    core_binds.push(make_core_bind("u16", "uint16_t"));
    mangled_paths.push(core_binds);
    mangled_local_names = vec<MangledLocalName>:new(16);
    gen_types = vec<GenericType>:new(8);
    enum_types = vec<Expr>:new(8);
    vars = vec<MangledPath>:new(8);
    analyze_expr_vec(ast);
};

MangledPath make_core_bind(ptr<u8> from, ptr<u8> to) {
    ret MangledPath {
        path = vec<PathPart> {
            buf = alloc<PathPart>(PathPart {
                name = from,
                gens = vec<Expr> {
                    len = 0,
                },
            }),
            len = 1,
        },
        name = to,
    };
};

sub analyze_expr_vec(vec<Expr> exprs) {
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_expr(exprs.buf + i);
    };
};

sub analyze_expr(ptr<Expr> expr) {
    ptr<Expr> path;
    match expr@.kind {
        ExprKind:Block(body) {
            analyze_expr_vec(body);
            ret;
        },
        ExprKind:Mod(path, body) {
            int old_mods_len = mods.len;
            match path@.kind {
                ExprKind:Path(_path) {
                    mods.join_vec(_path);
                },
            };
            analyze_expr_vec(body);
            mods.len = old_mods_len;
            ret;
        },
        ExprKind:Comp(_path) {
            path = _path;
        },
        ExprKind:Enum(_path) {
            enum_types.push(expr@);
            path = _path;
        },
        ExprKind:Def(_prop, target, body) {
            int old_mods_len = mods.len;
            match _prop@.kind {
                ExprKind:Path(_path) {
                    mods.join_vec(_path);
                },
            };
            analyze_expr_vec(body);
            mods.len = old_mods_len;
            ret;
        },
        ExprKind:SubDef(_, _path) {
            path = _path;
        },
        ExprKind:VarDef(_type, path) {
            match _type@.kind {
                ExprKind:Path(__type) {
                    vars.push(MangledPath {
                        path = __type,
                        name = mangle(path@, false, false, 0),
                    });
                },
            };
        },
        _ {
            ret;
        }
    };

    vec<PathPart>:join_back(path@.kind.u.u2.path$, mods);

    if has_generics(path@) {
        gen_types.push(GenericType {
            name = mangle(path@, false, false, 0),
            path = path,
            _type = expr,
        });
        ret;
    };

    ~l for i = 0, (i) < expr@.tags.len, i += 1 {
        match expr@.tags.buf[i].kind {
            ExprKind:Tag(name, args) {
                if str:cmp(name, "bind") == 0 {
                    match args.buf[0].kind {
                        ExprKind:StringLit(value) {
                            vec<PathPart> _path = path@.kind.u.u2.path;
                            while _path.len > mangled_paths.len {
                                mangled_paths.push(vec<MangledPath>:new(32));
                            };
                            vec<MangledPath>:push(mangled_paths.buf[_path.len - 1]$, MangledPath {
                                path = _path,
                                name = value,
                            });
                        },
                    };
                };
            },
        };
    };
};

ptr<u8> mangle(Expr expr, bool gens, bool new, int pointer_count) {
    vec<PathPart> path = expr.kind.u.u2.path;

    if (path.buf[0].gens.len == 1) & (str:cmp(path.buf[0].name, "ptr") == 0) {
        ret mangle(path.buf[0].gens.buf[0], gens, new, pointer_count + 1);
    };

    if (path.len == 1) & (path.buf[0].gens.len == 0) {
        if str:cmp(path.buf[0].name, "main") == 0 {
            ret "main";
        };

        for i = 0, (i) < mangled_local_names.len, i += 1 {
            if str:cmp(path.buf[0].name, mangled_local_names.buf[i].local_name) == 0 {
                ret (mangled_local_names.buf[i].name);
            };
        };
    };
    if path.len <= mangled_paths.len {
        vec<MangledPath> paths = mangled_paths.buf[path.len - 1];
        ~l for i = 0, (i) < paths.len, i += 1 {
            if compare_paths(path, paths.buf[i].path, gens) {
                if new {
                    ret none;
                } else {
                    ptr<u8> name = paths.buf[i].name;
                    if pointer_count > 0 {
                        int len = str:len(name);
                        vec<u8> _name = vec<u8>:new(len + pointer_count + 1);
                        mem:copy(_name.buf, name, len * mem:size<u8>());
                        _name.len = len;
                        for i = 0, (i) < pointer_count, i += 1 {
                            _name.push('*');
                        };
                        _name.buf[_name.len] = '\0';
                        ret (_name.buf);
                    } else {
                        ret name;
                    };
                };
            };
        };
    } else {
        while path.len > mangled_paths.len {
            mangled_paths.push(vec<MangledPath>:new(32));
        };
    };

    vec<u8> name = new_mangle(mangle_index += 1, false);
    for i = 0, (i) < pointer_count, i += 1 {
        name.push('*');
    };
    name.buf[name.len] = '\0';

    vec<MangledPath>:push(mangled_paths.buf[path.len - 1]$, MangledPath {
        path = path,
        name = name.buf,
    });

    ret (name.buf);
};

vec<u8> new_mangle(int index, bool local) {
    vec<u8> name = vec<u8>:new(4);
    if local {
        name.join("__");
    } else {
        name.push('_');
    };

    while index > 0 {
        int i = index % 62;
        if (i) < 10 {
            i += 48;
        } elif (i) < 36 {
            i += 55;
        } else {
            i += 61;
        };
        name.push(i);
        index /= 62;
    };

    name.buf[name.len] = '\0';
    ret name;
};

bool compare_paths(vec<PathPart> path1, vec<PathPart> path2, bool gens) {
    if path1.len != path2.len {
        ret false;
    };
    
    ~l for i = 0, (i) < path1.len, i += 1 {
        PathPart part1 = path1.buf[i];
        PathPart part2 = path2.buf[i];
        if str:cmp(part1.name, part2.name) == 0 {
            if (gens == false) | compare_gens(part1.gens, part2.gens) {
                next ~l;
            };
        };
        ret false;
    };
    ret true;
};

bool compare_gens(vec<Expr> gens1, vec<Expr> gens2) {
    if gens1.len != gens2.len {
        ret false;
    };

    for i = 0, (i) < gens1.len, i += 1 {
        vec<PathPart> path1 = gens1.buf[i].kind.u.u2.path;
        vec<PathPart> path2 = gens2.buf[i].kind.u.u2.path;

        if compare_paths(path1, path2, true) == false {
            ret false;
        };
    };
    ret true;
};

` sub main(int abc) {
`     foo; ` undeclered foo
`     bar; ` good
`     int foo;
`     foo; ` good
`     abc; ` good
` }
` 
` int bar;
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

vec<Expr> type_paths;
vec<Expr> sub_paths;
vec<Expr> var_paths;
vec<Expr> enum_paths;

sub init_analyzer() {
    mods = vec<PathPart>:new(4);
    mangled_paths = vec<vec<MangledPath>>:new(8);
    mangled_local_names = vec<MangledLocalName>:new(16);
    gen_types = vec<GenericType>:new(8);
    type_paths = vec<Expr>:new(8);
    sub_paths = vec<Expr>:new(8);
    var_paths = vec<Expr>:new(8);
    enum_paths = vec<Expr>:new(4);

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
};

MangledPath make_core_bind(ptr<u8> from, ptr<u8> to) {
    vec<PathPart> path = vec<PathPart> {
        buf = alloc<PathPart>(PathPart {
            name = from,
            gens = vec<Expr> {
                len = 0,
            },
        }),
        len = 1,
    };
    type_paths.push(Expr {
        kind = ExprKind:Path(path, 0),
        tags = vec<Expr> { len=0, },
        label = none
    });
    ret MangledPath {
        path = path,
        name = to,
    };
};

sub analyze(File file, vec<Expr> exprs) {
    analyze_global_vec(file, exprs);
    analyze_local_vec(file, exprs);
};

sub analyze_global_vec(File file, vec<Expr> exprs) {
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_global(file, exprs.buf + i);
    };
};

sub analyze_global(File file, ptr<Expr> expr) {
    ptr<Expr> path;
    match expr@.kind {
        ExprKind:Block(body) {
            analyze_global_vec(file, body);
            ret;
        },
        ExprKind:Mod(_path, body) {
            int old_mods_len = mods.len;
            match _path@.kind {
                ExprKind:Path(__path) {
                    mods.join_vec(__path);
                },
            };
            analyze_global_vec(file, body);
            mods.len = old_mods_len;
            ret;
        },
        ExprKind:Comp(_path) {
            vec<PathPart>:join_back(_path@.kind.u.u2.path$, mods);
            type_paths.push(_path@);
            path = _path;
        },
        ExprKind:Enum(_path) {
            vec<PathPart>:join_back(_path@.kind.u.u2.path$, mods);
            type_paths.push(_path@);
            enum_paths.push(expr@);
            path = _path;
        },
        ExprKind:Def(_prop, target, body) {
            int old_mods_len = mods.len;
            match _prop@.kind {
                ExprKind:Path(_path) {
                    mods.join_vec(_path);
                },
            };
            analyze_global_vec(file, body);
            mods.len = old_mods_len;
            ret;
        },
        ExprKind:SubDef(_, _path) {
            vec<PathPart>:join_back(_path@.kind.u.u2.path$, mods);
            sub_paths.push(_path@);
            path = _path;
            ret;
        },
        ExprKind:VarDef(_type, _path) {
            vec<PathPart>:join_back(path@.kind.u.u2.path$, mods);
            var_paths.push(_path@);
            ret;
        },
        _ {
            ret;
        },
    };

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
                    ret;
                };
            },
        };
    };
};

sub analyze_local_vec(File file, vec<Expr> exprs) {
    int old_types_len = type_paths.len;
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_local(file, exprs.buf + i);
    };
    type_paths.len = old_types_len;
};

sub analyze_local(File file, ptr<Expr> expr) {
    match expr@.kind {
        ExprKind:Block(body) {
            analyze_local_vec(file, body);
        },
        ExprKind:Mod(path, body) {
            register_generic_paths(path@);
            analyze_local_vec(file, body);
        },
        ExprKind:Field(_type, name) {
            check_type_defined(file, _type@);
            vec<PathPart> parts = vec<PathPart>:new(1);
            parts.push(PathPart {
                name = name,
                gens = vec<Expr> { len = 0, }
            });
            var_paths.push(Expr {
                kind = ExprKind:Path(parts, 0),
                tags = vec<Expr> { len = 0, },
                label = none,
            });
        },
        ExprKind:Comp(path, fields) {
            register_generic_paths(path@);
            analyze_local_vec(file, fields);
        },
        ExprKind:Enum(path, opts) {
            register_generic_paths(path@);
            analyze_local_vec(file, opts);
        },
        ExprKind:Option(_, fields) {
            analyze_local_vec(file, fields);
        },
        ExprKind:Def(_prop, __, body) {
            register_generic_paths(_prop@);
            analyze_local_vec(file, body);
        },
        ExprKind:SubDef(_, path, args, body) {
            register_generic_paths(path@);
            analyze_local_vec(file, args);
            analyze_local_vec(file, body);
        },
        ExprKind:VarDef(_type, _, value) {
            check_type_defined(file, _type@);
            if value != none {
                analyze_local(file, value);
            };
        },
        ExprKind:LocalVarDef(_type, name, value) {
            check_type_defined(file, _type@);
            vec<PathPart> parts = vec<PathPart>:new(1);
            parts.push(PathPart {
                name = name,
                gens = vec<Expr> { len = 0, }
            });
            var_paths.push(Expr {
                kind = ExprKind:Path(parts, 0),
                tags = vec<Expr> { len = 0, },
                label = none,
            });
            if value != none {
                analyze_local(file, value);
            };
        },
        ExprKind:SubCall(path, args) {
            check_sub_defined(file, path@);
            analyze_local_vec(file, args);
        },
        ExprKind:VarUse(path) {
            check_var_defined(file, path@);
        },
        ExprKind:CompInst(_type, fields) {
            check_type_defined(file, _type@);
            for i = 0, (i) < fields.len, i += 1 {
                analyze_local(file, fields.buf[i].value);
            };
        },
        ExprKind:LocalBlock(body) {
            analyze_local_vec(file, body);
        },
        ExprKind:If(_if, _elif, _else) {
            analyze_local(file, _if);
            analyze_local_vec(file, _elif);
            if _else != none {
                analyze_local(file, _else);
            };
        },
        ExprKind:IfBranch(cond, body) {
            analyze_local(file, cond);
            analyze_local_vec(file, body);
        },
        ExprKind:ElseBranch(body) {
            analyze_local_vec(file, body);
        },
        ExprKind:Loop(body) {
            analyze_local_vec(file, body);
        },
        ExprKind:While(cond, body) {
            analyze_local(file, cond);
            analyze_local_vec(file, body);
        },
        ExprKind:For(_, iter_value, cond, _next, body) {
            analyze_local(file, iter_value);
            analyze_local(file, cond);
            analyze_local(file, _next);
            analyze_local_vec(file, body);
        },
        ExprKind:Each(_, iter, body) {
            analyze_local(file, iter);
            analyze_local_vec(file, body);
        },
        ExprKind:Match(value, cases) {
            analyze_local(file, value);
            analyze_local_vec(file, cases);
        },
        ExprKind:Case(values, body) {
            analyze_local_vec(file, values);
            analyze_local_vec(file, body);
        },
        ExprKind:Ret(_, value) {
            analyze_local(file, value);
        },
        ExprKind:Try(_, value) {
            analyze_local(file, value);
        },
        ExprKind:UnaryOp(value) {
            analyze_local(file, value);
        },
        ExprKind:BinaryOp(lhs, rhs, kind) {
            match kind {
                TokenKind:Dot {
                    match lhs@.kind {
                        ExprKind:VarUse(lhs_path) {
                            match lhs_path@.kind {
                                ExprKind:Path(_lhs_path) {
                                    match rhs@.kind {
                                        ExprKind:SubCall(_) {
                                            for i = 0, (i) < var_paths.len, i += 1 {
                                                match var_paths.buf[i].kind {
                                                    ExprKind:Path(var_path) {
                                                        if compare_paths(_lhs_path, var_path, false) {
                                                            vec<PathPart>:join_back(rhs@.kind.u.u16.path@.kind.u.u2.path$, var_path);
                                                            vec<Expr>:push_back(rhs@.kind.u.u16.args$, Expr {
                                                                kind = ExprKind:UnaryOp(
                                                                    lhs,
                                                                    TokenKind:Address
                                                                ),
                                                                tags = vec<Expr> { len = 0, },
                                                                label = none,
                                                            });
                                                            expr@ = rhs@;
                                                            ret;
                                                        };
                                                    },
                                                };
                                            };
                                        },
                                    };
                                },
                            };
                        },
                    };
                },
            };
            analyze_local(file, lhs);
            analyze_local(file, rhs);
        },
        ExprKind:TernaryOp(cond, valueA, valueB) {
            analyze_local(file, cond);
            analyze_local(file, valueA);
            analyze_local(file, valueB);
        },
    };
};

sub register_generic_paths(Expr path) {
    match path.kind {
        ExprKind:Path(_path) {
            for i = 0, (i) < _path.len, i += 1 {
                PathPart part = _path.buf[i];
                for ii = 0, (ii) < part.gens.len, ii += 1 {
                    type_paths.push(part.gens.buf[ii]);
                };
            };
        },
    };
};

sub check_type_defined(File file, Expr path) {
    match path.kind {
        ExprKind:Path(_path, index) {
            for i = 0, (i) < type_paths.len, i += 1 {
                match type_paths.buf[i].kind {
                    ExprKind:Path(type_path) {
                        if compare_paths(_path, type_path, false) {
                            ret;
                        };
                    },
                };
            };
            throw(file, index, "type not defined");
        },
    };
};

sub check_sub_defined(File file, Expr path) {
    match path.kind {
        ExprKind:Path(_path, index) {
            for i = 0, (i) < sub_paths.len, i += 1 {
                match sub_paths.buf[i].kind {
                    ExprKind:Path(sub_path) {
                        if compare_paths(_path, sub_path, false) {
                            ret;
                        };
                    },
                };
            };
            throw(file, index, "subroutine not defined");
        },
    };
};

sub check_var_defined(File file, Expr path) {
    match path.kind {
        ExprKind:Path(_path, index) {
            for i = 0, (i) < var_paths.len, i += 1 {
                match var_paths.buf[i].kind {
                    ExprKind:Path(var_path) {
                        if compare_paths(_path, var_path, false) {
                            ret;
                        };
                    },
                };
            };
            throw(file, index, "variable not defined");
        },
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
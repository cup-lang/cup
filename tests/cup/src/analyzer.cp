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
vec<MangledPath> var_paths;
vec<Expr> enum_paths;

sub init_analyzer() {
    mods = vec<PathPart>:new(4);
    mangled_paths = vec<vec<MangledPath>>:new(8);
    mangled_local_names = vec<MangledLocalName>:new(16);
    gen_types = vec<GenericType>:new(8);
    type_paths = vec<Expr>:new(8);
    sub_paths = vec<Expr>:new(8);
    var_paths = vec<MangledPath>:new(8);
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
    analyze_local_vec(file, exprs, true);
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
        },
        ExprKind:VarDef(_type, _path) {
            vec<PathPart>:join_back(_path@.kind.u.u2.path$, mods);
            path = _path;
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
                };
            },
        };
    };

    match expr@.kind {
        ExprKind:VarDef(_type, _path) {
            match _type@.kind {
                ExprKind:Path(__type) {
                    var_paths.push(MangledPath {
                        path = __type,
                        name = mangle(_path@, false, false, 0),
                    });
                },
            };
        },
    };
};

sub analyze_local_vec(File file, vec<Expr> exprs, bool clean_vars) {
    int old_types_len = type_paths.len;
    int old_var__len = var_paths.len;
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_local(file, exprs.buf + i);
    };
    type_paths.len = old_types_len;
    if clean_vars {
        var_paths.len = old_var__len;
    };
};

sub analyze_local(File file, ptr<Expr> expr) {
    match expr@.kind {
        ExprKind:Block(body) {
            analyze_local_vec(file, body, true);
        },
        ExprKind:Mod(path, body) {
            register_generic_paths(path@);
            analyze_local_vec(file, body, true);
        },
        ExprKind:Field(_type, name) {
            check_type_defined(file, _type@);
            vec<PathPart> parts = vec<PathPart>:new(1);
            parts.push(PathPart {
                name = name,
                gens = vec<Expr> { len = 0, }
            });
            match _type@.kind {
                ExprKind:Path(__type) {
                    var_paths.push(MangledPath {
                        path = __type,
                        name = mangle(Expr {
                            kind = ExprKind:Path(parts, 0),
                            tags = vec<Expr> { len = 0, },
                            label = none,
                        }, false, false, 0),
                    });
                },
            };
        },
        ExprKind:Comp(path, fields) {
            register_generic_paths(path@);
            analyze_local_vec(file, fields, true);
        },
        ExprKind:Enum(path, opts) {
            register_generic_paths(path@);
            analyze_local_vec(file, opts, true);
        },
        ExprKind:Option(_, fields) {
            analyze_local_vec(file, fields, true);
        },
        ExprKind:Def(_prop, __, body) {
            register_generic_paths(_prop@);
            analyze_local_vec(file, body, true);
        },
        ExprKind:SubDef(_, path, args, body) {
            register_generic_paths(path@);
            analyze_local_vec(file, args, false);
            analyze_local_vec(file, body, true);
        },
        ExprKind:VarDef(_type, _, value) {
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
            match _type@.kind {
                ExprKind:Path(__type) {
                    var_paths.push(MangledPath {
                        path = __type,
                        name = mangle(Expr {
                            kind = ExprKind:Path(parts, 0),
                            tags = vec<Expr> { len = 0, },
                            label = none,
                        }, false, false, 0),
                    });
                },
            };
            if value != none {
                analyze_local(file, value);
            };
        },
        ExprKind:SubCall(path, args) {
            if check_enum_inst(file, expr, path@, args) {
                analyze_local(file, expr);
                ret;
            };
            check_sub_defined(file, path@);
            analyze_local_vec(file, args, true);
        },
        ExprKind:VarUse(path) {
            vec<Expr> empty_args = vec<Expr> { len = 0, };
            if check_enum_inst(file, expr, path@, empty_args) {
                analyze_local(file, expr);
                ret;
            };
            match path@.kind {
                ExprKind:Path(_path) {
                    if (_path.len > 1) | (str:cmp(_path.buf[0].name, "_") != 0) {
                        check_var_defined(file, path@);
                    };
                },
            };
        },
        ExprKind:CompInst(_type, fields) {
            check_type_defined(file, _type@);
            for i = 0, (i) < fields.len, i += 1 {
                analyze_local(file, fields.buf[i].value);
            };
        },
        ExprKind:EnumInst(_, __, ___, args) {
            analyze_local_vec(file, args, true);
        },
        ExprKind:LocalBlock(body) {
            analyze_local_vec(file, body, true);
        },
        ExprKind:If(_if, _elif, _else) {
            analyze_local(file, _if);
            analyze_local_vec(file, _elif, true);
            if _else != none {
                analyze_local(file, _else);
            };
        },
        ExprKind:IfBranch(cond, body) {
            analyze_local(file, cond);
            analyze_local_vec(file, body, true);
        },
        ExprKind:ElseBranch(body) {
            analyze_local_vec(file, body, true);
        },
        ExprKind:Loop(body) {
            analyze_local_vec(file, body, true);
        },
        ExprKind:While(cond, body) {
            analyze_local(file, cond);
            analyze_local_vec(file, body, true);
        },
        ExprKind:For(iter, iter_value, cond, _next, body) {
            vec<PathPart> type_path = vec<PathPart>:new(1);
            type_path.push(PathPart {
                name = "int",
                gens = vec<Expr> { len = 0, },
            });
            vec<PathPart> parts = vec<PathPart>:new(1);
            parts.push(PathPart {
                name = iter,
                gens = vec<Expr> { len = 0, },
            });
            var_paths.push(MangledPath {
                path = type_path,
                name = mangle(Expr {
                    kind = ExprKind:Path(parts, 0),
                    tags = vec<Expr> { len = 0, },
                    label = none,
                }, false, false, 0),
            });
            analyze_local(file, iter_value);
            analyze_local(file, cond);
            analyze_local(file, _next);
            analyze_local_vec(file, body, true);
        },
        ExprKind:Each(_, iter, body) {
            analyze_local(file, iter);
            analyze_local_vec(file, body, true);
        },
        ExprKind:Match(value, cases) {
            analyze_local(file, value);
            analyze_local_vec(file, cases, true);
        },
        ExprKind:Case(values, body) {
            analyze_local_vec(file, values, false);
            analyze_local_vec(file, body, true);
        },
        ExprKind:Ret(_, value) {
            if value != none {
                analyze_local(file, value);
            };
        },
        ExprKind:Try(_, value) {
            analyze_local(file, value);
        },
        ExprKind:UnaryOp(value) {
            analyze_local(file, value);
        },
        ExprKind:BinaryOp(lhs, rhs, kind) {
            match kind {
                TokenKind:LeftBracket {
                    vec<PathPart> path = vec<PathPart>:new(1);
                    path.push(PathPart {
                        name = "buf",
                        gens = vec<Expr> { len = 0, },
                    });
                    expr@.kind.u.u42.lhs@.kind = ExprKind:BinaryOp(
                        alloc<Expr>(lhs@),
                        alloc<Expr>(Expr {
                            kind = ExprKind:VarUse(
                                alloc<Expr>(Expr {
                                    kind = ExprKind:Path(path),
                                    tags = vec<Expr> { len = 0, },
                                    label = none,
                                })
                            ),
                            tags = vec<Expr> { len = 0, },
                            label = none,
                        }),
                        TokenKind:Dot
                    );
                },
                TokenKind:Dot {
                    match lhs@.kind {
                        ExprKind:VarUse(lhs_path) {
                            match rhs@.kind {
                                ExprKind:SubCall(_) {
                                    ptr<u8> name = mangle(lhs_path@, false, false, 0);
                                    for i = 0, (i) < var_paths.len, i += 1 {
                                        if str:cmp(name, var_paths.buf[i].name) == 0 {
                                            expr@ = rhs@;
                                            vec<PathPart>:join_back(expr@.kind.u.u16.path@.kind.u.u2.path$, var_paths.buf[i].path);
                                            vec<Expr>:push_back(expr@.kind.u.u16.args$, Expr {
                                                kind = ExprKind:UnaryOp(
                                                    lhs,
                                                    TokenKind:Address
                                                ),
                                                tags = vec<Expr> { len = 0, },
                                                label = none,
                                            });
                                            analyze_local(file, expr);
                                            ret;
                                        };
                                    };
                                },
                            };
                        },
                    };
                    analyze_local(file, lhs);
                    ret;
                },
                TokenKind:As {
                    analyze_local(file, lhs);
                    ret;
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
            ptr<u8> name = mangle(path, false, false, 0);
            for i = 0, (i) < var_paths.len, i += 1 {
                if str:cmp(name, var_paths.buf[i].name) == 0 {
                    ret;
                };
            };
            throw(file, index, "variable not defined");
        },
    };
};

bool check_enum_inst(File file, ptr<Expr> expr, Expr path, vec<Expr> args) {
    match path.kind {
        ExprKind:Path(_path, index) {
            if _path.len > 1 {
                _path.len -= 1;
                for i = 0, (i) < enum_paths.len, i += 1 {
                    Expr _enum = enum_paths.buf[i];
                    match _enum.kind {
                        ExprKind:Enum(enum_path, enum_opts) {
                            match enum_path@.kind {
                                ExprKind:Path(_enum_path) {
                                    if compare_paths(_enum_path, _path, false) {
                                        ptr<u8> name = _path.buf[_path.len].name;
                                        for ii = 0, (ii) < enum_opts.len, ii += 1 {
                                            match enum_opts.buf[ii].kind {
                                                ExprKind:Option(opt_name, opt_fields) {
                                                    if str:cmp(name, opt_name) == 0 {
                                                        ptr<Expr> new_path = alloc<Expr>(path);
                                                        new_path@.kind.u.u2.path.len -= 1;
                                                        expr@.kind = ExprKind:EnumInst(alloc<Expr>(enum_paths.buf[i]), new_path, ii, args);
                                                        for iii = 0, (iii) < args.len, iii += 1 {
                                                            match args.buf[iii].kind {
                                                                ExprKind:VarUse(arg_path) {
                                                                    var_paths.push(MangledPath {
                                                                        path = opt_fields.buf[iii].kind.u.u7._type@.kind.u.u2.path,
                                                                        name = mangle(arg_path@, false, false, 0),
                                                                    });
                                                                },
                                                            };
                                                        };
                                                        ret true;
                                                    };
                                                },
                                            };
                                        };
                                    };
                                },
                            };
                        },
                    };
                };
            };
        },
    };
    ret false;
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

    vec<MangledPath>:push(mangled_paths.buf[path.len - 1]$, MangledPath {
        path = path,
        name = name.buf,
    });

    if pointer_count > 0 {
        ptr<u8> old_name = name.buf;
        name = vec<u8>:new(name.len);
        name.join(old_name);
        for i = 0, (i) < pointer_count, i += 1 {
            name.push('*');
        };
        name.buf[name.len] = '\0';
    };

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
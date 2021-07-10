comp GenericInstance (
    ptr<u8> name,
    ptr<Expr> path,
);

vec<GenericInstance> gens;

comp Req (
    ptr<u8> name,
);

vec<Req> reqs;
vec<u8> types_headers;
vec<u8> funcs_headers;
vec<u8> types;
vec<u8> funcs;

ptr<u8> generate(vec<Expr> ast) {
    gens = vec<GenericInstance>:new(2);
    reqs = vec<Req>:new(4);
    types_headers = vec<u8>:new(256);
    funcs_headers = vec<u8>:new(256);
    types = vec<u8>:new(1024);
    funcs = vec<u8>:new(1024);
    generate_expr_vec(none, ast, false, false, false);
    vec<u8> out = vec<u8>:new((reqs.len * 32) + types_headers.len + funcs_headers.len + types.len + funcs.len);
    types_headers.buf[types_headers.len] = funcs_headers.buf[funcs_headers.len] = types.buf[types.len] = funcs.buf[funcs.len] = '\0';
    for i = 0, (i) < reqs.len, i += 1 {
        out.join("#include ");
        out.push('"');
        out.join(reqs.buf[i].name);
        out.push('"');
        out.join("\n");
    };
    out.join(types_headers.buf);
    out.join(funcs_headers.buf);
    out.join(types.buf);
    out.join(funcs.buf);
    out.buf[out.len] = '\0';
    ret (out.buf);
};

sub generate_expr_vec(ptr<vec<u8>> out, vec<Expr> exprs, bool semicolon, bool comma, bool clean_locals) {
    int old_local_names_len = mangled_local_names.len;
    for i = 0, (i) < exprs.len, i += 1 {
        bool is_last = i + 1 == exprs.len;
        generate_expr(out, exprs.buf[i], is_last, semicolon, 0);
        if comma & (is_last == false) {
            vec<u8>:push(out, ',');
        };
    };
    if clean_locals {
        for i = old_local_names_len, (i) < mangled_local_names.len, i += 1 {
            mem:free(mangled_local_names.buf[i].name);
        };
        mangled_local_names.len = old_local_names_len;
    };
};

sub generate_expr(ptr<vec<u8>> out, Expr expr, bool last, bool semicolon, int parenths) {
    ~l for i = 0, (i) < expr.tags.len, i += 1 {
        match expr.tags.buf[i].kind {
            ExprKind:Tag(name, args) {
                if str:cmp(name, "os") == 0 {
                    match args.buf[0].kind {
                        ExprKind:StringLit(value) {
                            #raw("#if defined _WIN32")
                            if str:cmp(value, "win") != 0 {
                                ret;
                            };
                            #raw("#elif defined __linux__")
                            if str:cmp(value, "linux") != 0 {
                                ret;
                            };
                            #raw("#elif defined __APPLE__")
                            if str:cmp(value, "mac") != 0 {
                                ret;
                            };
                            #raw("#endif") 0+0;
                        },
                    };
                } elif str:cmp(name, "req") == 0 {
                    match args.buf[0].kind {
                        ExprKind:StringLit(value) {
                            for ii = 0, (ii) < reqs.len, ii += 1 {
                                if str:cmp(reqs.buf[ii].name, value) == 0 {
                                    next ~l;
                                };
                            };
                            reqs.push(Req(
                                name = value,
                            ));
                        },
                    };
                } elif str:cmp(name, "raw") == 0 {
                    match args.buf[0].kind {
                        ExprKind:StringLit(value) {
                            vec<u8>:join(out, value);
                        },
                    };
                } elif str:cmp(name, "bind") == 0 {
                    ret;
                };
            },
        };
    };

    match expr.kind {
        ExprKind:Block(body) {
            generate_expr_vec(out, body, false, false, false);
        },
        ExprKind:Mod(_, body) {
            generate_expr_vec(out, body, false, false, false);
        },
        ExprKind:Field(_type, name) {
            Expr __type = apply_genenerics(_type@);
            register_path_use(__type);
            vec<u8>:join(out, mangle(__type, true, false, 0));
            vec<u8>:push(out, ' ');
            if semicolon {
                vec<u8>:join(out, name);
            } else {
                vec<u8>:join(out, register_local_name(name));
            };
            if semicolon {
                vec<u8>:push(out, ';');
            } elif last == false {
                vec<u8>:push(out, ',');
            };
        },
        ExprKind:Comp(path, fields, body) {
            bool has_gens = has_generics(path@);
            if (gens.len == 0) & has_gens {
                ret;
            };
            Expr _path = apply_genenerics(path@);
            ptr<u8> name = mangle(_path, true, has_gens, 0);
            if has_gens & (name == none) {
                ret;
            };

            vec<u8>:join(types_headers$, "typedef struct ");
            vec<u8>:join(types_headers$, name);
            vec<u8>:push(types_headers$, ' ');
            vec<u8>:join(types_headers$, name);
            vec<u8>:push(types_headers$, ';');

            vec<u8> comp_out = vec<u8>:new(128);
            vec<u8>:join(comp_out$, "typedef struct ");
            vec<u8>:join(comp_out$, name);
            vec<u8>:push(comp_out$, '{');
            generate_expr_vec(comp_out$, fields, true, false, false);
            vec<u8>:push(comp_out$, '}');
            vec<u8>:join(comp_out$, name);
            vec<u8>:push(comp_out$, ';');
            comp_out.buf[comp_out.len] = '\0';
            vec<u8>:join(types$, comp_out.buf);
            mem:free(comp_out.buf);
        },
        ExprKind:Enum(path, opts, body) {
            bool has_gens = has_generics(path@);
            if (gens.len == 0) & has_gens {
                ret;
            };
            Expr _path = apply_genenerics(path@);
            ptr<u8> name = mangle(_path, true, has_gens, 0);
            if has_gens & (name == none) {
                ret;
            };

            vec<u8>:join(types_headers$, "typedef struct ");
            vec<u8>:join(types_headers$, name);
            vec<u8>:push(types_headers$, ' ');
            vec<u8>:join(types_headers$, name);
            vec<u8>:push(types_headers$, ';');

            vec<u8> enum_out = vec<u8>:new(256);
            vec<u8>:join(enum_out$, "typedef struct ");
            vec<u8>:join(enum_out$, name);
            vec<u8>:push(enum_out$, '{');
            vec<u8>:join(enum_out$, "int t;union{");
            for i = 0, (i) < opts.len, i += 1 {
                vec<u8>:join(enum_out$, "struct{");
                vec<u8>:join(enum_out$, "}u");
                ptr<u8> union_name = new_mangle(i + 1, false).buf;
                vec<u8>:join(enum_out$, union_name);
                mem:free(union_name);
                vec<u8>:push(enum_out$, ';');
            };
            vec<u8>:join(enum_out$, "}u;}");
            vec<u8>:join(enum_out$, name);
            vec<u8>:push(enum_out$, ';');
            enum_out.buf[enum_out.len] = '\0';
            vec<u8>:join(types$, enum_out.buf);
            mem:free(enum_out.buf);
        },
        ExprKind:Prop {},
        ExprKind:Def {},
        ExprKind:SubDef(ret_type, path, args, body) {
            bool has_gens = has_generics(path@);
            if (gens.len == 0) & has_gens {
                ret;
            };

            Expr _path = apply_genenerics(path@);
            ptr<u8> name = mangle(_path, true, has_gens, 0);
            if has_gens & (name == none) {
                ret;
            };
            vec<u8> sub_out = vec<u8>:new(256);
            if ret_type != none {
                Expr _ret_type = apply_genenerics(ret_type@);
                vec<u8>:join(sub_out$, mangle(_ret_type, true, false, 0));
                vec<u8>:push(sub_out$, ' ');
            } else {
                vec<u8>:join(sub_out$, "void ");
            };
            vec<u8>:join(sub_out$, name);
            vec<u8>:push(sub_out$, '(');
            generate_expr_vec(sub_out$, args, false, false, false);
            sub_out.buf[sub_out.len] = '\0';
            vec<u8>:join(funcs_headers$, sub_out.buf);
            vec<u8>:join(funcs_headers$, ");");

            vec<u8>:join(sub_out$, "){");
            generate_expr_vec(sub_out$, body, true, false, true);
            vec<u8>:join(sub_out$, "};");
            sub_out.buf[sub_out.len] = '\0';
            vec<u8>:join(funcs$, sub_out.buf);
            mem:free(sub_out.buf);
        },
        ExprKind:VarDef(_type, path, value) {
            Expr __type = apply_genenerics(_type@);
            register_path_use(__type);
            vec<u8>:join(out, mangle(__type, true, false, 0));
            vec<u8>:push(out, ' ');
            Expr _path = apply_genenerics(path@);
            register_path_use(_path);
            vec<u8>:join(out, mangle(_path, true, false, 0));
            if value != none {
                vec<u8>:push(out, '=');
                generate_expr(out, value@, false, false, 0);
            };
            vec<u8>:push(out, ';');
        },
        ExprKind:LocalVarDef(_type, name, value) {
            Expr __type = apply_genenerics(_type@);
            register_path_use(__type);
            vec<u8>:join(out, mangle(__type, true, false, 0));
            vec<u8>:push(out, ' ');
            vec<u8>:join(out, register_local_name(name));
            if value != none {
                vec<u8>:push(out, '=');
                generate_expr(out, value@, false, false, 0);
            };
            vec<u8>:push(out, ';');
        },
        ExprKind:SubCall(path, args) {
            Expr _path = apply_genenerics(path@);
            register_path_use(_path);
            vec<u8>:join(out, mangle(_path, true, false, 0));
            vec<u8>:push(out, '(');
            generate_expr_vec(out, args, false, true, false);
            vec<u8>:push(out, ')');
            if semicolon {
                vec<u8>:push(out, ';');
            };
        },
        ExprKind:VarUse(path) {
            Expr _path = apply_genenerics(path@);
            register_path_use(_path);
            vec<u8>:join(out, mangle(_path, true, false, 0));
            if semicolon {
                vec<u8>:push(out, ';');
            };
        },
        ExprKind:StringLit(value) {
            vec<u8>:push(out, '"');
            vec<u8>:join(out, value);
            vec<u8>:push(out, '"');
        },
        ExprKind:CharLit(value) {
            vec<u8>:push(out, 39);
            vec<u8>:join(out, value);
            vec<u8>:push(out, 39);
        },
        ExprKind:IntLit(value) {
            vec<u8>:join(out, value);
        },
        ExprKind:FloatLit(value) {
            vec<u8>:join(out, value);
        },
        ExprKind:BoolLit(value) {
            if value {
                vec<u8>:push(out, '1');
            } else {
                vec<u8>:push(out, '0');
            };
        },
        ExprKind:LocalBlock(body) {
            vec<u8>:push(out, '{');
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:If(_if, _elif, _else) {
            generate_expr(out, _if@, false, false, 0);
            for i = 0, (i) < _elif.len, i += 1 {
                vec<u8>:join(out, "else ");
                generate_expr(out, _elif.buf[i], false, false, 0);
            };
            if _else != none {
                generate_expr(out, _else@, false, false, 0);
            };
        },
        ExprKind:IfBranch(cond, body) {
            vec<u8>:join(out, "if(");
            generate_expr(out, cond@, false, false, 0);
            vec<u8>:join(out, "){");
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:ElseBranch(body) {
            vec<u8>:join(out, "else{");
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:Loop(body) {
            vec<u8>:join(out, "for(;;){");
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:While(cond, body) {
            vec<u8>:join(out, "while(");
            generate_expr(out, cond@, false, false, 0);
            vec<u8>:join(out, "){");
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:For(iter, iter_value, cond, _next, body) {
            vec<u8>:join(out, "for(int ");
            vec<u8>:join(out, iter);
            if iter_value != none {
                vec<u8>:push(out, '=');
                generate_expr(out, iter_value@, false, false, 0);
            };
            vec<u8>:push(out, ';');
            generate_expr(out, cond@, false, false, 0);
            vec<u8>:push(out, ';');
            generate_expr(out, _next@, false, false, 0);
            vec<u8>:join(out, "){");
            generate_expr_vec(out, body, true, false, true);
            vec<u8>:push(out, '}');
        },
        ExprKind:Each {},
        ExprKind:Match {},
        ExprKind:Case {},
        ExprKind:Ret(label, value) {
            if label != none {
                vec<u8>:join(out, "goto brk_");
                vec<u8>:join(out, label);
            } else {
                vec<u8>:join(out, "return");
            };
            if value != none {
                vec<u8>:push(out, ' ');
                generate_expr(out, value@, false, false, 0);
            };
            vec<u8>:push(out, ';');
        },
        ExprKind:Next(label) {
            if label != none {
                vec<u8>:join(out, "goto ");
                vec<u8>:join(out, label);
                vec<u8>:push(out, ';');
            } else {
                vec<u8>:join(out, "continue;");
            };
        },
        ExprKind:Jump(label) {
            vec<u8>:join(out, "goto ");
            vec<u8>:join(out, label);
            vec<u8>:push(out, ';');
        },
        ExprKind:Try {},
        ExprKind:UnaryOp(value, kind) {
            vec<u8>:push(out, '(');
            match kind {
                TokenKind:Not {
                    vec<u8>:push(out, '!');
                },
                TokenKind:Deref {
                    vec<u8>:push(out, '*');
                },
                TokenKind:Address {
                    vec<u8>:push(out, '&');
                },
            };
            generate_expr(out, value@, false, false, parenths + 1);
            vec<u8>:push(out, ')');
        },
        ExprKind:BinaryOp(lhs, rhs, kind) {
            if parenths > 0 {
                vec<u8>:push(out, '(');
            };

            match kind {
                TokenKind:As {
                    vec<u8>:push(out, '(');
                    ` generate_type(rhs);
                    vec<u8>:push(out, ')');
                    generate_expr(out, lhs@, false, false, parenths + 1);
                },
                _ {
                    generate_expr(out, lhs@, false, false, parenths + 1);
                    match kind {
                        TokenKind:Assign {
                            vec<u8>:push(out, '=');
                        },
                        TokenKind:Equal {
                            vec<u8>:join(out, "==");
                        },
                        TokenKind:And {
                            vec<u8>:join(out, "&&");
                        },
                        TokenKind:Or {
                            vec<u8>:join(out, "||");
                        },
                        TokenKind:Less {
                            vec<u8>:push(out, '<');
                        },
                        TokenKind:LessEqual {
                            vec<u8>:join(out, "<=");
                        },
                        TokenKind:Greater {
                            vec<u8>:push(out, '>');
                        },
                        TokenKind:GreaterEqual {
                            vec<u8>:join(out, ">=");
                        },
                        TokenKind:Add {
                            vec<u8>:push(out, '+');
                        },
                        TokenKind:AddAssign {
                            vec<u8>:join(out, "+=");
                        },
                        TokenKind:Subtract {
                            vec<u8>:push(out, '-');
                        },
                        TokenKind:SubtractAssign {
                            vec<u8>:join(out, "-=");
                        },
                        TokenKind:Multiply {
                            vec<u8>:push(out, '*');
                        },
                        TokenKind:MultiplyAssign {
                            vec<u8>:join(out, "*=");
                        },
                        TokenKind:Divide {
                            vec<u8>:push(out, '/');
                        },
                        TokenKind:DivideAssign {
                            vec<u8>:join(out, "/=");
                        },
                        TokenKind:Modulo {
                            vec<u8>:push(out, '%');
                        },
                        TokenKind:ModuloAssign {
                            vec<u8>:join(out, "%=");
                        },
                        TokenKind:NotEqual {
                            vec<u8>:join(out, "!=");
                        },
                        TokenKind:Dot {
                            vec<u8>:push(out, '.');
                        },
                        TokenKind:LeftBracket {
                            vec<u8>:push(out, '[');
                        },
                    };
                    generate_expr(out, rhs@, false, false, parenths + 1);
                    match kind {
                        TokenKind:LeftBracket {
                            vec<u8>:push(out, ']');
                        },
                    };
                },
            };

            if parenths > 0 {
                vec<u8>:push(out, ')');
            };
            if semicolon > 0 {
                vec<u8>:push(out, ';');
            };
        },
        ExprKind:TernaryOp(cond, valueA, valueB) {
            generate_expr(out, cond@, false, false, parenths + 1);
            vec<u8>:push(out, '?');
            generate_expr(out, valueA@, false, false, parenths + 1);
            vec<u8>:push(out, ':');
            generate_expr(out, valueB@, false, false, parenths + 1);
        },
    };
};

bool has_generics(Expr expr) {
    match expr.kind {
        ExprKind:Path(path) {
            for i = 0, (i) < path.len, i += 1 {
                if path.buf[i].gens.len > 0 {
                    ret true;
                };
            };
        },
    };
    ret false;
};

sub register_path_use(Expr expr) {
    ptr<u8> name = mangle(expr, false, false, 0);
    for i = 0, (i) < gen_types.len, i += 1 {
        GenericType gen = gen_types.buf[i];
        if gen.name == name {
            match expr.kind {
                ExprKind:Path(path) {
                    match gen.path@.kind {
                        ExprKind:Path(type_path) {
                            int old_gens_len = gens.len;
                            for ii = 0, (ii) < type_path.len, ii += 1 {
                                PathPart part = type_path.buf[ii];
                                for iii = 0, (iii) < part.gens.len, iii += 1 {
                                    match part.gens.buf[iii].kind {
                                        ExprKind:Path(gen_path) {
                                            gens.push(GenericInstance(
                                                name = gen_path.buf[0].name,
                                                path = path.buf[ii].gens.buf[iii]$,
                                            ));
                                        },
                                    };
                                };
                            };
                            generate_expr(none, gen._type@, false, false, 0);
                            gens.len = old_gens_len;
                        },
                    };
                },
            };
            ret;
        };
    };
};

ptr<u8> register_local_name(ptr<u8> local_name) {
    ptr<u8> name = new_mangle(mangled_local_names.len + 1, true).buf;
    mangled_local_names.push(MangledLocalName(
        local_name = local_name, 
        name = name,
    ));
    ret name;
};

Expr apply_genenerics(Expr expr) {
    match expr.kind {
        ExprKind:Path(path) {
            if path.len == 1 {
                for i = 0, (i) < gens.len, i += 1 {
                    if str:cmp(gens.buf[i].name, path.buf[0].name) == 0 {
                        ret (gens.buf[i].path@);
                    };
                };
            };
            ptr<PathPart> new_path = mem:alloc(path.len * mem:size<PathPart>());
            for i = 0, (i) < path.len, i += 1 {
                PathPart part = path.buf[i];
                new_path[i].name = part.name;
                new_path[i].gens = vec<Expr>:new(part.gens.len);
                new_path[i].gens.len = part.gens.len;
                for ii = 0, (ii) < part.gens.len, ii += 1 {
                    new_path[i].gens.buf[ii] = apply_genenerics(part.gens.buf[ii]);
                };
            };
            expr.kind.u.u2.path.buf = new_path;
        },
    };
    ret expr;
};
sub REMOVE_ME2() { vec<PathPart>:new(0); };

comp MangledPath {
    vec<PathPart> path,
    ptr<u8> name,
};

int mangle_index = 0;
vec<vec<MangledPath>> mangled_paths;

sub generate(ptr<vec<u8>> out, vec<Expr> ast) {
    mangled_paths = vec<vec<MangledPath>>:new(8);
    generate_expr_vec(out, ast, false, false);    
};

sub generate_expr_vec(ptr<vec<u8>> out, vec<Expr> exprs, bool semicolon, bool comma) {
    ` let out = '';
    ` vars.push({});
    for i = 0, (i) < exprs.len, i += 1 {
        bool is_last = i + 1 == exprs.len;
        generate_expr(out, exprs.buf[i], is_last, semicolon, 0);
        if comma & (is_last == false) {
            vec<u8>:push(out, ',');
        };
    };
    ` vars.pop();
};

sub generate_expr(ptr<vec<u8>> out, Expr expr, bool last, bool semicolon, int parenths) {
    ` if (expr.gen && expr.gen.length > 0) {
    `     return '';
    ` }

    ` if (expr.tags) {
    `     for (let i = 0; i < expr.tags.length; ++i) {
    `         const tag = expr.tags[i];
    `         switch (tag.name) {
    `             case 'os':
    `                 switch (tag.args[0].value) {
    `                     case 'win':
    `                         if (process.platform !== 'win32') {
    `                             return '';
    `                         }
    `                         break;
    `                     case 'linux':
    `                         if (process.platform !== 'linux') {
    `                             return '';
    `                         }
    `                         break;
    `                 }
    `                 break;
    `             case 'req':
    `                 if (!reqs.includes(tag.args[0].value)) {
    `                     reqs.push(tag.args[0].value);
    `                 }
    `                 break;
    `             case 'bind':
    `                 break;
    `             case 'raw':
    `                 output += tag.args[0].value;
    `                 break;
    `         }
    `     }
    ` }

    ` let oldScopeGens = [].concat(scopeGens);
    ` if (expr.gen) {
    `     for (let i = 0; i < expr.gen.length; ++i) {
    `         scopeGens.push(expr.gen[i]);
    `     }
    `     expr.gen = oldScopeGens.concat(expr.gen);
    ` }

    ` let out = '';

    match expr.kind {
        ExprKind:Block(body) {
            generate_expr_vec(out, body, false, false);
        },
        ExprKind:Field {},
        ExprKind:Comp {},
        ExprKind:Enum {},
        ExprKind:Option {},
        ExprKind:Prop {},
        ExprKind:Def {},
        ExprKind:SubDef {},
        ExprKind:VarDef(_type, path, value) {
            vec<u8>:join(out, mangle(_type@));
            vec<u8>:push(out, ' ');
            vec<u8>:join(out, mangle(path@));
            if value != none {
                vec<u8>:push(out, '=');
                generate_expr(out, value@, false, false, 0);
            };
            vec<u8>:push(out, ';');
        },
        ExprKind:LocalVarDef {},
        ExprKind:SubCall {},
        ExprKind:VarUse {},
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
            generate_expr_vec(out, body, true, false);
            vec<u8>:push(out, '}');
        },
        ExprKind:If {},
        ExprKind:Loop(body) {
            vec<u8>:join(out, "for(;;){");
            generate_expr_vec(out, body, true, false);
            vec<u8>:push(out, '}');
        },
        ExprKind:While(cond, body) {
            vec<u8>:join(out, "while(");
            generate_expr(out, cond@, false, false, 0);
            vec<u8>:join(out, "){");
            generate_expr_vec(out, body, true, false);
            vec<u8>:push(out, '}');
        },
        ExprKind:For {},
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

    ` if (expr.label) {
    `     out += `brk_${expr.label}:`;
    ` }

    ` scopeGens = oldScopeGens;
};

ptr<u8> mangle(Expr expr) {
    vec<PathPart> path = expr.kind.u.u2.path;
    if path.len <= mangled_paths.len {
        vec<MangledPath> paths = mangled_paths.buf[path.len - 1];
        ~l for i = 0, (i) < paths.len, i += 1 {
            if compare_paths(path, paths.buf[i].path) {
                ret (paths.buf[i].name);
            };
        };
    } else {
        while path.len > mangled_paths.len {
            mangled_paths.push(vec<MangledPath>:new(32));
        };
    };
    ptr<u8> name = new_mangle(mangle_index += 1);
    vec<MangledPath>:push(mangled_paths.buf[path.len - 1]$, MangledPath {
        path = path,
        name = name,
    }); 
    ret name;
};

ptr<u8> new_mangle(int index) {
    vec<u8> name = vec<u8>:new(4);

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
    ret (name.buf);
};

bool compare_paths(vec<PathPart> path1, vec<PathPart> path2) {
    ~l for i = 0, (i) < path1.len, i += 1 {
        PathPart part1 = path1.buf[i];
        PathPart part2 = path2.buf[i];
        if part1.gens.len == part2.gens.len {
            if str:cmp(part1.name, part2.name) == 0 {
                if compare_gens(part1.gens, part2.gens) {
                    next ~l;
                };
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

        if compare_paths(path1, path2) == false {
            ret false;
        };
    };
    ret true;
};
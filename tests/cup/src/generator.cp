sub generate(ptr<vec<u8>> out, vec<Expr> ast) {
    generate_expr_vec(out, ast, 0, 0);    
};

sub generate_expr_vec(ptr<vec<u8>> out, vec<Expr> exprs, int semicolon, int comma) {
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

sub generate_expr(ptr<vec<u8>> out, Expr expr, int last, int semicolon, int parenths) {
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
        ExprKind:TagDef {},
        ExprKind:Block(body) {
            generate_expr_vec(out, body, 0, 0);
        },
        ExprKind:Field {},
        ExprKind:Comp {},
        ExprKind:Enum {},
        ExprKind:Option {},
        ExprKind:Prop {},
        ExprKind:Def {},
        ExprKind:SubDef {},
        ExprKind:VarDef {},
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
            generate_expr_vec(out, body, 1, 0);
            vec<u8>:push(out, '}');
        },
        ExprKind:If {},
        ExprKind:Loop(body) {
            vec<u8>:join(out, "for(;;){");
            generate_expr_vec(out, body, 1, 0);
            vec<u8>:push(out, '}');
        },
        ExprKind:While(cond, body) {
            vec<u8>:join(out, "while(");
            generate_expr(out, cond@, 0, 0, 0);
            vec<u8>:join(out, "){");
            generate_expr_vec(out, body, 1, 0);
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
                generate_expr(out, value@, 0, 0, 0);
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
            generate_expr(out, value@, 0, 0, parenths + 1);
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
                    generate_expr(out, lhs@, 0, 0, parenths + 1);
                },
                _ {
                    generate_expr(out, lhs@, 0, 0, parenths + 1);
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
                    generate_expr(out, rhs@, 0, 0, parenths + 1);
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
            generate_expr(out, cond@, 0, 0, parenths + 1);
            vec<u8>:push(out, '?');
            generate_expr(out, valueA@, 0, 0, parenths + 1);
            vec<u8>:push(out, ':');
            generate_expr(out, valueB@, 0, 0, parenths + 1);
        },
    };

    ` if (expr.label) {
    `     out += `brk_${expr.label}:`;
    ` }

    ` scopeGens = oldScopeGens;
};
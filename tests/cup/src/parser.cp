sub REMOVE_ME() { vec<PathPart>:new(0); vec<VarName>:new(0); };

comp PathPart {
    ptr<u8> name,
    vec<Expr> gens,
};

comp VarName {
    ptr<u8> name,
};

enum ExprKind {
    Tag(ptr<u8> name, vec<Expr> args),
    Path(vec<PathPart> path),
    TagDef(ptr<Expr> path, vec<Expr> args, vec<Expr> body),
    Block(vec<Expr> body),
    Mod(ptr<Expr> path),
    Use(ptr<Expr> path),
    Field(ptr<Expr> _type, ptr<u8> name),
    Comp(ptr<Expr> path, vec<Expr> fields, vec<Expr> body),
    Enum(ptr<Expr> path, vec<Expr> fields, vec<Expr> opts, vec<Expr> body),
    Option(ptr<u8> name, vec<Expr> fields),
    Prop(ptr<Expr> path, vec<Expr> body),
    Def(ptr<Expr> _prop, ptr<Expr> target, vec<Expr> body),
    SubDef(ptr<Expr> ret_type, ptr<Expr> path, vec<Expr> args, vec<Expr> body),
    VarDef(ptr<Expr> _type, ptr<Expr> path, ptr<Expr> value),

    LocalVarDef(ptr<Expr> _type, ptr<u8> name, ptr<Expr> value),
    SubCall(ptr<Expr> path, vec<Expr> args),
    VarUse(ptr<Expr> path),
    StringLit(ptr<u8> value),
    CharLit(ptr<u8> value),
    IntLit(ptr<u8> value),
    FloatLit(ptr<u8> value),
    BoolLit(bool value),
    ThisLit,
    TypeLit,
    LocalBlock(vec<Expr> body),
    If(ptr<Expr> _if, vec<Expr> _elif, ptr<Expr> _else),
    IfBranch(ptr<Expr> cond, vec<Expr> body),
    ElseBranch(vec<Expr> body),
    Loop(vec<Expr> body),
    While(ptr<Expr> cond, vec<Expr> body),
    For(vec<Expr> iter, ptr<Expr> cond, vec<Expr> _next, vec<Expr> body),
    Each(vec<VarName> vars, ptr<Expr> iter, vec<Expr> body),
    Match(ptr<Expr> value, vec<Expr> cases),
    Case(vec<Expr> values, vec<Expr> body),
    Ret(ptr<u8> label, ptr<Expr> value),
    Next(ptr<u8> label),
    Jump(ptr<u8> label),
    Try(ptr<u8> label, ptr<Expr> value),

    UnaryOp(ptr<Expr> value, TokenKind kind),
    BinaryOp(ptr<Expr> lhs, ptr<Expr> rhs, TokenKind kind),
    TernaryOp(ptr<Expr> cond, ptr<Expr> valueA, ptr<Expr> valueB),
};

comp Expr {
    ExprKind kind,
    vec<Expr> tags,
    ptr<u8> label,
};

vec<Expr> parse(File file, vec<Token> tokens) {
    int index = 0;
    vec<Expr> exprs = vec<Expr>:new(16);

    loop {
        match tokens.buf[index].kind {
            TokenKind:Empty {
                ret exprs;
            },
        };

        exprs.push(parse_global(file, tokens, index$));

        match tokens.buf[index].kind {
            TokenKind:Semicolon { },
            TokenKind:Empty {
                ret exprs;
            },
            _ {
                throw(file, tokens.buf[index].index, "unexpected smth after last expr");
            },
        };

        index += 1;
    };
};

vec<Expr> parse_block(File file, vec<Token> tokens, ptr<int> index, bool local) {
    vec<Expr> exprs = vec<Expr>:new(4);

    loop {
        match tokens.buf[index@].kind {
            TokenKind:RightBrace {
                index@ += 1;
                ret exprs;
            },
        };

        Expr expr;
        if local {
            expr = parse_local(file, tokens, index);
        } else {
            expr = parse_global(file, tokens, index);
        };
        exprs.push(expr);

        match tokens.buf[index@].kind {
            TokenKind:Semicolon { },
            TokenKind:RightBrace {
                index@ += 1;
                ret exprs;
            },
            _ {
                throw(file, tokens.buf[index@].index, "unexpected smth after last expr");
            },
        };

        index@ += 1;
    };
};

Token expect_token(File file, vec<Token> tokens, ptr<int> index, TokenKind kind, ptr<u8> error) {
    Token token = tokens.buf[index@];
    match token.kind {
        TokenKind:Unset {},
        kind {},
        _ {
            throw(file, token.index, error);
        },
    };
    index@ += 1;
    ret token;
};

bool opt_token(vec<Token> tokens, ptr<int> index, TokenKind kind) {
    match tokens.buf[index@].kind {
        TokenKind:Unset {},
        kind {
            index@ += 1;
            ret true;
        },
        _ {
            ret false;
        },
    };
};

Expr parse_local(File file, vec<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.label = parse_label(file, tokens, index, true);
    expr.tags = parse_tags(file, tokens, index);

    Token token = tokens.buf[index@];

    ~l match token.kind {
        TokenKind:Ident {
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            if opt_token(tokens, index, TokenKind:LeftParen) {
                vec<Expr> args = vec<Expr>:new(2);
                ~ll loop {
                    if opt_token(tokens, index, TokenKind:RightParen) {
                        ret ~ll;
                    };

                    args.push(parse_local(file, tokens, index));

                    if opt_token(tokens, index, TokenKind:Comma) == false {
                        expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last argument");
                        ret ~ll;
                    };
                };
                expr.kind = ExprKind:SubCall(path, args);
                ` sub call
                ret ~l;
            };

            if opt_token(tokens, index, TokenKind:Ident) {
                ptr<u8> name = tokens.buf[index@ - 1].value;
                ptr<Expr> value = none;
                if opt_token(tokens, index, TokenKind:Assign) {
                    value = alloc<Expr>(parse_local(file, tokens, index));
                };
                expr.kind = ExprKind:LocalVarDef(path, name, value);
                ret ~l;
            };

            expr.kind = ExprKind:VarUse(path);
        },
        TokenKind:Var {
            index@ += 1;
            ptr<u8> name = expect_token(file, tokens, index, TokenKind:Ident, "expected identifier after 'var' keyword").value;
            ptr<Expr> value = none;
            if opt_token(tokens, index, TokenKind:Assign) {
                value = alloc<Expr>(parse_local(file, tokens, index));
            };
            expr.kind = ExprKind:LocalVarDef(none, name, value);
        },
        TokenKind:StringLit {
            expr.kind = ExprKind:StringLit(token.value);
            index@ += 1;
        },
        TokenKind:CharLit {
            expr.kind = ExprKind:CharLit(token.value);
            index@ += 1;
        },
        TokenKind:IntLit {
            expr.kind = ExprKind:IntLit(token.value);
            index@ += 1;
        },
        TokenKind:FloatLit {
            expr.kind = ExprKind:FloatLit(token.value);
            index@ += 1;
        },
        TokenKind:True {
            expr.kind = ExprKind:BoolLit(true);
            index@ += 1;
        },
        TokenKind:False {
            expr.kind = ExprKind:BoolLit(false);
            index@ += 1;
        },
        TokenKind:This {
            expr.kind = ExprKind:ThisLit;
            index@ += 1;
        },
        TokenKind:Type {
            expr.kind = ExprKind:TypeLit;
            index@ += 1;
        },
        TokenKind:LeftBrace {
            index@ += 1;
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:LocalBlock(body);
        },
        TokenKind:If {
            index@ += 1;
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'if' condition");
            vec<Expr> body = parse_block(file, tokens, index, true);
            ptr<Expr> _if = alloc<Expr>(Expr {
                tags = vec<Expr> { len = 0, },
                label = none,
                kind = ExprKind:IfBranch(cond, body),
            });
            vec<Expr> _elif = vec<Expr>:new(1);
            while opt_token(tokens, index, TokenKind:Elif) {
                cond = alloc<Expr>(parse_local(file, tokens, index));
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'elif' condition");
                vec<Expr> body = parse_block(file, tokens, index, true);
                _elif.push(Expr {
                    tags = vec<Expr> { len = 0, },
                    label = none,
                    kind = ExprKind:IfBranch(cond, body),
                });
            };
            if opt_token(tokens, index, TokenKind:Else) {
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'else' keyword");
                vec<Expr> body = parse_block(file, tokens, index, true);
                _elif.push(Expr {
                    tags = vec<Expr> { len = 0, },
                    label = none,
                    kind = ExprKind:ElseBranch(body),
                });
            };
            expr.kind = ExprKind:If(_if, _elif, none);
        },
        TokenKind:Loop {
            index@ += 1;
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'loop' keyword");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Loop(body);
        },
        TokenKind:While {
            index@ += 1;
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'while' condition");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:While(cond, body);
        },
        TokenKind:For {
            index@ += 1;

        },
        TokenKind:Each {
            index@ += 1;
            vec<VarName> vars = vec<VarName>:new(2);
            while opt_token(tokens, index, TokenKind:Ident) {
                vars.push(VarName {
                    name = tokens.buf[index@ - 1].value,
                });
                opt_token(tokens, index, TokenKind:Comma);
            };
            if vars.len == 0 {
                throw(file, tokens.buf[index@ - 1].index, "expected sex");
            };
            expect_token(file, tokens, index, TokenKind:In, "expected 'in' keyword after 'each' variables");
            ptr<Expr> iter = alloc<Expr>(parse_local(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'each' iterator");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Each(vars, iter, body);
        },
        TokenKind:Match {
            index@ += 1;
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'match' value");
            vec<Expr> body = vec<Expr>:new(4);
            ~lll loop {
                if opt_token(tokens, index, TokenKind:RightBrace) {
                    expr.kind = ExprKind:Match(value, body);
                    ret ~l;
                };
                vec<Expr> values = vec<Expr>:new(2);
                ~llll loop {
                    if opt_token(tokens, index, TokenKind:LeftBrace) {
                        jump ~b;
                    };
                    values.push(parse_local(file, tokens, index));
                    if opt_token(tokens, index, TokenKind:Comma) == false {
                        ret ~llll;
                    };
                };
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'match' case value");
                ~b while false {}; 
                vec<Expr> case_body = parse_block(file, tokens, index, true);
                body.push(Expr {
                    tags = vec<Expr> {
                        len = 0,
                    },
                    label = none,
                    kind = ExprKind:Case(values, case_body),
                });
                if opt_token(tokens, index, TokenKind:Comma) == false {
                    ret ~lll;
                };
            };
            expect_token(file, tokens, index, TokenKind:RightBrace, "expected '}' after 'match' body");
            expr.kind = ExprKind:Match(value, body);
        },
        TokenKind:Ret {
            index@ += 1;
            ptr<u8> label = parse_label(file, tokens, index, false);
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index));
            expr.kind = ExprKind:Ret(label, value);
        },
        TokenKind:Next {
            index@ += 1;
            expr.kind = ExprKind:Next(parse_label(file, tokens, index, false));
        },
        TokenKind:Jump {
            index@ += 1;
            expr.kind = ExprKind:Jump(parse_label(file, tokens, index, false));
        },
        TokenKind:Try {
            index@ += 1;
            ptr<u8> label = parse_label(file, tokens, index, false);
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index));
            expr.kind = ExprKind:Try(label, value);
        },
        _ {
            throw(file, token.index, "expected a local expression");
        }
    };

    ret expr;
};

comp OpData {
    int op_level,
    TokenKind op_kind,
    int op_index,
};

OpData get_op_data(File file, vec<Token> tokens, int start, int end) {
    arr<int> scopes = arr<int>:new(3);
    scopes.buf[2] = scopes.buf[1] = scopes.buf[0] = 0;
    int skip_blocks = 0;
    int need_operator = false;

    int i = start - 1;
    ~l while (i) < end {
        Token token = tokens.buf[i += 1];

        if skip_blocks > 0 {
            match token.kind {
                TokenKind:LeftBrace {
                    scopes.buf[1] += 1;
                },
                TokenKind:RightBrace {
                    scopes.buf[1] -= 1;
                    if scopes.buf[1] == 0 {
                        skip_blocks -= 1;
                    };
                },
            };
            next ~l;
        };

        match token.kind {
            TokenKind:LeftParen {
                scopes.buf[0] += 1; next ~l;
            },
            TokenKind:RightParen {
                if (scopes.buf[0] -= 1) == 0 {
                    throw(file, token.index, "unexpected ')'");
                }; next ~l;
            },
        };

        match token.kind {
            TokenKind:LeftBrace {
                scopes.buf[1] += 1; next ~l;
            },
            TokenKind:RightBrace {
                if (scopes.buf[1] -= 1) == 0 {
                    throw(file, token.index, "unexpected '}'");
                }; next ~l;
            },
        };

        match token.kind {
            TokenKind:LeftBracket {
                if ((scopes.buf[0] == 0) & (scopes.buf[1] == 0)) & (scopes.buf[2] == 0) {
                    ` if op_level == 0 {
                    `     op_level = 1;
                    ` };
                    ` if op_level == 1 {
                    `     op_kind = token.kind;
                    `     op_index = i;
                    ` };
                };
                scopes.buf[2] += 1; next ~l;
            },
            TokenKind:RightBracket {
                if (scopes.buf[2] -= 1) == 0 {
                    throw(file, token.index, "unexpected ']'");
                }; next ~l;
            },
        };

        if ((scopes.buf[0] > 0) | (scopes.buf[1] > 0)) | (scopes.buf[2] > 0) {
            next ~l;
        };

        match token.kind {
            TokenKind:If { skip_blocks += 1; next ~l; },
            TokenKind:Loop { skip_blocks += 1; next ~l; },
            TokenKind:While { skip_blocks += 1; next ~l; },
            TokenKind:For { skip_blocks += 1; next ~l; },
            TokenKind:Each { skip_blocks += 1; next ~l; },
            TokenKind:Match { skip_blocks += 1; next ~l; },
        };

        ` match token.kind {
        `     TokenKind:Assign:
        `     TokenKind:AddAssign:
        `     TokenKind:SubtractAssign:
        `     TokenKind:MultiplyAssign:
        `     TokenKind:DivideAssign:
        `     TokenKind:ModuloAssign:
        `         if (opLevel < 6) {
        `             opLevel = 6;
        `             opIndex = i;
        `             opKind = token.kind;
        `         }
        `         break;
        `     TokenKind:As:
        `         if (opLevel < 5) {
        `             opLevel = 5;
        `         }
        `         if (opLevel == 5) {
        `             opKind = token.kind;
        `             opIndex = i;
        `         }
        `         break;
        `     TokenKind:Less:
        `         if (opLevel != 4) {
        `             let genericCount = 1;
        `             let tryEnd = end ? end : tokens.length + 1;
        `             f: for (let ii = i + 1; ii < tryEnd; ++ii) {
        `                 if (!end && tokens[ii].kind === tokenKind.SEMICOLON || tokens[ii].kind === endTokenKind) {
        `                     break;
        `                 }
        `                 switch (tokens[ii].kind) {
        `                     case tokenKind.LESS:
        `                         ++genericCount;
        `                         break;
        `                     case tokenKind.IDENT:
        `                     case tokenKind.COLON:
        `                     case tokenKind.COMMA:
        `                         break;
        `                     case tokenKind.GREATER:
        `                         if (--genericCount == 0) {
        `                             i = ii;
        `                             continue w;
        `                         }
        `                         break;
        `                     default:
        `                         break f;
        `                 }
        `             }
        `         }
        `     TokenKind:And:
        `     TokenKind:Or:
        `     TokenKind:LessEqual:
        `     TokenKind:Greater:
        `     TokenKind:GreaterEqual:
        `     TokenKind:Equal:
        `     TokenKind:NotEqual:
        `         if (opLevel == 4) {
        `             throw 'dont mix these (you mixed ' + token.kind + ' with ' + opKind + ')';
        `         }
        `         if (opLevel < 4) {
        `             opLevel = 4;
        `             opKind = token.kind;
        `             opIndex = i;
        `         }
        `         break;
        `     TokenKind:Add:
        `     TokenKind:Subtract:
        `         if (opLevel < 3) {
        `             opLevel = 3;
        `         }
        `         if (opLevel == 3) {
        `             opKind = token.kind;
        `             opIndex = i;
        `         }
        `         break;
        `     TokenKind:Multiply:
        `     TokenKind:Divide:
        `     TokenKind:Modulo:
        `         if (opLevel < 2) {
        `             opLevel = 2;
        `         }
        `         if (opLevel == 2) {
        `             opKind = token.kind;
        `             opIndex = i;
        `         }
        `         break;
        `     TokenKind:Dot:
        `     TokenKind:Not:
        `     TokenKind:Deref:
        `     TokenKind:Address:
        `         if (opLevel == 0) {
        `             opLevel = 1;
        `         }
        `         if (opLevel == 1) {
        `             opKind = token.kind;
        `             opIndex = i;
        `         }
        `         break;
        ` };
    };
};

Expr parse_global(File file, vec<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.label = parse_label(file, tokens, index, true);
    expr.tags = parse_tags(file, tokens, index);

    Token token = tokens.buf[index@];

    match token.kind {
        TokenKind:Tag {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'tag' path");
            vec<Expr> args = parse_fields(file, tokens, index);
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, true);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:TagDef(path, args, body);
        },
        TokenKind:LeftBrace {
            index@ += 1;
            expr.kind = ExprKind:Block(parse_block(file, tokens, index, false));
        },
        TokenKind:Mod {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expr.kind = ExprKind:Mod(path);
        },
        TokenKind:Use {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expr.kind = ExprKind:Use(path);
        },
        TokenKind:Comp {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'comp' path");
            vec<Expr> fields = parse_fields(file, tokens, index);
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, true);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Comp(path, fields, body);
        },
        TokenKind:Enum {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'comp' path");
            vec<Expr> opts = parse_options(file, tokens, index);
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, true);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Enum(path, opts, body);
        },
        TokenKind:Prop {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Prop(path, body);
        },
        TokenKind:Def {
            index@ += 1;
            ptr<Expr> _prop = alloc<Expr>(parse_path(file, tokens, index));
            ptr<Expr> _target = none;
            if opt_token(tokens, index, TokenKind:Comma) {
                _target = alloc<Expr>(parse_path(file, tokens, index));
            };
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Def(_prop, _target, body);
        },
        TokenKind:Sub {
            index@ += 1;

            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'sub' path");
            vec<Expr> args = parse_fields(file, tokens, index);

            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, true);
            } else {
                body.len = 0;
            };

            expr.kind = ExprKind:SubDef(none, path, args, body);
        },
        TokenKind:Var {
            index@ += 1;

            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            ptr<Expr> value = none;
            if opt_token(tokens, index, TokenKind:Assign) {
                index@ += 1;
                ` value = parse_local();
            };
            expr.kind = ExprKind:VarDef(none, path, value);
        },
        TokenKind:Ident {
            ptr<Expr> _type = alloc<Expr>(parse_path(file, tokens, index));

            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            token = tokens.buf[index@];
            match token.kind {
                TokenKind:LeftParen {
                    index@ += 1;

                    vec<Expr> args = parse_fields(file, tokens, index);

                    vec<Expr> body;
                    if opt_token(tokens, index, TokenKind:LeftBrace) {
                        body = parse_block(file, tokens, index, true);
                    } else {
                        body.len = 0;
                    };

                    expr.kind = ExprKind:SubDef(_type, path, args, body);
                },
                _ {
                    ptr<Expr> value = none;
                    if opt_token(tokens, index, TokenKind:Assign) {
                        index@ += 1;
                        ` value = parse_local();
                    };
                    expr.kind = ExprKind:VarDef(_type, path, value);
                },
            };
        },
        _ {
            throw(file, token.index, "expected a global expression");
        },
    };

    ret expr;
};

ptr<u8> parse_label(File file, vec<Token> tokens, ptr<int> index, bool opt) {
    if opt & (opt_token(tokens, index, TokenKind:Tilde) == false) {
        ret none;
    } elif opt == false {
        expect_token(file, tokens, index, TokenKind:Tilde, "expected a label");
    };
    ptr<u8> label = expect_token(file, tokens, index, TokenKind:Ident, "expected identifier after label declaration").value;
    ret label;
};

vec<Expr> parse_tags(File file, vec<Token> tokens, ptr<int> index) {
    vec<Expr> tags = vec<Expr>:new(2);

    ~l loop {
        match tokens.buf[index@].kind {
            TokenKind:Hash {
                Token token = tokens.buf[index@ += 1];
                match token.kind {
                    TokenKind:Ident {
                        vec<Expr> args = vec<Expr>:new(1);

                        Expr _tag = Expr {
                            kind = ExprKind:Tag(token.value, args),
                            tags = vec<Expr> {
                                len = 0,
                            },
                            label = none,
                        };

                        token = tokens.buf[index@ += 1];

                        match token.kind {
                            TokenKind:LeftParen {
                                ~ll loop {
                                    token = tokens.buf[index@ += 1];

                                    match token.kind {
                                        TokenKind:RightParen { ret ~ll; },
                                    };

                                    ` tag.args.push(parse_value());

                                    ` token = tokens[++index];
                                    ` if (token.kind === tokenKind.COMMA) {
                                    `     next ~ll;
                                    ` }
                                    ` else if (token.kind === tokenKind.RIGHT_PAREN) {
                                    `     ret ~ll;
                                    ` }

                                    throw(file, token.index, "unexpected smth in 'tag' declaration");
                                };

                                tags.push(_tag);
                                index@ += 1;
                            },
                            _ {
                                tags.push(_tag);
                                next ~l;
                            },
                        };
                    },
                    _ {
                        throw(file, token.index, "expected identifier after tag declaration");
                    },
                };
            },
            _ { ret ~l; },
        };
    };

    ret tags;
};

Expr parse_path(File file, vec<Token> tokens, ptr<int> index) {
    match tokens.buf[index@].kind {
        TokenKind:Ident {
            ret parse_opt_path(file, tokens, index);
        },
        _ {
            throw(file, tokens.buf[index@].index, "expected a path");
        },
    };
};

Expr parse_opt_path(File file, vec<Token> tokens, ptr<int> index) {
    vec<PathPart> path = vec<PathPart>:new(2);
    bool need_colon = false;
    ~l loop {
        Token token = tokens.buf[index@];
        if need_colon {
            match token.kind {
                TokenKind:Colon {
                    need_colon = false;
                },
                _ {
                    ret ~l;
                },
            };
        } else {
            match token.kind {
                TokenKind:Ident {
                    PathPart part = PathPart {
                        name = token.value,
                        gens = vec<Expr>:new(2),
                    };
                    match tokens.buf[index@ + 1].kind {
                        TokenKind:Less {
                            index@ += 2;
                            ~ll loop {
                                token = tokens.buf[index@];
                                match token.kind {
                                    TokenKind:Greater {
                                        ret ~ll;
                                    },
                                    _ {},
                                };
                                vec<Expr>:push(part.gens$, parse_path(file, tokens, index));
                                bool a = opt_token(tokens, index, TokenKind:Comma);
                            };
                        },
                    };
                    path.push(part);
                    need_colon = true;
                },
                _ {
                    if path.len > 0 {
                        throw(file, token.index, "expected identifier after ':'");
                    } else {
                        ret ~l;
                    };
                },
            };
        };
        index@ += 1;
    };
    ret Expr {
        kind = ExprKind:Path(path),
        tags = vec<Expr> {
            len = 0,
        },
        label = none,
    };
};

vec<Expr> parse_fields(File file, vec<Token> tokens, ptr<int> index) {
    vec<Expr> fields = vec<Expr>:new(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret fields;
        };

        Expr field = Expr {
            kind = ExprKind:Field(
                alloc<Expr>(parse_path(file, tokens, index)),
                expect_token(file, tokens, index, TokenKind:Ident, "expected field name in 'comp' definition").value
            ),
            tags = vec<Expr> {
                len = 0,
            },
            label = none,
        };
        fields.push(field);

        if opt_token(tokens, index, TokenKind:Comma) == false {
            ret ~l;
        };
    };
    expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last field");

    ret fields;
};

vec<Expr> parse_options(File file, vec<Token> tokens, ptr<int> index) {
    vec<Expr> opts = vec<Expr>:new(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret opts;
        };

        ptr<u8> name = expect_token(file, tokens, index, TokenKind:Ident, "expected option name in 'enum' definition").value;

        vec<Expr> fields;
        if opt_token(tokens, index, TokenKind:LeftParen) {
            fields = parse_fields(file, tokens, index);    
        } else {
            fields.len = 0;
        };

        opts.push(Expr {
            kind = ExprKind:Option(name, fields),
            tags = vec<Expr> {
                len = 0,
            },
            label = none,
        });

        if opt_token(tokens, index, TokenKind:Comma) == false {
            ret ~l;
        };
    };
    expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last option");

    ret opts;
};

sub print_exprs(vec<Expr> exprs) {
    fmt:print("Exprs:");
    print_expr_vec(exprs, 0);
    putchar('\n');
};

sub print_expr_vec(vec<Expr> exprs, int depth) {
    for i = 0, (i) < exprs.len, i += 1 {
        putchar('\n');
        indent(depth);
        print_expr(exprs.buf[i], depth);
    };
    putchar('\n');
};

sub print_opt_expr_vec(vec<Expr> exprs, int depth, ptr<u8> name, bool _next) {
    if exprs.len > 0 {
        if _next {
            fmt:print(", ");
        };
        fmt:print("%s = [", name);
        print_expr_vec(exprs, depth + 1);
        indent(depth);
        putchar(']');
    };
};

sub print_expr(Expr expr, int depth) {
    set_color(Color:Green);
    fmt:print("%s", get_expr_name(expr.kind));
    set_color(Color:Reset);
    putchar('(');
    if expr.label != none {
        fmt:print("label = %s, ", expr.label);
    };
    print_opt_expr_vec(expr.tags, 0, "tags", false);
    if expr.tags.len != 0 {
        fmt:print(", ");
    };
    match expr.kind {
        ExprKind:Tag(name, args) {
            fmt:print("name = %s", name);
            print_opt_expr_vec(args, depth, "args", true);
        },
        ExprKind:Path(path) {
            for i = 0, (i) < path.len, i += 1 {
                PathPart part = path.buf[i];
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("name = %s", part.name);
                print_opt_expr_vec(part.gens, depth, "gens", true);
            };
        },
        ExprKind:TagDef(path, args, body) {
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Block(body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:Mod(path) {
            fmt:print("path = ");
            print_expr(path@, depth);
        },
        ExprKind:Use(path) {
            fmt:print("path = ");
            print_expr(path@, depth);
        },
        ExprKind:Field(_type, name) {
            fmt:print("type = ");
            print_expr(_type@, depth);
            fmt:print(", name = %s", name);
        },
        ExprKind:Comp(path, fields, body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(fields, depth, "fields", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Enum(path, fields, opts, body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(fields, depth, "fields", true);
            print_opt_expr_vec(opts, depth, "opts", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Option(name, fields) {
            fmt:print("name = %s", name);
            print_opt_expr_vec(fields, depth, "fields", true);
        },
        ExprKind:Prop(path, body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Def(_prop, target, body) {
            fmt:print("prop = ");
            print_expr(_prop@, depth);
            if target != none {
                fmt:print(", target = ");
                print_expr(target@, depth);
            };
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:SubDef(ret_type, path, args, body) {
            if ret_type != none {
                fmt:print("ret_type = ");
                print_expr(ret_type@, depth);
                fmt:print(", ");
            };
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:VarDef(_type, path, value) {
            if _type != none {
                fmt:print("type = ");
                print_expr(_type@, depth);
                fmt:print(", ");
            };
            fmt:print("path = ");
            print_expr(path@, depth);
            if value != none {
                fmt:print(", value = ");
                print_expr(value@, depth);
            };
        },
        ExprKind:LocalVarDef(_type, name, value) {
            if _type != none {
                fmt:print("type = ");
                print_expr(_type@, depth);
                fmt:print(", ");
            };
            fmt:print("name = %s", name);
            if value != none {
                fmt:print(", value = ");
                print_expr(value@, depth);
            };
        },
        ExprKind:SubCall(path, args) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
        },
        ExprKind:VarUse(path) {
            fmt:print("path = ");
            print_expr(path@, depth);
        },
        ExprKind:StringLit(value) {
            fmt:print("%s", value);
        },
        ExprKind:CharLit(value) {
            fmt:print("%s", value);
        },
        ExprKind:IntLit(value) {
            fmt:print("%s", value);
        },
        ExprKind:FloatLit(value) {
            fmt:print("%s", value);
        },
        ExprKind:BoolLit(value) {
            if value {
                fmt:print("true");
            } else {
                fmt:print("false");
            };
        },
        ExprKind:ThisLit {},
        ExprKind:TypeLit {},
        ExprKind:LocalBlock(body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:If(_if, _elif, _else) {
            fmt:print("if = ");
            print_expr(_if@, depth);
            print_opt_expr_vec(_elif, depth, "elif", true);
            if _else != none {
                fmt:print(", else = ");
                print_expr(_else@, depth);
            };
        },
        ExprKind:IfBranch(cond, body) {
            fmt:print("cond = ");
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:ElseBranch(body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:Loop(body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:While(cond, body) {
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:For(iter, cond, _next, body) {
            print_opt_expr_vec(iter, depth, "iter", false);
            print_expr(cond@, depth);
            print_opt_expr_vec(_next, depth, "next", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Each(vars, iter, body) {
            fmt:print("vars = [");
            for i = 0, (i) < vars.len, i += 1 {
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("%s", vars.buf[i]);
            };
            fmt:print("], iter = ");
            print_expr(iter@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Match(value, cases) {
            fmt:print("value = ");
            print_expr(value@, depth);
            print_opt_expr_vec(cases, depth, "cases", true);
        },
        ExprKind:Case(values, body) {
            print_opt_expr_vec(values, depth, "values", false);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Ret(label, value) {
            if label != none {
                fmt:print("label = %s, ", label);
            };
            fmt:print("value = ");
            print_expr(value@, depth);
        },
        ExprKind:Next(label) {
            if label != none {
                fmt:print("label = %s", label);
            };
        },
        ExprKind:Jump(label) {
            if label != none {
                fmt:print("label = %s", label);
            };
        },
        ExprKind:Try(label, value) {
            if label != none {
                fmt:print("label = %s, ", label);
            };
            fmt:print("value = ");
            print_expr(value@, depth);
        },
        ExprKind:UnaryOp(value, kind) {
            print_expr(value@, depth);
        },
        ExprKind:BinaryOp(lhs, rhs, kind) {
            print_expr(lhs@, depth);
            print_expr(rhs@, depth);
        },
        ExprKind:TernaryOp(cond, valueA, valueB) {
            print_expr(cond@, depth);
            print_expr(valueA@, depth);
            print_expr(valueB@, depth);
        },
    };
    putchar(')');
};

ptr<u8> get_expr_name(ExprKind kind) {
    match kind {
        ExprKind:Tag { ret "TAG"; },
        ExprKind:Path { ret "PATH"; },
        ExprKind:TagDef { ret "TAG_DEF"; },
        ExprKind:Block { ret "BLOCK"; },
        ExprKind:Mod { ret "MOD"; },
        ExprKind:Use { ret "USE"; },
        ExprKind:Field { ret "FIELD"; },
        ExprKind:Comp { ret "COMP"; },
        ExprKind:Enum { ret "ENUM"; },
        ExprKind:Option { ret "OPTION"; },
        ExprKind:Prop { ret "PROP"; },
        ExprKind:Def { ret "DEF"; },
        ExprKind:SubDef { ret "SUB_DEF"; },
        ExprKind:VarDef { ret "VAR_DEF"; },
        ExprKind:LocalVarDef { ret "LOCAL_VAR_DEF"; },
        ExprKind:SubCall { ret "SUB_CALL"; },
        ExprKind:VarUse { ret "VAR_USE"; },
        ExprKind:StringLit { ret "STRING_LIT"; },
        ExprKind:CharLit { ret "CHAR_LIT"; },
        ExprKind:IntLit { ret "INT_LIT"; },
        ExprKind:FloatLit { ret "FLOAT_LIT"; },
        ExprKind:BoolLit { ret "BOOL_LIT"; },
        ExprKind:ThisLit { ret "THIS_LIT"; },
        ExprKind:TypeLit { ret "TYPE_LIT"; },
        ExprKind:LocalBlock { ret "LOCAL_BLOCK"; },
        ExprKind:If { ret "IF"; },
        ExprKind:IfBranch { ret "IF_BRANCH"; },
        ExprKind:ElseBranch { ret "ELSE_BRANCH"; },
        ExprKind:Loop { ret "LOOP"; },
        ExprKind:While { ret "WHILE"; },
        ExprKind:For { ret "FOR"; },
        ExprKind:Each { ret "EACH"; },
        ExprKind:Match { ret "MATCH"; },
        ExprKind:Case { ret "CASE"; },
        ExprKind:Ret { ret "RET"; },
        ExprKind:Next { ret "NEXT"; },
        ExprKind:Jump { ret "JUMP"; },
        ExprKind:Try { ret "TRY"; },
        ExprKind:UnaryOp { ret "UNARY_OP"; },
        ExprKind:BinaryOp { ret "BINARY_OP"; },
        ExprKind:TernaryOp { ret "TERNARY_OP"; },
    };
};

sub indent(int depth) {
    for i = 0, i <= depth, i += 1 {
        fmt:print("  ");
    };
};
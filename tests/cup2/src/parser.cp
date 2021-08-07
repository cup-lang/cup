comp PathPart (
    str name,
    vec<Expr> gens,
);

comp FieldValue (
    str name,
    ptr<Expr> value,
);

enum ExprKind (
    Empty,
    Tag(str name, vec<Expr> args),
    Path(vec<PathPart> path, int index),
    TagDef(ptr<Expr> path, vec<Expr> args, vec<Expr> body),
    Block(vec<Expr> body),
    Mod(ptr<Expr> path, vec<Expr> body),
    Use(ptr<Expr> path),
    Field(ptr<Expr> _type, str name),
    Comp(ptr<Expr> path, vec<Expr> fields, vec<Expr> body),
    Enum(ptr<Expr> path, vec<Expr> opts, vec<Expr> body),
    Option(str name, vec<Expr> fields),
    Prop(ptr<Expr> path, vec<Expr> body),
    Def(ptr<Expr> _prop, opt<ptr<Expr>> target, vec<Expr> body),
    SubDef(opt<ptr<Expr>> ret_type, ptr<Expr> path, vec<Expr> args, vec<Expr> body),
    VarDef(opt<ptr<Expr>> _type, ptr<Expr> path, opt<ptr<Expr>> value),

    LocalVarDef(opt<ptr<Expr>> _type, str name, opt<ptr<Expr>> value),
    SubCall(ptr<Expr> path, vec<Expr> args),
    VarUse(ptr<Expr> path),
    CompInst(ptr<Expr> _type, vec<FieldValue> field_vals),
    EnumInst(ptr<Expr> _type, ptr<Expr> path, int opt_index, vec<Expr> args),
    StringLit(str value),
    CharLit(str value),
    IntLit(str value),
    FloatLit(str value),
    BoolLit(bool value),
    ThisLit,
    TypeLit,
    LocalBlock(vec<Expr> body),
    If(ptr<Expr> _if, vec<Expr> _elif, opt<ptr<Expr>> _else),
    IfBranch(ptr<Expr> cond, vec<Expr> body),
    ElseBranch(vec<Expr> body),
    Loop(vec<Expr> body),
    While(ptr<Expr> cond, vec<Expr> body),
    For(str iter, opt<ptr<Expr>> iter_value, ptr<Expr> cond, ptr<Expr> _next, vec<Expr> body),
    Each(vec<str> vars, ptr<Expr> iter, vec<Expr> body),
    Match(ptr<Expr> value, vec<Expr> cases),
    Case(vec<Expr> values, vec<Expr> body),
    Ret(opt<str> label, opt<ptr<Expr>> value),
    Next(opt<str> label),
    Jump(opt<str> label),
    Try(opt<str> label, ptr<Expr> value),

    UnaryOp(ptr<Expr> value, TokenKind kind),
    BinaryOp(ptr<Expr> lhs, ptr<Expr> rhs, TokenKind kind),
    TernaryOp(ptr<Expr> cond, ptr<Expr> valueA, ptr<Expr> valueB),
);

comp Expr (
    ExprKind kind,
    vec<Expr> tags,
    opt<str> label,
);

vec<Expr> parse(File file, vec<Token> tokens) {
    int index = 0;
    vec<Expr> exprs = vec<Expr>:new_with_cap(16);

    loop {
        match tokens[index].kind {
            TokenKind:Empty {
                ret exprs;
            },
        };

        exprs.push(parse_global(file, tokens, index$));

        match tokens[index].kind {
            TokenKind:Semicolon {},
            TokenKind:Empty {
                ret exprs;
            },
            _ {
                file.throw(tokens[index].index, "unexpected token after last expr");
            },
        };

        index += 1;
    };
};

vec<Expr> parse_block(File file, vec<Token> tokens, ptr<int> index, bool local) {
    vec<Expr> exprs = vec<Expr>:new_with_cap(4);

    loop {
        if tokens[index@].kind == TokenKind:RightBrace {
            index@ += 1;
            ret exprs;
        };

        Expr expr;
        if local {
            expr = parse_local(file, tokens, index, 0, false);
        } else {
            expr = parse_global(file, tokens, index);
        };
        exprs.push(expr);

        match tokens[index@].kind {
            TokenKind:Semicolon {},
            TokenKind:RightBrace {
                index@ += 1;
                ret exprs;
            },
            _ {
                file.throw(tokens[index@].index, "unexpected token after last expr");
            },
        };

        index@ += 1;
    };
};

Token expect_token(File file, vec<Token> tokens, ptr<int> index, TokenKind kind, ptr<u8> error) {
    Token token = tokens[index@];
    match token.kind {
        kind {},
        _ {
            file.throw(token.index, error);
        },
    };
    index@ += 1;
    ret token;
};

bool opt_token(vec<Token> tokens, ptr<int> index, TokenKind kind) {
    match tokens[index@].kind {
        kind {
            index@ += 1;
            ret true;
        },
        _ {
            ret false;
        },
    };
};

Expr parse_local(File file, vec<Token> tokens, ptr<int> index, int op_level, bool opt) {
    Expr expr;
    expr.label = parse_label(file, tokens, index, true);
    expr.tags = parse_tags(file, tokens, index);

    Token token = tokens[index@];

    ~l match token.kind {
        TokenKind:LeftParen {
            index@ += 1;
            expr = parse_local(file, tokens, index, 0, false);
            expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after '('");
        },
        TokenKind:Ident {
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            if opt_token(tokens, index, TokenKind:LeftParen) {
                vec<Expr> args = vec<Expr>:new_with_cap(4);
                ~ll loop {
                    if opt_token(tokens, index, TokenKind:RightParen) {
                        ret ~ll;
                    };

                    args.push(parse_local(file, tokens, index, 0, false));

                    if opt_token(tokens, index, TokenKind:Comma) == false {
                        expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last argument");
                        ret ~ll;
                    };
                };
                expr.kind = ExprKind:SubCall(path, args);
                ret ~l;
            };

            if opt_token(tokens, index, TokenKind:Ident) {
                str name = tokens[index@ - 1].value;
                opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
                if opt_token(tokens, index, TokenKind:Assign) {
                    value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
                };
                expr.kind = ExprKind:LocalVarDef(opt<ptr<Expr>>:Some(path), name, value);
                ret ~l;
            };

            expr.kind = ExprKind:VarUse(path);
        },
        TokenKind:Var {
            index@ += 1;
            str name = expect_token(file, tokens, index, TokenKind:Ident, "expected identifier after 'var' keyword").value;
            opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Assign) {
                value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
            };
            expr.kind = ExprKind:LocalVarDef(opt<ptr<Expr>>:None, name, value);
        },
        TokenKind:New {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'new' path");
            vec<FieldValue> field_vals = vec<FieldValue>:new_with_cap(4);
            ~f loop {
                if opt_token(tokens, index, TokenKind:RightBrace) {
                    ret ~f;
                };

                str name = expect_token(file, tokens, index, TokenKind:Ident, "expected field name").value;
                expect_token(file, tokens, index, TokenKind:Assign, "expected '=' after field name");
                field_vals.push(new FieldValue {
                    name = name,
                    value = alloc<Expr>(parse_local(file, tokens, index, 0, false)),
                });

                if opt_token(tokens, index, TokenKind:Comma) == false {
                    expect_token(file, tokens, index, TokenKind:RightBrace, "expected '}' after last field");
                    ret ~f;
                };
            };
            expr.kind = ExprKind:CompInst(path, field_vals);
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
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'if' condition");
            vec<Expr> body = parse_block(file, tokens, index, true);
            ptr<Expr> _if = alloc<Expr>(new Expr {
                tags = new vec<Expr> { len = 0, },
                label = opt<str>:None,
                kind = ExprKind:IfBranch(cond, body),
            });
            vec<Expr> _elif = vec<Expr>:new_with_cap(1);
            while opt_token(tokens, index, TokenKind:Elif) {
                cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'elif' condition");
                vec<Expr> body = parse_block(file, tokens, index, true);
                _elif.push(new Expr {
                    tags = new vec<Expr> { len = 0, },
                    label = opt<str>:None,
                    kind = ExprKind:IfBranch(cond, body),
                });
            };
            opt<ptr<Expr>> _else = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Else) {
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'else' keyword");
                vec<Expr> body = parse_block(file, tokens, index, true);
                _else = opt<ptr<Expr>>:Some(alloc<Expr>(new Expr {
                    tags = new vec<Expr> { len = 0, },
                    label = opt<str>:None,
                    kind = ExprKind:ElseBranch(body),
                }));
            };
            expr.kind = ExprKind:If(_if, _elif, _else);
        },
        TokenKind:Loop {
            index@ += 1;
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'loop' keyword");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Loop(body);
        },
        TokenKind:While {
            index@ += 1;
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'while' condition");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:While(cond, body);
        },
        TokenKind:For {
            index@ += 1;
            str iter = expect_token(file, tokens, index, TokenKind:Ident, "expected 'for' iterator name").value;
            opt<ptr<Expr>> iter_value = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Assign) {
                iter_value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
            };
            expect_token(file, tokens, index, TokenKind:Comma, "expected a ',' after 'for' iterator name");
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:Comma, "expected a ',' after 'for' condition");
            ptr<Expr> _next = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'for' next");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:For(iter, iter_value, cond, _next, body);
        },
        TokenKind:Each {
            index@ += 1;
            vec<str> vars = vec<str>:new_with_cap(2);
            while opt_token(tokens, index, TokenKind:Ident) {
                vars.push(tokens[index@ - 1].value);
                opt_token(tokens, index, TokenKind:Comma);
            };
            if vars.len == 0 {
                file.throw(tokens[index@ - 1].index, "expected sex");
            };
            expect_token(file, tokens, index, TokenKind:In, "expected 'in' keyword after 'each' variables");
            ptr<Expr> iter = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'each' iterator");
            vec<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Each(vars, iter, body);
        },
        TokenKind:Match {
            index@ += 1;
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'match' value");
            vec<Expr> body = vec<Expr>:new_with_cap(4);
            ~lll loop {
                if opt_token(tokens, index, TokenKind:RightBrace) {
                    expr.kind = ExprKind:Match(value, body);
                    ret ~l;
                };
                vec<Expr> values = vec<Expr>:new_with_cap(2);
                ~llll loop {
                    if opt_token(tokens, index, TokenKind:LeftBrace) {
                        jump ~b;
                    };
                    values.push(parse_local(file, tokens, index, 0, false));
                    if opt_token(tokens, index, TokenKind:Comma) == false {
                        ret ~llll;
                    };
                };
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'match' case value");
                ~b while false {};
                vec<Expr> case_body = parse_block(file, tokens, index, true);
                body.push(new Expr {
                    kind = ExprKind:Case(values, case_body),
                    tags = new vec<Expr> { len = 0, },
                    label = opt<str>:None,
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
            opt<str> label = parse_label(file, tokens, index, true);
            Expr value = parse_local(file, tokens, index, 0, true);
            match value.kind {
                ExprKind:Empty {
                    expr.kind = ExprKind:Ret(label, opt<ptr<Expr>>:None);
                },
                _ {
                    expr.kind = ExprKind:Ret(label, opt<ptr<Expr>>:Some(alloc<Expr>(value)));
                },
            };
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
            opt<str> label = parse_label(file, tokens, index, true);
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expr.kind = ExprKind:Try(label, value);
        },
        _ {
            if opt {
                expr.kind = ExprKind:Empty;
                ret expr;
            } else {
                file.throw(token.index, "expected a local expression");
            };
        }
    };

    ~o loop {
        int new_op_level = 0;
        token = tokens[index@];
        match token.kind {
            TokenKind:QuestionMark {
                index@ += 1;
                Expr valueA = parse_local(file, tokens, index, 0, false);
                expect_token(file, tokens, index, TokenKind:Comma, "expected ',' after smth");
                Expr valueB = parse_local(file, tokens, index, 0, false);
                expr = new Expr {
                    tags = new vec<Expr> { len = 0, },
                    label = opt<str>:None,
                    kind = ExprKind:TernaryOp(alloc<Expr>(expr), alloc<Expr>(valueA), alloc<Expr>(valueB)),
                };
                ret ~o;
            },
            TokenKind:Assign,
            TokenKind:AddAssign,
            TokenKind:SubtractAssign,
            TokenKind:MultiplyAssign,
            TokenKind:DivideAssign,
            TokenKind:ModuloAssign {
                new_op_level = 7;
            },
            TokenKind:As {
                new_op_level = 6;
            },
            TokenKind:Less,
            TokenKind:And,
            TokenKind:Or,
            TokenKind:LessEqual,
            TokenKind:Greater,
            TokenKind:GreaterEqual,
            TokenKind:Equal,
            TokenKind:NotEqual {
                new_op_level = 5;
            },
            TokenKind:Add,
            TokenKind:Subtract {
                new_op_level = 4;
            },
            TokenKind:Multiply,
            TokenKind:Divide,
            TokenKind:Modulo {
                new_op_level = 3;
            },
            TokenKind:Not,
            TokenKind:Deref,
            TokenKind:Address {
                new_op_level = 2;
            },
            TokenKind:LeftBracket {
                if op_level == 1 {
                    ret ~o;
                };
                index@ += 1;
                jump ~e;
            },
            TokenKind:Dot {
                new_op_level = 1;
            },
        };
        if new_op_level == 2 {
            if (op_level == 0) | (new_op_level < op_level) {
                index@ += 1;
                expr = new Expr {
                    tags = new vec<Expr> { len = 0, },
                    label = opt<str>:None,
                    kind = ExprKind:UnaryOp(alloc<Expr>(expr), token.kind),
                };
                next ~o;
            };
            ret ~o;
        };
        if (new_op_level == 0) | ((op_level != 0) & (op_level != 6) & (new_op_level >= op_level)) {
            ret ~o;
        };
        index@ += 1;
        ~e while false {};
        expr.kind = ExprKind:BinaryOp(alloc<Expr>(expr), alloc<Expr>(parse_local(file, tokens, index, new_op_level, false)), token.kind);
        match token.kind {
            TokenKind:LeftBracket {
                expect_token(file, tokens, index, TokenKind:RightBracket, "expected ']' after '['");
            },
            TokenKind:Add {
                match expr.kind {
                    ExprKind:BinaryOp(ptr<Expr> lhs, ptr<Expr> rhs) {
                        match lhs@.kind {
                            ExprKind:StringLit(str lhs_str) {
                                match rhs@.kind {
                                    ExprKind:StringLit(str rhs_str) {
                                        dstr temp = lhs_str.to_dstr();
                                        temp.join(rhs_str);
                                        mem:free(rhs_str.buf);
                                        expr.kind = ExprKind:StringLit(temp.to_str());
                                    },
                                };
                            },
                        };
                    },
                };
            },
        };
    };

    ret expr;
};

Expr parse_global(File file, vec<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.label = parse_label(file, tokens, index, true);
    expr.tags = parse_tags(file, tokens, index);

    Token token = tokens[index@];

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
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Mod(path, body);
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
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Comp(path, fields, body);
        },
        TokenKind:Enum {
            index@ += 1;
            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'enum' path");
            vec<Expr> opts = parse_options(file, tokens, index);
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
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
            opt<ptr<Expr>> target = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Comma) {
                target = opt<ptr<Expr>>:Some(alloc<Expr>(parse_path(file, tokens, index)));
            };
            vec<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Def(_prop, target, body);
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

            expr.kind = ExprKind:SubDef(opt<ptr<Expr>>:None, path, args, body);
        },
        TokenKind:Var {
            index@ += 1;

            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Assign) {
                value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
            };
            expr.kind = ExprKind:VarDef(opt<ptr<Expr>>:None, path, value);
        },
        TokenKind:Ident {
            ptr<Expr> _type = alloc<Expr>(parse_path(file, tokens, index));

            ptr<Expr> path = alloc<Expr>(parse_path(file, tokens, index));

            token = tokens[index@];
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

                    expr.kind = ExprKind:SubDef(opt<ptr<Expr>>:Some(_type), path, args, body);
                },
                _ {
                    opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
                    if opt_token(tokens, index, TokenKind:Assign) {
                        value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
                    };
                    expr.kind = ExprKind:VarDef(opt<ptr<Expr>>:Some(_type), path, value);
                },
            };
        },
        _ {
            file.throw(token.index, "expected a global expression");
        },
    };

    ret expr;
};

opt<str> parse_label(File file, vec<Token> tokens, ptr<int> index, bool opt) {
    if opt & (opt_token(tokens, index, TokenKind:Tilde) == false) {
        ret opt<str>:None;
    } elif opt == false {
        expect_token(file, tokens, index, TokenKind:Tilde, "expected a label");
    };
    ret opt<str>:Some(expect_token(file, tokens, index, TokenKind:Ident, "expected identifier after label declaration").value);
};

vec<Expr> parse_tags(File file, vec<Token> tokens, ptr<int> index) {
    vec<Expr> tags = vec<Expr>:new_with_cap(2);

    ~l loop {
        match tokens[index@].kind {
            TokenKind:Hash {
                Token token = tokens[index@ += 1];
                match token.kind {
                    TokenKind:Ident {
                        str name = token.value;
                        vec<Expr> args = vec<Expr>:new_with_cap(1);

                        token = tokens[index@ += 1];

                        match token.kind {
                            TokenKind:LeftParen {
                                index@ += 1;
                                ~ll loop {
                                    token = tokens[index@];

                                    if opt_token(tokens, index, TokenKind:RightParen) {
                                        ret ~ll;
                                    };

                                    args.push(parse_local(file, tokens, index, 0, false));

                                    if opt_token(tokens, index, TokenKind:Comma) {
                                        next ~ll;
                                    } else {
                                        expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last tag arg");
                                        ret ~ll;
                                    };
                                };
                            },
                        };
                        tags.push(new Expr {
                            kind = ExprKind:Tag(name, args),
                            tags = new vec<Expr> { len = 0, },
                            label = opt<str>:None,
                        });
                        next ~l;
                    },
                    _ {
                        file.throw(token.index, "expected identifier after tag declaration");
                    },
                };
            },
            _ { ret ~l; },
        };
    };

    ret tags;
};

Expr parse_path(File file, vec<Token> tokens, ptr<int> index) {
    match tokens[index@].kind {
        TokenKind:Ident {
            ret parse_opt_path(file, tokens, index);
        },
        _ {
            file.throw(tokens[index@].index, "expected a path");
        },
    };
};

Expr parse_opt_path(File file, vec<Token> tokens, ptr<int> index) {
    int start_index = index@;
    vec<PathPart> path = vec<PathPart>:new_with_cap(2);
    bool need_colon = false;
    ~l loop {
        Token token = tokens[index@];
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
                    PathPart part = new PathPart {
                        name = token.value,
                        gens = vec<Expr>:new_with_cap(2),
                    };
                    match tokens[index@ + 1].kind {
                        TokenKind:Less {
                            int old_index = index@;
                            index@ += 2;
                            ~ll loop {
                                token = tokens[index@];
                                match token.kind {
                                    TokenKind:Greater {
                                        ret ~ll;
                                    },
                                };
                                Expr gen = parse_opt_path(file, tokens, index);
                                match gen.kind {
                                    ExprKind:Path(vec<PathPart> gen_path) {
                                        if gen_path.len == 0 {
                                            index@ = old_index;
                                            part.gens.len = 0;
                                            ret ~ll;
                                        };
                                    },
                                };
                                vec<Expr>:push(part.gens$, gen);
                                opt_token(tokens, index, TokenKind:Comma);
                            };
                        },
                    };
                    path.push(part);
                    need_colon = true;
                },
                _ {
                    if path.len > 0 {
                        file.throw(token.index, "expected identifier after ':'");
                    } else {
                        ret ~l;
                    };
                },
            };
        };
        index@ += 1;
    };
    ret new Expr {
        kind = ExprKind:Path(path, tokens[start_index].index),
        tags = new vec<Expr> { len = 0, },
        label = opt<str>:None,
    };
};

vec<Expr> parse_fields(File file, vec<Token> tokens, ptr<int> index) {
    vec<Expr> fields = vec<Expr>:new_with_cap(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret fields;
        };

        Expr field = new Expr {
            kind = ExprKind:Field(
                alloc<Expr>(parse_path(file, tokens, index)),
                expect_token(file, tokens, index, TokenKind:Ident, "expected field name in 'comp' definition").value,
            ),
            tags = new vec<Expr> { len = 0, },
            label = opt<str>:None,
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
    vec<Expr> opts = vec<Expr>:new_with_cap(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret opts;
        };

        str name = expect_token(file, tokens, index, TokenKind:Ident, "expected option name in 'enum' definition").value;

        vec<Expr> fields;
        if opt_token(tokens, index, TokenKind:LeftParen) {
            fields = parse_fields(file, tokens, index);    
        } else {
            fields.len = 0;
        };

        opts.push(new Expr {
            kind = ExprKind:Option(name, fields),
            tags = new vec<Expr> { len = 0, },
            label = opt<str>:None,
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
    char:put('\n');
};

sub print_expr_vec(vec<Expr> exprs, int depth) {
    for i = 0, i < exprs.len, i += 1 {
        char:put('\n');
        indent(depth);
        print_expr(exprs[i], depth);
    };
    char:put('\n');
};

sub print_opt_expr_vec(vec<Expr> exprs, int depth, ptr<u8> name, bool _next) {
    if exprs.len > 0 {
        if _next {
            fmt:print(", ");
        };
        fmt:print("%s = [", name);
        print_expr_vec(exprs, depth + 1);
        indent(depth);
        char:put(']');
    };
};

sub print_expr(Expr expr, int depth) {
    color:set(Color:Green);
    fmt:print("%s", get_expr_name(expr.kind));
    color:reset();
    char:put('(');
    match expr.label {
        opt<str>:Some(str label) {
            fmt:print("label = %s, ", label.buf);
        },
    };
    print_opt_expr_vec(expr.tags, depth, "tags", false);
    if expr.tags.len != 0 {
        fmt:print(", ");
    };
    match expr.kind {
        ExprKind:Tag(str name, vec<Expr> args) {
            fmt:print("name = %s", name.buf);
            print_opt_expr_vec(args, depth, "args", true);
        },
        ExprKind:Path(vec<PathPart> path) {
            for i = 0, i < path.len, i += 1 {
                PathPart part = path[i];
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("name = %s", part.name.buf);
                print_opt_expr_vec(part.gens, depth, "gens", true);
            };
        },
        ExprKind:TagDef(ptr<Expr> path, vec<Expr> args, vec<Expr> body) {
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Block(vec<Expr> body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:Mod(ptr<Expr> path, vec<Expr> body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:Use(ptr<Expr> path) {
            fmt:print("path = ");
            print_expr(path@, depth);
        },
        ExprKind:Field(ptr<Expr> _type, str name) {
            fmt:print("type = ");
            print_expr(_type@, depth);
            fmt:print(", name = %s", name.buf);
        },
        ExprKind:Comp(ptr<Expr> path, vec<Expr> fields, vec<Expr> body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(fields, depth, "fields", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Enum(ptr<Expr> path, vec<Expr> opts, vec<Expr> body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(opts, depth, "opts", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Option(str name, vec<Expr> fields) {
            fmt:print("name = %s", name.buf);
            print_opt_expr_vec(fields, depth, "fields", true);
        },
        ExprKind:Prop(ptr<Expr> path, vec<Expr> body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Def(ptr<Expr> _prop, opt<ptr<Expr>> target, vec<Expr> body) {
            fmt:print("prop = ");
            print_expr(_prop@, depth);
            match target {
                opt<ptr<Expr>>:Some(ptr<Expr> _target) {
                    fmt:print(", target = ");
                    print_expr(_target@, depth);
                },
            };
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:SubDef(opt<ptr<Expr>> ret_type, ptr<Expr> path, vec<Expr> args, vec<Expr> body) {
            match ret_type {
                opt<ptr<Expr>>:Some(ptr<Expr> _ret_type) {
                    fmt:print("ret_type = ");
                    print_expr(_ret_type@, depth);
                    fmt:print(", ");
                },
            };
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:VarDef(opt<ptr<Expr>> _type, ptr<Expr> path, opt<ptr<Expr>> value) {
            match _type {
                opt<ptr<Expr>>:Some(ptr<Expr> __type) {
                    fmt:print("type = ");
                    print_expr(__type@, depth);
                    fmt:print(", ");
                },
            };
            fmt:print("path = ");
            print_expr(path@, depth);
            match value {
                opt<ptr<Expr>>:Some(ptr<Expr> _value) {
                    fmt:print(", value = ");
                    print_expr(_value@, depth);
                },
            };
        },
        ExprKind:LocalVarDef(opt<ptr<Expr>> _type, str name, opt<ptr<Expr>> value) {
            match _type {
                opt<ptr<Expr>>:Some(ptr<Expr> __type) {
                    fmt:print("type = ");
                    print_expr(__type@, depth);
                    fmt:print(", ");
                },
            };
            fmt:print("name = %s", name);
            match value {
                opt<ptr<Expr>>:Some(ptr<Expr> _value) {
                    fmt:print(", value = ");
                    print_expr(_value@, depth);
                },
            };
        },
        ExprKind:SubCall(ptr<Expr> path, vec<Expr> args) {
            fmt:print("path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
        },
        ExprKind:VarUse(ptr<Expr> path) {
            fmt:print("path = ");
            print_expr(path@, depth);
        },
        ExprKind:CompInst(ptr<Expr> _type, vec<FieldValue> field_vals) {
            fmt:print("type = ");
            print_expr(_type@, depth);
            fmt:print(", field_vals = [");
            for i = 0, i < field_vals.len, i += 1 {
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("%s = ", field_vals[i].name.buf);
                print_expr(field_vals[i].value@, depth);
            };
            fmt:print("]");
        },
        ExprKind:StringLit(str value) {
            fmt:print("%s", value);
        },
        ExprKind:CharLit(str value) {
            fmt:print("%s", value);
        },
        ExprKind:IntLit(str value) {
            fmt:print("%s", value);
        },
        ExprKind:FloatLit(str value) {
            fmt:print("%s", value);
        },
        ExprKind:BoolLit(bool value) {
            if value {
                fmt:print("true");
            } else {
                fmt:print("false");
            };
        },
        ExprKind:ThisLit {},
        ExprKind:TypeLit {},
        ExprKind:LocalBlock(vec<Expr> body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:If(ptr<Expr> _if, vec<Expr> _elif, opt<ptr<Expr>> _else) {
            fmt:print("if = ");
            print_expr(_if@, depth);
            print_opt_expr_vec(_elif, depth, "elif", true);
            match _else {
                opt<ptr<Expr>>:Some(ptr<Expr> __else) {
                    fmt:print(", else = ");
                    print_expr(__else@, depth);
                },
            };
        },
        ExprKind:IfBranch(ptr<Expr> cond, vec<Expr> body) {
            fmt:print("cond = ");
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:ElseBranch(vec<Expr> body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:Loop(vec<Expr> body) {
            print_opt_expr_vec(body, depth, "body", false);
        },
        ExprKind:While(ptr<Expr> cond, vec<Expr> body) {
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:For(str iter, opt<ptr<Expr>> iter_value, ptr<Expr> cond, ptr<Expr> _next, vec<Expr> body) {
            fmt:print("iter = %s", iter);
            match iter_value {
                opt<ptr<Expr>>:Some(ptr<Expr> _iter_value) {
                    fmt:print(", iter_value = ");
                    print_expr(_iter_value@, depth);
                },
            };
            fmt:print(", cond = ");
            print_expr(cond@, depth);
            fmt:print(", next = ");
            print_expr(_next@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Each(vec<str> vars, ptr<Expr> iter, vec<Expr> body) {
            fmt:print("vars = [");
            for i = 0, i < vars.len, i += 1 {
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("%s", vars[i]);
            };
            fmt:print("], iter = ");
            print_expr(iter@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Match(ptr<Expr> value, vec<Expr> cases) {
            fmt:print("value = ");
            print_expr(value@, depth);
            print_opt_expr_vec(cases, depth, "cases", true);
        },
        ExprKind:Case(vec<Expr> values, vec<Expr> body) {
            print_opt_expr_vec(values, depth, "values", false);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Ret(opt<str> label, opt<ptr<Expr>> value) {
            match label {
                opt<str>:Some(str _label) {
                    fmt:print("label = %s", _label.buf);
                },
            };
            match value {
                opt<ptr<Expr>>:Some(ptr<Expr> _value) {
                    fmt:print(", value = ");
                    print_expr(_value@, depth);
                },
            };
        },
        ExprKind:Next(opt<str> label) {
            match label {
                opt<str>:Some(str _label) {
                    fmt:print("label = %s, ", _label.buf);
                },
            };
        },
        ExprKind:Jump(opt<str> label) {
            match label {
                opt<str>:Some(str _label) {
                    fmt:print("label = %s, ", _label.buf);
                },
            };
        },
        ExprKind:Try(opt<str> label, ptr<Expr> value) {
            match label {
                opt<str>:Some(str _label) {
                    fmt:print("label = %s, ", _label.buf);
                },
            };
            fmt:print("value = ");
            print_expr(value@, depth);
        },
        ExprKind:UnaryOp(ptr<Expr> value, TokenKind kind) {
            print_expr(value@, depth);
        },
        ExprKind:BinaryOp(ptr<Expr> lhs, ptr<Expr> rhs, TokenKind kind) {
            fmt:print("lhs = ");
            print_expr(lhs@, depth);
            fmt:print(", rhs = ");
            print_expr(rhs@, depth);
        },
        ExprKind:TernaryOp(ptr<Expr> cond, ptr<Expr> valueA, ptr<Expr> valueB) {
            fmt:print("cond = ");
            print_expr(cond@, depth);
            fmt:print(", valueA = ");
            print_expr(valueA@, depth);
            fmt:print(", valueB = ");
            print_expr(valueB@, depth);
        },
    };
    char:put(')');
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
        ExprKind:CompInst { ret "COMP_INST"; },
        ExprKind:EnumInst { ret "ENUM_INST"; },
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
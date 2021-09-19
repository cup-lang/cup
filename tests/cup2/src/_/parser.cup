comp Path (
    darr<PathPart> parts,
    int index,
);

comp PathPart (
    bool pub,
    str name,
    darr<Path> gens,
);

comp Tag (
    Path path,
    darr<Expr> args,
    int index,
);

comp FieldValue (
    str name,
    ptr<Expr> value,
);

comp IfBranch (
    ptr<Expr> cond,
    darr<Expr> body,
);

comp ElseBranch (
    darr<Expr> body,
);

enum ExprKind (
    Empty,
    TagDef(Path path, darr<Expr> args, darr<Expr> body),
    Block(darr<Expr> body),
    Mod(Path path, darr<Expr> body),
    Use(Path path),
    Field(Path _type, str name),
    Comp(Path path, darr<Expr> fields, darr<Expr> body),
    Enum(Path path, darr<Expr> opts, darr<Expr> body),
    Option(str name, darr<Expr> fields),
    Prop(Path path, darr<Expr> body),
    Def(Path _prop, opt<Path> target, darr<Expr> body),
    SubDef(opt<Path> ret_type, Path path, darr<Expr> args, darr<Expr> body),
    VarDef(opt<Path> _type, Path path, opt<ptr<Expr>> value),

    LocalVarDef(opt<Path> _type, str name, opt<ptr<Expr>> value),
    SubCall(Path path, darr<Expr> args),
    VarUse(Path path),
    CompInst(Path _type, darr<FieldValue> field_vals),
    EnumInst(Path _type, Path path, int opt_index, darr<Expr> args),
    StringLit(str value),
    CharLit(str value),
    IntLit(str value),
    FloatLit(str value),
    BoolLit(bool value),
    ThisLit,
    TypeLit,
    LocalBlock(darr<Expr> body),
    If(IfBranch _if, darr<IfBranch> _elif, opt<ElseBranch> _else),
    Loop(darr<Expr> body),
    While(ptr<Expr> cond, darr<Expr> body),
    For(str iter, opt<ptr<Expr>> iter_value, ptr<Expr> cond, ptr<Expr> _next, darr<Expr> body),
    Each(darr<str> vars, ptr<Expr> iter, darr<Expr> body),
    Match(ptr<Expr> value, darr<Expr> cases),
    Case(darr<Expr> values, darr<Expr> body),
    Echo(ptr<Expr> value),
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
    darr<Tag> tags,
    opt<str> label,
);

darr<Expr> parse(File file, darr<Token> tokens) {
    int index = 0;
    darr<Expr> exprs = darr<Expr>:new_with_cap(16);

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

darr<Expr> parse_block(File file, darr<Token> tokens, ptr<int> index, bool local) {
    darr<Expr> exprs = darr<Expr>:new_with_cap(4);

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

Token expect_token(File file, darr<Token> tokens, ptr<int> index, TokenKind kind, ptr<u8> error) {
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

bool opt_token(darr<Token> tokens, ptr<int> index, TokenKind kind) {
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

Expr parse_local(File file, darr<Token> tokens, ptr<int> index, int op_level, bool opt) {
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
            Path path = parse_path(file, tokens, index);

            if opt_token(tokens, index, TokenKind:LeftParen) {
                darr<Expr> args = darr<Expr>:new_with_cap(4);
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
                expr.kind = ExprKind:LocalVarDef(opt<Path>:Some(path), name, value);
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
            expr.kind = ExprKind:LocalVarDef(opt<Path>:None, name, value);
        },
        TokenKind:New {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'new' path");
            darr<FieldValue> field_vals = darr<FieldValue>:new_with_cap(4);
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
            darr<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:LocalBlock(body);
        },
        TokenKind:If {
            index@ += 1;
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'if' condition");
            darr<Expr> body = parse_block(file, tokens, index, true);
            IfBranch _if = new IfBranch {
                cond = cond,
                body = body,
            };
            darr<IfBranch> _elif = darr<IfBranch>:new_with_cap(1);
            while opt_token(tokens, index, TokenKind:Elif) {
                cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'elif' condition");
                body = parse_block(file, tokens, index, true);
                _elif.push(new IfBranch {
                    cond = cond,
                    body = body,
                });
            };
            opt<ElseBranch> _else = opt<ElseBranch>:None;
            if opt_token(tokens, index, TokenKind:Else) {
                expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'else' keyword");
                darr<Expr> body = parse_block(file, tokens, index, true);
                _else = opt<ElseBranch>:Some(new ElseBranch { body = body });
            };
            expr.kind = ExprKind:If(_if, _elif, _else);
        },
        TokenKind:Loop {
            index@ += 1;
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'loop' keyword");
            darr<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Loop(body);
        },
        TokenKind:While {
            index@ += 1;
            ptr<Expr> cond = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'while' condition");
            darr<Expr> body = parse_block(file, tokens, index, true);
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
            darr<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:For(iter, iter_value, cond, _next, body);
        },
        TokenKind:Each {
            index@ += 1;
            darr<str> vars = darr<str>:new_with_cap(2);
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
            darr<Expr> body = parse_block(file, tokens, index, true);
            expr.kind = ExprKind:Each(vars, iter, body);
        },
        TokenKind:Match {
            index@ += 1;
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expect_token(file, tokens, index, TokenKind:LeftBrace, "expected '{' after 'match' value");
            darr<Expr> body = darr<Expr>:new_with_cap(4);
            ~lll loop {
                if opt_token(tokens, index, TokenKind:RightBrace) {
                    expr.kind = ExprKind:Match(value, body);
                    ret ~l;
                };
                darr<Expr> values = darr<Expr>:new_with_cap(2);
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
                darr<Expr> case_body = parse_block(file, tokens, index, true);
                body.push(new Expr {
                    kind = ExprKind:Case(values, case_body),
                    tags = new darr<Tag> { len = 0 },
                    label = opt<str>:None,
                });
                if opt_token(tokens, index, TokenKind:Comma) == false {
                    ret ~lll;
                };
            };
            expect_token(file, tokens, index, TokenKind:RightBrace, "expected '}' after 'match' body");
            expr.kind = ExprKind:Match(value, body);
        },
        TokenKind:Echo {
            index@ += 1;
            ptr<Expr> value = alloc<Expr>(parse_local(file, tokens, index, 0, false));
            expr.kind = ExprKind:Echo(value);
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
                    kind = ExprKind:TernaryOp(alloc<Expr>(expr), alloc<Expr>(valueA), alloc<Expr>(valueB)),
                    tags = new darr<Tag> { len = 0 },
                    label = opt<str>:None,
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
                    kind = ExprKind:UnaryOp(alloc<Expr>(expr), token.kind),
                    tags = new darr<Tag> { len = 0 },
                    label = opt<str>:None,
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

Expr parse_global(File file, darr<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.label = parse_label(file, tokens, index, true);
    expr.tags = parse_tags(file, tokens, index);

    Token token = tokens[index@];

    match token.kind {
        TokenKind:Tag {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'tag' path");
            darr<Expr> args = parse_fields(file, tokens, index);
            darr<Expr> body;
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
            Path path = parse_path(file, tokens, index);
            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Mod(path, body);
        },
        TokenKind:Use {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            expr.kind = ExprKind:Use(path);
        },
        TokenKind:Comp {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'comp' path");
            darr<Expr> fields = parse_fields(file, tokens, index);
            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Comp(path, fields, body);
        },
        TokenKind:Enum {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'enum' path");
            darr<Expr> opts = parse_options(file, tokens, index);
            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Enum(path, opts, body);
        },
        TokenKind:Prop {
            index@ += 1;
            Path path = parse_path(file, tokens, index);
            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Prop(path, body);
        },
        TokenKind:Def {
            index@ += 1;
            Path _prop = parse_path(file, tokens, index);
            opt<Path> target = opt<Path>:None;
            if opt_token(tokens, index, TokenKind:Comma) {
                target = opt<Path>:Some(parse_path(file, tokens, index));
            };
            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, false);
            } else {
                body.len = 0;
            };
            expr.kind = ExprKind:Def(_prop, target, body);
        },
        TokenKind:Sub {
            index@ += 1;

            Path path = parse_path(file, tokens, index);

            expect_token(file, tokens, index, TokenKind:LeftParen, "expected '(' after 'sub' path");
            darr<Expr> args = parse_fields(file, tokens, index);

            darr<Expr> body;
            if opt_token(tokens, index, TokenKind:LeftBrace) {
                body = parse_block(file, tokens, index, true);
            } else {
                body.len = 0;
            };

            expr.kind = ExprKind:SubDef(opt<Path>:None, path, args, body);
        },
        TokenKind:Var {
            index@ += 1;

            Path path = parse_path(file, tokens, index);

            opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
            if opt_token(tokens, index, TokenKind:Assign) {
                value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
            };
            expr.kind = ExprKind:VarDef(opt<Path>:None, path, value);
        },
        TokenKind:Ident {
            Path _type = parse_path(file, tokens, index);

            Path path = parse_path(file, tokens, index);

            token = tokens[index@];
            match token.kind {
                TokenKind:LeftParen {
                    index@ += 1;

                    darr<Expr> args = parse_fields(file, tokens, index);

                    darr<Expr> body;
                    if opt_token(tokens, index, TokenKind:LeftBrace) {
                        body = parse_block(file, tokens, index, true);
                    } else {
                        body.len = 0;
                    };

                    expr.kind = ExprKind:SubDef(opt<Path>:Some(_type), path, args, body);
                },
                _ {
                    opt<ptr<Expr>> value = opt<ptr<Expr>>:None;
                    if opt_token(tokens, index, TokenKind:Assign) {
                        value = opt<ptr<Expr>>:Some(alloc<Expr>(parse_local(file, tokens, index, 0, false)));
                    };
                    expr.kind = ExprKind:VarDef(opt<Path>:Some(_type), path, value);
                },
            };
        },
        _ {
            expr = parse_local(file, tokens, index, 0, true);
            if expr.kind == ExprKind:Empty {
                file.throw(token.index, "expected a expression");
            };
        },
    };

    ret expr;
};

opt<str> parse_label(File file, darr<Token> tokens, ptr<int> index, bool opt) {
    if opt & (opt_token(tokens, index, TokenKind:Tilde) == false) {
        ret opt<str>:None;
    } elif opt == false {
        expect_token(file, tokens, index, TokenKind:Tilde, "expected a label");
    };
    ret opt<str>:Some(expect_token(file, tokens, index, TokenKind:Ident, "expected identifier after label declaration").value);
};

darr<Tag> parse_tags(File file, darr<Token> tokens, ptr<int> index) {
    darr<Tag> tags = darr<Tag>:new_with_cap(2);

    ~l loop {
        match tokens[index@].kind {
            TokenKind:Hash {
                int tag_start = tokens[index@].index;
                Token token = tokens[index@ += 1];
                match token.kind {
                    TokenKind:Ident {
                        Path path = parse_path(file, tokens, index);
                        darr<Expr> args = darr<Expr>:new_with_cap(2);

                        token = tokens[index@];
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
                        tags.push(new Tag {
                            path = path,
                            args = args,
                            index = tag_start,
                        });
                        next ~l;
                    },
                    _ {
                        file.throw(token.index, "expected a path after tag declaration");
                    },
                };
            },
            _ { ret ~l; },
        };
    };

    ret tags;
};

Path parse_path(File file, darr<Token> tokens, ptr<int> index) {
    match tokens[index@].kind {
        TokenKind:Ident {
            ret parse_opt_path(file, tokens, index);
        },
        _ {
            file.throw(tokens[index@].index, "expected a path");
        },
    };
};

Path parse_opt_path(File file, darr<Token> tokens, ptr<int> index) {
    int start_index = index@;
    darr<PathPart> path = darr<PathPart>:new_with_cap(2);
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
                        gens = darr<Path>:new_with_cap(2),
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
                                Path gen = parse_opt_path(file, tokens, index);
                                if gen.parts.len == 0 {
                                    index@ = old_index;
                                    part.gens.len = 0;
                                    ret ~ll;
                                };
                                darr<Path>:push(part.gens$, gen);
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
    ret new Path {
        parts = path,
        index = tokens[start_index].index,
    };
};

darr<Expr> parse_fields(File file, darr<Token> tokens, ptr<int> index) {
    darr<Expr> fields = darr<Expr>:new_with_cap(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret fields;
        };

        Expr field = new Expr {
            kind = ExprKind:Field(
                parse_path(file, tokens, index),
                expect_token(file, tokens, index, TokenKind:Ident, "expected field name in 'comp' definition").value,
            ),
            tags = new darr<Tag> { len = 0 },
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

darr<Expr> parse_options(File file, darr<Token> tokens, ptr<int> index) {
    darr<Expr> opts = darr<Expr>:new_with_cap(2);

    ~l loop {
        if opt_token(tokens, index, TokenKind:RightParen) {
            ret opts;
        };

        str name = expect_token(file, tokens, index, TokenKind:Ident, "expected option name in 'enum' definition").value;

        darr<Expr> fields;
        if opt_token(tokens, index, TokenKind:LeftParen) {
            fields = parse_fields(file, tokens, index);    
        } else {
            fields.len = 0;
        };

        opts.push(new Expr {
            kind = ExprKind:Option(name, fields),
            tags = new darr<Tag> { len = 0 },
            label = opt<str>:None,
        });

        if opt_token(tokens, index, TokenKind:Comma) == false {
            ret ~l;
        };
    };
    expect_token(file, tokens, index, TokenKind:RightParen, "expected ')' after last option");

    ret opts;
};

sub print_all_exprs(darr<Expr> exprs) {
    fmt:print("Exprs:");
    print_exprs(exprs, 0);
    char:put('\n');
};

sub print_exprs(darr<Expr> exprs, int depth) {
    for i = 0, i < exprs.len, i += 1 {
        char:put('\n');
        indent(depth);
        print_expr(exprs[i], depth);
    };
    char:put('\n');
};

sub print_opt_exprs(darr<Expr> exprs, int depth, ptr<u8> name, bool _next) {
    if exprs.len > 0 {
        if _next {
            fmt:print(", ");
        };
        fmt:print("%s = [", name);
        print_exprs(exprs, depth + 1);
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
    print_tags(expr.tags, depth);
    if expr.tags.len != 0 {
        fmt:print(", ");
    };
    match expr.kind {
        ExprKind:TagDef(Path path, darr<Expr> args, darr<Expr> body) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(args, depth, "args", true);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Block(darr<Expr> body) {
            print_opt_exprs(body, depth, "body", false);
        },
        ExprKind:Mod(Path path, darr<Expr> body) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(body, depth, "body", false);
        },
        ExprKind:Use(Path path) {
            fmt:print("path = ");
            print_path(path, depth);
        },
        ExprKind:Field(Path _type, str name) {
            fmt:print("type = ");
            print_path(_type, depth);
            fmt:print(", name = %s", name.buf);
        },
        ExprKind:Comp(Path path, darr<Expr> fields, darr<Expr> body) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(fields, depth, "fields", true);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Enum(Path path, darr<Expr> opts, darr<Expr> body) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(opts, depth, "opts", true);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Option(str name, darr<Expr> fields) {
            fmt:print("name = %s", name.buf);
            print_opt_exprs(fields, depth, "fields", true);
        },
        ExprKind:Prop(Path path, darr<Expr> body) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Def(Path _prop, opt<Path> target, darr<Expr> body) {
            fmt:print("prop = ");
            print_path(_prop, depth);
            match target {
                opt<Path>:Some(Path _target) {
                    fmt:print(", target = ");
                    print_path(_target, depth);
                },
            };
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:SubDef(opt<Path> ret_type, Path path, darr<Expr> args, darr<Expr> body) {
            match ret_type {
                opt<Path>:Some(Path _ret_type) {
                    fmt:print("ret_type = ");
                    print_path(_ret_type, depth);
                    fmt:print(", ");
                },
            };
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(args, depth, "args", true);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:VarDef(opt<Path> _type, Path path, opt<ptr<Expr>> value) {
            match _type {
                opt<Path>:Some(Path __type) {
                    fmt:print("type = ");
                    print_path(__type, depth);
                    fmt:print(", ");
                },
            };
            fmt:print("path = ");
            print_path(path, depth);
            match value {
                opt<ptr<Expr>>:Some(ptr<Expr> _value) {
                    fmt:print(", value = ");
                    print_expr(_value@, depth);
                },
            };
        },
        ExprKind:LocalVarDef(opt<Path> _type, str name, opt<ptr<Expr>> value) {
            match _type {
                opt<Path>:Some(Path __type) {
                    fmt:print("type = ");
                    print_path(__type, depth);
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
        ExprKind:SubCall(Path path, darr<Expr> args) {
            fmt:print("path = ");
            print_path(path, depth);
            print_opt_exprs(args, depth, "args", true);
        },
        ExprKind:VarUse(Path path) {
            fmt:print("path = ");
            print_path(path, depth);
        },
        ExprKind:CompInst(Path _type, darr<FieldValue> field_vals) {
            fmt:print("type = ");
            print_path(_type, depth);
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
        ExprKind:LocalBlock(darr<Expr> body) {
            print_opt_exprs(body, depth, "body", false);
        },
        ExprKind:If(IfBranch _if, darr<IfBranch> _elif, opt<ElseBranch> _else) {
            fmt:print("if = ");
            print_if_branch(_if, depth);
            print_if_branch_darr(_elif, depth);
            match _else {
                opt<ElseBranch>:Some(ElseBranch __else) {
                    fmt:print(", else = ");
                    print_else_branch(__else, depth);
                },
            };
        },
        ExprKind:Loop(darr<Expr> body) {
            print_opt_exprs(body, depth, "body", false);
        },
        ExprKind:While(ptr<Expr> cond, darr<Expr> body) {
            print_expr(cond@, depth);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:For(str iter, opt<ptr<Expr>> iter_value, ptr<Expr> cond, ptr<Expr> _next, darr<Expr> body) {
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
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Each(darr<str> vars, ptr<Expr> iter, darr<Expr> body) {
            fmt:print("vars = [");
            for i = 0, i < vars.len, i += 1 {
                if i != 0 {
                    fmt:print(", ");
                };
                fmt:print("%s", vars[i]);
            };
            fmt:print("], iter = ");
            print_expr(iter@, depth);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Match(ptr<Expr> value, darr<Expr> cases) {
            fmt:print("value = ");
            print_expr(value@, depth);
            print_opt_exprs(cases, depth, "cases", true);
        },
        ExprKind:Case(darr<Expr> values, darr<Expr> body) {
            print_opt_exprs(values, depth, "values", false);
            print_opt_exprs(body, depth, "body", true);
        },
        ExprKind:Echo(ptr<Expr> value) {
            fmt:print("value = ");
            print_expr(value@, depth);
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

sub print_path(Path path, int depth) {
    color:set(Color:Green);
    fmt:print("PATH");
    color:reset();
    char:put('(');
    for i = 0, i < path.parts.len, i += 1 {
        PathPart part = path.parts[i];
        if i != 0 {
            fmt:print(", ");
        };
        fmt:print("name = %s", part.name.buf);
        if part.gens.len > 0 {
            fmt:print(", gens = [");
            for ii = 0, ii < part.gens.len, ii += 1 {
                char:put('\n');
                indent(depth + 1);
                print_path(part.gens[ii], depth + 1);
            };
            char:put('\n');
            indent(depth);
            char:put(']');
        };
    };
    char:put(')');
};

sub print_tags(darr<Tag> tags, int depth) {
    if tags.len > 0 {
        fmt:print("tags = [");
        for i = 0, i < tags.len, i += 1 {
            char:put('\n');
            indent(depth + 1);
            color:set(Color:Green);
            fmt:print("TAG");
            color:reset();
            fmt:print("(path = ");
            print_path(tags[i].path, depth + 1);
            print_opt_exprs(tags[i].args, depth + 1, "args", true);
            char:put(')');
        };
        char:put('\n');
        indent(depth);
        char:put(']');
    };
};

sub print_if_branch(IfBranch _if, int depth) {
    color:set(Color:Green);
    fmt:print("IF_BRANCH");
    color:reset();
    fmt:print("(cond = ");
    print_expr(_if.cond@, depth);
    print_opt_exprs(_if.body, depth, "body", true);
};

sub print_if_branch_darr(darr<IfBranch> ifs, int depth) {
    if ifs.len > 0 {
        fmt:print(", elif = [");
        for i = 0, i < ifs.len, i += 1 {
            char:put('\n');
            indent(depth + 1);
            print_if_branch(ifs[i], depth + 1);
        };
        char:put('\n');
        indent(depth);
        char:put(']');
    };
};

sub print_else_branch(ElseBranch _else, int depth) {
    color:set(Color:Green);
    fmt:print("ELSE_BRANCH");
    color:reset();
    char:put('(');
    print_opt_exprs(_else.body, depth, "body", false);
    char:put(')');
};

ptr<u8> get_expr_name(ExprKind kind) {
    match kind {
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
        ExprKind:Loop { ret "LOOP"; },
        ExprKind:While { ret "WHILE"; },
        ExprKind:For { ret "FOR"; },
        ExprKind:Each { ret "EACH"; },
        ExprKind:Match { ret "MATCH"; },
        ExprKind:Case { ret "CASE"; },
        ExprKind:Echo { ret "ECHO"; },
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
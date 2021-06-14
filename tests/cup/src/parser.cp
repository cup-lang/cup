sub REMOVE_ME() { vec<PathPart>:new(0); vec<FieldInst>:new(0); };

comp PathPart {
    ptr<u8> name,
    vec<Expr> gens,
};

comp FieldInst {
    ptr<u8> name,
    ptr<Expr> value,
};

enum ExprKind {
    Tag(ptr<u8> name, vec<Expr> args),
    Path(vec<PathPart> path),
    TagDef(ptr<Expr> path, vec<Expr> body),
    Block(vec<Expr> body),
    Mod(ptr<Expr> path),
    Use(ptr<Expr> path),
    Field(ptr<Expr> _type, ptr<u8> name),
    Comp(ptr<Expr> path, vec<Expr> fields, vec<Expr> body),
    Enum(ptr<Expr> path, vec<Expr> fields, vec<Expr> opts, vec<Expr> body),
    Option(ptr<u8> name, vec<Expr> fields),
    Prop(ptr<Expr> path, vec<Expr> body),
    Def(ptr<Expr> _prop, ptr<Expr> target),
    SubDef(ptr<Expr> ret_type, ptr<Expr> path, vec<Expr> args, vec<Expr> body),
    VarDef(ptr<Expr> _type, ptr<Expr> path, ptr<Expr> value),

    LocalVarDef(ptr<Expr> _type, ptr<u8> name, ptr<Expr> value),
    SubCall(ptr<Expr> path, vec<Expr> args),
    VarUse(ptr<Expr> path),
    CompInst(ptr<Expr> path, vec<FieldInst> fields),
    StringLit(ptr<u8> value),
    CharLit(ptr<u8> value),
    IntLit(ptr<u8> value),
    FloatLit(ptr<u8> value),
    BoolLit(bool value),
    ThisLit,
    TypeLit,
    LocalBlock(vec<Expr> body),
    If(ptr<Expr> cond, vec<Expr> body),
    Elif(ptr<Expr> cond, vec<Expr> body),
    Else(vec<Expr> body),
    Loop(vec<Expr> body),
    While(ptr<Expr> cond, vec<Expr> body),
    For(vec<Expr> iter, ptr<Expr> cond, vec<Expr> _next, vec<Expr> body),
    Each(ptr<u8> iter, ptr<Expr> value, vec<Expr> body),
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
    ret parse_block(file, tokens, index$, false);
};

vec<Expr> parse_block(File file, vec<Token> tokens, ptr<int> index, bool local) {
    vec<Expr> exprs = vec<Expr>:new(4);

    while index@ < tokens.len {
        match tokens.buf[index@].kind {
            TokenKind:RightBrace {
                index@ += 1;
                ret exprs;
            },
            TokenKind:Empty {
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
    };

    ret exprs;
};

bool opt_token(vec<Token> tokens, TokenKind kind, ptr<int> index) {
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

sub expect_token(File file, vec<Token> tokens, TokenKind kind, ptr<int> index, ptr<u8> error) {
    Token token = tokens.buf[index@];
    match token.kind {
        TokenKind:Unset {},
        kind {},
        _ {
            throw(file, token.index, error);
        },
    };
    index@ += 1;
};

Expr parse_local(File file, vec<Token> tokens, ptr<int> index) {
    ret Expr {};
};

Expr parse_global(File file, vec<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.tags.len = 0;

    Token token = tokens.buf[index@];

    match token.kind {
        TokenKind:Mod {
            index@ += 1;
            ptr<Expr> path = mem:alloc(mem:size<Expr>());
            path@ = parse_path(file, tokens, index);
            expr.kind = ExprKind:Mod(path);
        },
        TokenKind:Use {
            index@ += 1;
            ptr<Expr> path = mem:alloc(mem:size<Expr>());
            path@ = parse_path(file, tokens, index);
            expr.kind = ExprKind:Use(path);
        },
        TokenKind:Comp {
            index@ += 1;
            ptr<Expr> path = mem:alloc(mem:size<Expr>());
            path@ = parse_path(file, tokens, index);
            expr.kind = ExprKind:Comp(path);
            expect_token(file, tokens, TokenKind:LeftParen, index, "expected '(' after 'comp' path");
            vec<Expr> fields = parse_fields(file, tokens, index);
            expect_token(file, tokens, TokenKind:LeftParen, index, "expected ')' after last field");
            ` fields
            ` body
        },
        _ {
            throw(file, token.index, "expected a global item");
        },
    };

    ret expr;
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
                                bool a = opt_token(tokens, TokenKind:Comma, index);
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

    loop {
        if opt_token(tokens, TokenKind:RightBrace, index) {

        };

        ` let should_end;
        ` token = optionalToken(tokenKind.RIGHT_BRACE, () => {
        `     token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'comp' body");
        `     should_end = true;
        ` });
        ` if (should_end) { break end; }
        ` let field = {
        `     kind: exprKind.FIELD,
        `     tags: parseTags()
        ` };
        ` field.type = parseType();
        ` token = expectToken(tokenKind.IDENT, "expected field name in 'comp' body", () => {
        `     field.name = tokens[index].value;
        ` });
        ` expr.body.push(field);
        ` let should_break;
        ` token = optionalToken(tokenKind.COMMA, null, () => {
        `     should_break = true;
        ` });
        ` if (should_break) { break; }
    };

    ret fields;
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
        set_color(Color:Green);
        printf("%s", get_expr_name(exprs.buf[i].kind));
        set_color(Color:Reset);
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
    putchar('(');
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
        ExprKind:TagDef(path, body) {
            print_expr(path@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Block(body) {
            print_opt_expr_vec(body, depth, "body", true);
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
            print_expr(_type@, depth);
            fmt:print("name = %s", name);
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
        ExprKind:Def(_prop, target) {
            fmt:print("prop = ");
            print_expr(_prop@, depth);
            fmt:print(", target = ");
            print_expr(target@, depth);
        },
        ExprKind:SubDef(ret_type, path, args, body) {
            fmt:print("ret_type = ");
            print_expr(ret_type@, depth);
            fmt:print(", path = ");
            print_expr(path@, depth);
            print_opt_expr_vec(args, depth, "args", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:VarDef(_type, path, value) {
            fmt:print("type = ");
            print_expr(_type@, depth);
            fmt:print(", path = ");
            print_expr(path@, depth);
            fmt:print(", value = ");
            print_expr(value@, depth);
        },
        ExprKind:LocalVarDef(_type, name, value) {
            fmt:print("type = ");
            print_expr(_type@, depth);
            fmt:print(", name = %s, value = ", name);
            print_expr(value@, depth);
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
        ExprKind:CompInst(path, body) {
            fmt:print("path = ");
            print_expr(path@, depth);
            for i = 0, (i) < body.len, i += 1 {
                FieldInst field = body.buf[i];
                fmt:print("name = %s", field.name);
                print_expr(field.value@, depth);
            };
        },
        ExprKind:StringLit(value) {
            fmt:print("name = %s", value);
        },
        ExprKind:CharLit(value) {
            fmt:print("name = %s", value);
        },
        ExprKind:IntLit(value) {
            fmt:print("name = %s", value);
        },
        ExprKind:FloatLit(value) {
            fmt:print("name = %s", value);
        },
        ExprKind:BoolLit(value) {
            if value {
                fmt:print("value = true");
            } else {
                fmt:print("value = false");
            };
        },
        ExprKind:ThisLit {},
        ExprKind:TypeLit {},
        ExprKind:LocalBlock(body) {
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:If(cond, body) {
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Elif(cond, body) {
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Else(body) {
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Loop(body) {
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:While(cond, body) {
            print_expr(cond@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:For(iter, cond, _next, body) {
            print_opt_expr_vec(iter, depth, "iter", true);
            print_expr(cond@, depth);
            print_opt_expr_vec(_next, depth, "next", true);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Each(iter, value, body) {
            fmt:print("iter = %s", iter);
            print_expr(value@, depth);
            print_opt_expr_vec(body, depth, "body", true);
        },
        ExprKind:Match(value, cases) {
            ` value
            ` cases
        },
        ExprKind:Case(values, body) {
            ` values
            ` body
        },
        ExprKind:Ret(label, value) {
            fmt:print("label = %s", label);
            print_expr(value@, depth);
        },
        ExprKind:Next(label) {
            fmt:print("label = %s", label);
        },
        ExprKind:Jump(label) {
            fmt:print("label = %s", label);
        },
        ExprKind:Try(label, value) {
            fmt:print("label = %s", label);
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
        ExprKind:Mod { ret "MOD"; },
        ExprKind:Use { ret "USE"; },
        ExprKind:Comp { ret "COMP"; },
        ExprKind:Enum { ret "ENUM"; },
        ExprKind:Prop { ret "PROP"; },
        ExprKind:Def { ret "DEF"; },
        ExprKind:SubDef { ret "SUB_DEF"; },
        ExprKind:VarDef { ret "VAR_DEF"; },
        ExprKind:LocalVarDef { ret "LOCAL_VAR_DEF"; },
        ExprKind:SubCall { ret "SUB_CALL"; },
        ExprKind:VarUse { ret "VAR_USE"; },
        ExprKind:CompInst { ret "COMP_INST"; },
        ExprKind:StringLit { ret "STRING_LIT"; },
        ExprKind:CharLit { ret "CHAR_LIT"; },
        ExprKind:IntLit { ret "INT_LIT"; },
        ExprKind:FloatLit { ret "FLOAT_LIT"; },
        ExprKind:BoolLit { ret "BOOL_LIT"; },
        ExprKind:ThisLit { ret "THIS_LIT"; },
        ExprKind:TypeLit { ret "TYPE_LIT"; },
        ExprKind:LocalBlock { ret "LOCAL_BLOCK"; },
        ExprKind:If { ret "IF"; },
        ExprKind:Elif { ret "ELIF"; },
        ExprKind:Else { ret "ELSE"; },
        ExprKind:Loop { ret "LOOP"; },
        ExprKind:While { ret "WHILE"; },
        ExprKind:For { ret "FOR"; },
        ExprKind:Each { ret "EACH"; },
        ExprKind:Match { ret "MATCH"; },
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
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

Expr parse_local(File file, vec<Token> tokens, ptr<int> index) {
    ret Expr {};
};

Expr parse_global(File file, vec<Token> tokens, ptr<int> index) {
    Expr expr;
    expr.tags.len = 0;

    Token token = tokens.buf[index@];

    match token.kind {
        TokenKind:Use {
            token = tokens.buf[index@ += 1];
            match token.kind {
                TokenKind:Ident {
                    ` ptr<Expr> path = parse_path(file, tokens, index);
                    ` expr.kind = ExprKind:Use(path);
                },
                _ {
                    throw(file, token.index, "expected identifier");
                },
            };
        },
    };

    ret expr;
};

ptr<Expr> parse_path(File file, vec<Token> tokens, ptr<int> index) {
    ptr<Expr> expr = mem:alloc(mem:size<Expr>());
    
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
                            index@ += 1;
                            ` nextToken();
                            loop {
                                ` let should_break;
                                ` optionalToken(tokenKind.GREATER, () => {
                                `     should_break = true;
                                ` }, null);
                                ` if (should_break) { break; }
                                ` part.gens.push(parseType());
                                ` optionalToken(tokenKind.COMMA, null, null);
                            };
                            index@ -= 1;
                        },
                    };
                    path.push(part);
                    need_colon = true;
                },
                _ {
                    if path.len > 0 {
                        throw(file, index@, "expected identifier after ':'");
                    } else {
                        ret ~l;
                    };
                },
            };
        };
        index@ += 1;
    };

    expr@.kind = ExprKind:Path(path);
    ret expr;
};

sub print_exprs(vec<Expr> exprs) {
    fmt:print("Exprs:");
    print_expr_vec(exprs, 0, none);
    putchar('\n');
};

sub print_expr_vec(vec<Expr> exprs, int depth, ptr<u8> name) {
    for i = 0, (i) < exprs.len, i += 1 {
        putchar('\n');
        if name != none {
            fmt:print("%s = [", name);
        };
        indent(depth);
        set_color(Color:Green);
        fmt:print("%s", get_expr_name(exprs.buf[i].kind));
        set_color(Color:Reset);
        print_expr(exprs.buf[i], depth);
        if name != none {
            indent(depth);
            putchar(']');
        };
    };
    if exprs.len != 0 {
        putchar('\n');
    };
};

sub print_expr(Expr expr, int depth) {
    putchar('(');
    print_expr_vec(expr.tags, 0, "tags");
    if expr.tags.len != 0 {
        fmt:print(", ");
    };
    match expr.kind {
        ExprKind:Tag(name, args) {
            fmt:print("name = %s", name);
            print_expr_vec(args, depth, "args");
        },
        ExprKind:Path(path) {
            for i = 0, (i) < path.len, i += 1 {
                PathPart part = path.buf[i];
                fmt:print("name = %s", part.name);
                print_expr_vec(part.gens, depth, "gens");
            };
        },
        ExprKind:TagDef(path, body) {
            print_expr(path@, depth);
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Block(body) {
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Mod(path) {
            print_expr(path@, depth);
        },
        ExprKind:Use(path) {
            print_expr(path@, depth);
        },
        ExprKind:Field(_type, name) {
            print_expr(_type@, depth);
            fmt:print("name = %s", name);
        },
        ExprKind:Comp(path, fields, body) {
            print_expr(path@, depth);
            print_expr_vec(fields, depth, "fields");
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Enum(path, fields, opts, body) {
            print_expr(path@, depth);
            print_expr_vec(fields, depth, "fields");
            print_expr_vec(opts, depth, "opts");
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Option(name, fields) {
            fmt:print("name = %s", name);
            print_expr_vec(fields, depth, "fields");
        },
        ExprKind:Prop(path, body) {
            print_expr(path@, depth);
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Def(_prop, target) {
            print_expr(_prop@, depth);
            print_expr(target@, depth);
        },
        ExprKind:SubDef(ret_type, path, args, body) {
            print_expr(ret_type@, depth);
            print_expr(path@, depth);
            print_expr_vec(args, depth, "args");
            print_expr_vec(body, depth, "body");
        },
        ExprKind:VarDef(_type, path, value) {
            print_expr(_type@, depth);
            print_expr(path@, depth);
            print_expr(value@, depth);
        },
        ExprKind:LocalVarDef(_type, name, value) {
            print_expr(_type@, depth);
            fmt:print("name = %s", name);
            print_expr(value@, depth);
        },
        ExprKind:SubCall(path, args) {
            print_expr(path@, depth);
            print_expr_vec(args, depth, "args");
        },
        ExprKind:VarUse(path) {
            print_expr(path@, depth);
        },
        ExprKind:CompInst(path, body) {
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
            print_expr_vec(body, depth, "body");
        },
        ExprKind:If(cond, body) {
            print_expr(cond@, depth);
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Elif(cond, body) {
            print_expr(cond@, depth);
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Else(body) {
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Loop(body) {
            print_expr_vec(body, depth, "body");
        },
        ExprKind:While(cond, body) {
            print_expr(cond@, depth);
            print_expr_vec(body, depth, "body");
        },
        ExprKind:For(iter, cond, _next, body) {
            print_expr_vec(iter, depth, "iter");
            print_expr(cond@, depth);
            print_expr_vec(_next, depth, "next");
            print_expr_vec(body, depth, "body");
        },
        ExprKind:Each(iter, value, body) {
            fmt:print("iter = %s", iter);
            print_expr(value@, depth);
            print_expr_vec(body, depth, "body");
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
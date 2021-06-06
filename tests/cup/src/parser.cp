enum ExprKind {
    Tag(ptr<u8> name, vec<Expr> args),
    Type,
    TagDef,
    Block,
    Mod(ptr<u8> name),
    Use(ptr<u8> name),
    Comp(ptr<u8> name, vec<Expr> body),
    CompField(ptr<Expr> _type, ptr<u8> name),
    Enum(ptr<u8> name, vec<Expr> body),
    Option(ptr<u8> name, vec<Expr> args),
    OptionField(ptr<Expr> _type, ptr<u8> name),
    Prop(ptr<u8> name, vec<Expr> body),
    Def,
    SubDef(ptr<Expr> _type, ptr<u8> name, vec<Expr> body),
    Arg(ptr<Expr> _type, ptr<u8> name),
    VarDef(ptr<Expr> _type, ptr<u8> name, ptr<Expr> value),

    LocalVarDef(ptr<Expr> _type, ptr<u8> name, ptr<Expr> value),
    SubCall,
    VarUse,
    CompInst,
    FieldVal,
    StringLit,
    CharLit,
    ArrayLit,
    IntLit,
    FloatLit,
    BoolLit,
    NoneLit,
    ThisLit,
    TypeLit,
    LocalBlock,
    If,
    Elif,
    Else,
    Loop,
    While,
    For,
    Each,
    Match,
    Cases,
    Case,
    Ret,
    Next,
    Jump,
    Try,

    UnaryOp(ptr<Expr> value, TokenKind kind),
    BinaryOp(ptr<Expr> lhs, ptr<Expr> rhs, TokenKind kind),
    TernaryOp,
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
                    expr.kind = ExprKind:Use(token.value);
                    token = tokens.buf[index@ += 1];
                },
                _ {
                    throw(file, token.index, "expected identifier");
                },
            };
        },
    };

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
        ExprKind:Type {},
        ExprKind:TagDef {},
        ExprKind:Block {},
        ExprKind:Mod {},
        ExprKind:Use(name) {
            fmt:print("name = %s", name);
        },
        ExprKind:Comp {},
        ExprKind:CompField {},
        ExprKind:Enum {},
        ExprKind:Option {},
        ExprKind:OptionField {},
        ExprKind:Prop {},
        ExprKind:Def {},
        ExprKind:SubDef {},
        ExprKind:Arg {},
        ExprKind:VarDef {},
        ExprKind:LocalVarDef {},
        ExprKind:SubCall {},
        ExprKind:VarUse {},
        ExprKind:CompInst {},
        ExprKind:FieldVal {},
        ExprKind:StringLit {},
        ExprKind:CharLit {},
        ExprKind:ArrayLit {},
        ExprKind:IntLit {},
        ExprKind:FloatLit {},
        ExprKind:BoolLit {},
        ExprKind:NoneLit {},
        ExprKind:ThisLit {},
        ExprKind:TypeLit {},
        ExprKind:LocalBlock {},
        ExprKind:If {},
        ExprKind:Elif {},
        ExprKind:Else {},
        ExprKind:Loop {},
        ExprKind:While {},
        ExprKind:For {},
        ExprKind:Each {},
        ExprKind:Match {},
        ExprKind:Cases {},
        ExprKind:Case {},
        ExprKind:Ret {},
        ExprKind:Next {},
        ExprKind:Jump {},
        ExprKind:Try {},
        ExprKind:UnaryOp {},
        ExprKind:BinaryOp {},
        ExprKind:TernaryOp {},
    };
    putchar(')');
};

ptr<u8> get_expr_name(ExprKind kind) {
    match kind {
        ExprKind:Tag { ret "TAG"; },
        ExprKind:Type { ret "TYPE"; },
        ExprKind:ConstrType { ret "CONSTR_TYPE"; },
        ExprKind:TagDef { ret "TAG_DEF"; },
        ExprKind:Mod { ret "MOD"; },
        ExprKind:Use { ret "USE"; },
        ExprKind:Comp { ret "COMP"; },
        ExprKind:Field { ret "FIELD"; },
        ExprKind:Enum { ret "ENUM"; },
        ExprKind:Option { ret "OPTION"; },
        ExprKind:OptionField { ret "OPTION_FIELD"; },
        ExprKind:Prop { ret "PROP"; },
        ExprKind:Def { ret "DEF"; },
        ExprKind:SubDef { ret "SUB_DEF"; },
        ExprKind:Arg { ret "ARG"; },
        ExprKind:VarDef { ret "VAR_DEF"; },
    };
};

sub indent(int depth) {
    for i = 0, i <= depth, i += 1 {
        fmt:print("  ");
    };
};
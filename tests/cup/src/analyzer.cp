sub REMOVE_ME2() { vec<Expr>:new(0); vec<GenericType>:new(0); };

vec<GenericType> gens;

comp GenericType {
    int index,
    vec<Expr> _type,
};

sub analyze(vec<Expr> ast) {
    analyze_expr_vec(ast);
};

sub analyze_expr_vec(vec<Expr> exprs) {
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_expr(exprs.buf[i]);
    };
};

sub analyze_expr(Expr expr) {
    for i = 0, (i) < expr.tags.len, i += 1 {
        match expr.tags.buf[i].kind {
            ExprKind:Tag(name, args) {
                if str:cmp(name, "gen") == 0 {
                    
                };
            },
        };
    };

    match expr.kind {
        ExprKind:Comp(path, fields, body) {},
    };
};

` sub main(int abc) {
`     foo; ` undeclered foo
`     bar; ` good
`     int foo;
`     foo; ` good
`     abc; ` good
` }
` 
` int bar;
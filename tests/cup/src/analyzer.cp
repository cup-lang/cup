sub REMOVE_ME2() { vec<Expr>:new(0); vec<PathPart>:new(0); vec<GenName>:new(0); vec<GenericType>:new(0); };

comp MangledPath {
    vec<PathPart> path,
    ptr<u8> name,
};

int mangle_index = 0;
vec<vec<MangledPath>> mangled_paths;

vec<GenericType> gen_types;

comp GenName {
    ptr<u8> name,
};

comp GenericType {
    ptr<u8> name,
    ptr<Expr> path,
    ptr<Expr> _type,
};

sub analyze(vec<Expr> ast) {
    mangled_paths = vec<vec<MangledPath>>:new(8);
    gen_types = vec<GenericType>:new(8);
    analyze_expr_vec(ast);
};

sub analyze_expr_vec(vec<Expr> exprs) {
    for i = 0, (i) < exprs.len, i += 1 {
        analyze_expr(exprs.buf + i);
    };
};

sub analyze_expr(ptr<Expr> expr) {
    ptr<Expr> path;
    match expr@.kind {
        ExprKind:Block(body) {
            analyze_expr_vec(body);
            ret;
        },
        ExprKind:Comp(_path) {
            path = _path;
        },
        ExprKind:Enum(_path) {
            path = _path;
        },
        ExprKind:SubDef(_, _path) {
            path = _path;
        },
        _ {
            ret;
        }
    };

    if has_generics(path@) {
        gen_types.push(GenericType {
            name = mangle(path@, false),
            path = path,
            _type = expr,
        });
    };
};

ptr<u8> mangle(Expr expr, bool gens) {
    vec<PathPart> path = expr.kind.u.u2.path;
    if path.len <= mangled_paths.len {
        vec<MangledPath> paths = mangled_paths.buf[path.len - 1];
        ~l for i = 0, (i) < paths.len, i += 1 {
            if compare_paths(path, paths.buf[i].path, gens) {
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

bool compare_paths(vec<PathPart> path1, vec<PathPart> path2, bool gens) {
    ~l for i = 0, (i) < path1.len, i += 1 {
        PathPart part1 = path1.buf[i];
        PathPart part2 = path2.buf[i];
        if (gens == false) | (part1.gens.len == part2.gens.len) {
            if str:cmp(part1.name, part2.name) == 0 {
                if (gens == false) | compare_gens(part1.gens, part2.gens) {
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

        if compare_paths(path1, path2, true) == false {
            ret false;
        };
    };
    ret true;
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
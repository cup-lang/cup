comp Expr { u8 _ };

vec<parser:Expr> parse(vec<lexer:Token> tokens) {
    ret vec<parser:Expr>:new(1);
};
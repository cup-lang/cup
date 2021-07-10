enum TokenKind (
    Unset,
    Empty,
    Ident,
    StringLit,
    CharLit,
    IntLit,
    FloatLit,
    True,
    False,
    This,
    Type,

    Tag,
    Mod,
    Use,
    Comp,
    Enum,
    Prop,
    Def,
    Sub,
    Var,
    Where,

    If,
    Elif,
    Else,
    Loop,
    While,
    For,
    Each,
    In,
    Match,
    Ret,
    Next,
    Jump,
    Try,
    As,
    Has,

    Semicolon,
    Colon,
    Comma,
    Dot,
    QuestionMark,
    Tilde,
    Hash,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,

    Deref,
    Address,
    Assign,
    Equal,
    Not,
    NotEqual,
    And,
    Or,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Add,
    AddAssign,
    Subtract,
    SubtractAssign,
    Multiply,
    MultiplyAssign,
    Divide,
    DivideAssign,
    Modulo,
    ModuloAssign,
);

int get_token_length(TokenKind kind) {
    match kind {
        TokenKind:True { ret 4; },
        TokenKind:False { ret 5; },
        TokenKind:This { ret 4; },
        TokenKind:Type { ret 4; },
        TokenKind:Tag { ret 3; },
        TokenKind:Mod { ret 3; },
        TokenKind:Use { ret 3; },
        TokenKind:Comp { ret 4; },
        TokenKind:Enum { ret 4; },
        TokenKind:Prop { ret 4; },
        TokenKind:Def { ret 3; },
        TokenKind:Sub { ret 3; },
        TokenKind:Var { ret 3; },
        TokenKind:Where { ret 5; },
        TokenKind:If { ret 2; },
        TokenKind:Elif { ret 4; },
        TokenKind:Else { ret 4; },
        TokenKind:Loop { ret 4; },
        TokenKind:While { ret 5; },
        TokenKind:For { ret 3; },
        TokenKind:Each { ret 4; },
        TokenKind:In { ret 2; },
        TokenKind:Match { ret 5; },
        TokenKind:Ret { ret 3; },
        TokenKind:Next { ret 4; },
        TokenKind:Jump { ret 4; },
        TokenKind:Try { ret 3; },
        TokenKind:As { ret 2; },
        TokenKind:Has { ret 3; },
        TokenKind:Semicolon { ret 1; },
        TokenKind:Colon { ret 1; },
        TokenKind:Comma { ret 1; },
        TokenKind:Dot { ret 1; },
        TokenKind:QuestionMark { ret 1; },
        TokenKind:Tilde { ret 1; },
        TokenKind:Hash { ret 1; },
        TokenKind:LeftParen { ret 1; },
        TokenKind:RightParen { ret 1; },
        TokenKind:LeftBrace { ret 1; },
        TokenKind:RightBrace { ret 1; },
        TokenKind:LeftBracket { ret 1; },
        TokenKind:RightBracket { ret 1; },
        TokenKind:Deref { ret 1; },
        TokenKind:Address { ret 1; },
        TokenKind:Assign { ret 1; },
        TokenKind:Equal { ret 2; },
        TokenKind:Not { ret 1; },
        TokenKind:NotEqual { ret 2; },
        TokenKind:And { ret 1; },
        TokenKind:Or { ret 1; },
        TokenKind:Less { ret 1; },
        TokenKind:LessEqual { ret 2; },
        TokenKind:Greater { ret 1; },
        TokenKind:GreaterEqual { ret 2; },
        TokenKind:Add { ret 1; },
        TokenKind:AddAssign { ret 2; },
        TokenKind:Subtract { ret 1; },
        TokenKind:SubtractAssign { ret 2; },
        TokenKind:Multiply { ret 1; },
        TokenKind:MultiplyAssign { ret 2; },
        TokenKind:Divide { ret 1; },
        TokenKind:DivideAssign { ret 2; },
        TokenKind:Modulo { ret 1; },
        TokenKind:ModuloAssign { ret 2; },
    };
};

ptr<u8> get_token_name(TokenKind kind) {
    match kind {
        TokenKind:Empty { ret "EMPTY"; },
        TokenKind:Ident { ret "IDENT"; },
        TokenKind:StringLit { ret "STRING_LIT"; },
        TokenKind:CharLit { ret "CHAR_LIT"; },
        TokenKind:IntLit { ret "INT_LIT"; },
        TokenKind:FloatLit { ret "FLOAT_LIT"; },
        TokenKind:True { ret "TRUE"; },
        TokenKind:False { ret "FALSE"; },
        TokenKind:This { ret "THIS"; },
        TokenKind:Type { ret "TYPE"; },
        TokenKind:Tag { ret "TAG"; },
        TokenKind:Mod { ret "MOD"; },
        TokenKind:Use { ret "USE"; },
        TokenKind:Comp { ret "COMP"; },
        TokenKind:Enum { ret "ENUM"; },
        TokenKind:Prop { ret "PROP"; },
        TokenKind:Def { ret "DEF"; },
        TokenKind:Sub { ret "SUB"; },
        TokenKind:Var { ret "VAR"; },
        TokenKind:Where { ret "WHERE"; },
        TokenKind:If { ret "IF"; },
        TokenKind:Elif { ret "ELIF"; },
        TokenKind:Else { ret "ELSE"; },
        TokenKind:Loop { ret "LOOP"; },
        TokenKind:While { ret "WHILE"; },
        TokenKind:For { ret "FOR"; },
        TokenKind:Each { ret "EACH"; },
        TokenKind:In { ret "IN"; },
        TokenKind:Match { ret "MATCH"; },
        TokenKind:Ret { ret "RET"; },
        TokenKind:Next { ret "NEXT"; },
        TokenKind:Jump { ret "JUMP"; },
        TokenKind:Try { ret "TRY"; },
        TokenKind:As { ret "AS"; },
        TokenKind:Has { ret "HAS"; },
        TokenKind:Semicolon { ret "SEMICOLON"; },
        TokenKind:Colon { ret "COLON"; },
        TokenKind:Comma { ret "COMMA"; },
        TokenKind:Dot { ret "DOT"; },
        TokenKind:QuestionMark { ret "QUESTION_MARK"; },
        TokenKind:Tilde { ret "TILDE"; },
        TokenKind:Hash { ret "HASH"; },
        TokenKind:LeftParen { ret "LEFT_PAREN"; },
        TokenKind:RightParen { ret "RIGHT_PAREN"; },
        TokenKind:LeftBrace { ret "LEFT_BRACE"; },
        TokenKind:RightBrace { ret "RIGHT_BRACE"; },
        TokenKind:LeftBracket { ret "LEFT_BRACKET"; },
        TokenKind:RightBracket { ret "RIGHT_BRACKET"; },
        TokenKind:Deref { ret "DEREF"; },
        TokenKind:Address { ret "ADDRESS"; },
        TokenKind:Assign { ret "ASSIGN"; },
        TokenKind:Equal { ret "EQUAL"; },
        TokenKind:Not { ret "NOT"; },
        TokenKind:NotEqual { ret "NOT_EQUAL"; },
        TokenKind:And { ret "AND"; },
        TokenKind:Or { ret "OR"; },
        TokenKind:Less { ret "LESS"; },
        TokenKind:LessEqual { ret "LESS_EQUAL"; },
        TokenKind:Greater { ret "GREATER"; },
        TokenKind:GreaterEqual { ret "GREATER_EQUAL"; },
        TokenKind:Add { ret "ADD"; },
        TokenKind:AddAssign { ret "ADD_ASSIGN"; },
        TokenKind:Subtract { ret "SUBTRACT"; },
        TokenKind:SubtractAssign { ret "SUBTRACT_ASSIGN"; },
        TokenKind:Multiply { ret "MULTIPLY"; },
        TokenKind:MultiplyAssign { ret "MULTIPLY_ASSIGN"; },
        TokenKind:Divide { ret "DIVIDE"; },
        TokenKind:DivideAssign { ret "DIVIDE_ASSIGN"; },
        TokenKind:Modulo { ret "MODULO"; },
        TokenKind:ModuloAssign { ret "MODULO_ASSIGN"; },
    };
};

comp Token (
    TokenKind kind,
    int index,
    ptr<u8> value,
);

sub add_to_value(File file, ptr<vec<u8>> value, int i, u8 c, u8 is_literal) {
    if (is_literal == 2) & (value@.len == 4) {
        throw(file, i, "too many characters in character literal");
    };
    push_char(value, c);
};

TokenKind get_keyword(ptr<u8> value) {
    if str:cmp(value, "tag") == 0 {
        ret TokenKind:Tag;
    } elif str:cmp(value, "mod") == 0 {
        ret TokenKind:Mod;
    } elif str:cmp(value, "use") == 0 {
        ret TokenKind:Use;
    } elif str:cmp(value, "comp") == 0 {
        ret TokenKind:Comp;
    } elif str:cmp(value, "enum") == 0 {
        ret TokenKind:Enum;
    } elif str:cmp(value, "prop") == 0 {
        ret TokenKind:Prop;
    } elif str:cmp(value, "def") == 0 {
        ret TokenKind:Def;
    } elif str:cmp(value, "sub") == 0 {
        ret TokenKind:Sub;
    } elif str:cmp(value, "var") == 0 {
        ret TokenKind:Var;
    } elif str:cmp(value, "this") == 0 {
        ret TokenKind:This;
    } elif str:cmp(value, "type") == 0 {
        ret TokenKind:Type;
    } elif str:cmp(value, "where") == 0 {
        ret TokenKind:Where;
    } elif str:cmp(value, "true") == 0 {
        ret TokenKind:True;
    } elif str:cmp(value, "false") == 0 {
        ret TokenKind:False;
    } elif str:cmp(value, "if") == 0 {
        ret TokenKind:If;
    } elif str:cmp(value, "elif") == 0 {
        ret TokenKind:Elif;
    } elif str:cmp(value, "else") == 0 {
        ret TokenKind:Else;
    } elif str:cmp(value, "loop") == 0 {
        ret TokenKind:Loop;
    } elif str:cmp(value, "while") == 0 {
        ret TokenKind:While;
    } elif str:cmp(value, "for") == 0 {
        ret TokenKind:For;
    } elif str:cmp(value, "each") == 0 {
        ret TokenKind:Each;
    } elif str:cmp(value, "in") == 0 {
        ret TokenKind:In;
    } elif str:cmp(value, "match") == 0 {
        ret TokenKind:Match;
    } elif str:cmp(value, "ret") == 0 {
        ret TokenKind:Ret;
    } elif str:cmp(value, "next") == 0 {
        ret TokenKind:Next;
    } elif str:cmp(value, "jump") == 0 {
        ret TokenKind:Jump;
    } elif str:cmp(value, "try") == 0 {
        ret TokenKind:Try;
    } elif str:cmp(value, "as") == 0 {
        ret TokenKind:As;
    } elif str:cmp(value, "has") == 0 {
        ret TokenKind:Has;
    };
    ret TokenKind:Empty;
}; 

bool is_binary_operator(TokenKind kind) {
    match kind {
        TokenKind:LeftParen { ret true; },
        TokenKind:Assign { ret true; },
        TokenKind:Equal { ret true; },
        TokenKind:NotEqual { ret true; },
        TokenKind:And { ret true; },
        TokenKind:Or { ret true; },
        TokenKind:Less { ret true; },
        TokenKind:LessEqual { ret true; },
        TokenKind:Greater { ret true; },
        TokenKind:GreaterEqual { ret true; },
        TokenKind:Add { ret true; },
        TokenKind:AddAssign { ret true; },
        TokenKind:Subtract { ret true; },
        TokenKind:SubtractAssign { ret true; },
        TokenKind:Multiply { ret true; },
        TokenKind:MultiplyAssign { ret true; },
        TokenKind:Divide { ret true; },
        TokenKind:DivideAssign { ret true; },
        TokenKind:Modulo { ret true; },
        TokenKind:ModuloAssign { ret true; },
        TokenKind:As { ret true; },
        TokenKind:Has { ret true; },
        _ { ret false; },
    };
};

vec<Token> lex(File file) {
    vec<Token> tokens = vec<Token>:new(32);
    u8 is_comment = 0;
    u8 is_literal = 0;
    vec<u8> value = vec<u8>:new(8);
    value.empty();

    ~l for i = 0, i <= file.data.len, i += 1 {
        u8 c = file.data.buf[i];

        if ((((is_literal != 1) & (is_literal != 2)) & (is_literal != 3)) & (is_literal != 4)) & (c == '`') {
            is_comment = 1;
            next ~l;
        };

        if is_comment != 0 {
            if c == '\n' {
                is_comment = 0;
            };
            next ~l;
        };

        TokenKind kind = TokenKind:Unset;

        if (is_literal == 3) | (is_literal == 4) {
            is_literal -= 2;
        };

        if (is_literal == 1) | (is_literal == 2) {
            if c == 0 {
                if is_literal == 1 {
                    throw(file, i - 1, "expected end of string literal");
                } else {
                    throw(file, i - 1, "expected end of char literal");
                };
            }
            elif (is_literal == 1) & (c == '"') {}
            elif (is_literal == 2) & (c == 39) {}
            else {
                add_to_value(file, value$, i, c, is_literal);
                next ~l;
            };
        };

        ~ll if (c == 0) | ch:is_space(c) {
            kind = TokenKind:Empty;
        } else {
            if c == '"' {
                if is_literal == 1 {
                    kind = TokenKind:StringLit;
                    is_literal = 0;
                } else {
                    kind = TokenKind:Empty;
                    is_literal = 3;
                };
            } elif c == 39 {
                if is_literal == 2 {
                    kind = TokenKind:CharLit;
                    is_literal = 0;
                } else {
                    kind = TokenKind:Empty;
                    is_literal = 4;
                };
            } elif c == ';' {
                kind = TokenKind:Semicolon;
            } elif c == ':' {
                kind = TokenKind:Colon;
            } elif c == ',' {
                kind = TokenKind:Comma;
            } elif c == '.' {
                u8 n = file.data.buf[i + 1];
                if (is_literal != 5) | ((n == '_') | ch:is_alpha(n)) {
                    kind = TokenKind:Dot;
                };
            } elif c == '?' {
                kind = TokenKind:QuestionMark;
            } elif c == '~' {
                kind = TokenKind:Tilde;
            } elif c == '#' {
                kind = TokenKind:Hash;
            } elif c == '(' {
                kind = TokenKind:LeftParen;
            } elif c == ')' {
                kind = TokenKind:RightParen;
            } elif c == '{' {
                kind = TokenKind:LeftBrace;
            } elif c == '}' {
                kind = TokenKind:RightBrace;
            } elif c == '[' {
                kind = TokenKind:LeftBracket;
            } elif c == ']' {
                kind = TokenKind:RightBracket;
            } elif c == '@' {
                kind = TokenKind:Deref;
            } elif c == '$' {
                kind = TokenKind:Address;
            } elif c == '&' {
                kind = TokenKind:And;
            } elif c == '|' {
                kind = TokenKind:Or;
            } elif c == '=' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:Equal;
                    i += 1;
                } else {
                    kind = TokenKind:Assign;
                };
            } elif c == '!' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:NotEqual;
                    i += 1;
                } else {
                    kind = TokenKind:Not;
                };
            } elif c == '<' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:LessEqual;
                    i += 1;
                } else {
                    kind = TokenKind:Less;
                };
            } elif c == '>' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:GreaterEqual;
                    i += 1;
                } else {
                    kind = TokenKind:Greater;
                };
            } elif c == '+' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:AddAssign;
                    i += 1;
                } else {
                    kind = TokenKind:Add;
                };
            } elif c == '-' {
                if is_binary_operator(tokens.buf[tokens.len - 1].kind) & ch:is_num(file.data.buf[i + 1]) {
                    ret ~ll;
                };
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:SubtractAssign;
                    i += 1;
                } else {
                    kind = TokenKind:Subtract;
                };
            } elif c == '*' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:MultiplyAssign;
                    i += 1;
                } else {
                    kind = TokenKind:Multiply;
                };
            } elif c == '/' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:DivideAssign;
                    i += 1;
                } else {
                    kind = TokenKind:Divide;
                };
            } elif c == '%' {
                if (i + 1 < file.data.len) & (file.data.buf[i + 1] == '=') {
                    kind = TokenKind:ModuloAssign;
                    i += 1;
                } else {
                    kind = TokenKind:Modulo;
                };
            };
        };

        match kind {
            TokenKind:Unset {
                if ((((c == '-') | (c == '_')) | (c == '.')) | (c == ':')) | ch:is_alpha_num(c) {
                    if ((value.len == 0) & ch:is_num(c)) | (c == '-') {
                        is_literal = 5;
                    } elif (is_literal == 5) | (is_literal == 6) {
                        if (c == '.') & (is_literal == 5) {
                            is_literal = 6;
                        } elif (c == '_') | ch:is_num(c) {} 
                        else {
                            throw(file, i - value.len, "invalid identifier name starting with a digit");
                        };
                    };

                    if ((is_literal != 5) & (is_literal != 6)) | (c != '_') {
                        add_to_value(file, value$, i, c, is_literal);
                    };
                } else {
                    throw(file, i, "unexpected symbol %c", c);
                };
            },
            _ {
                if value.len > 0 {
                    match kind {
                        TokenKind:StringLit {},
                        TokenKind:CharLit {},
                        _ {
                            TokenKind value_kind = get_keyword(value.buf);
                            
                            match value_kind {
                                TokenKind:Empty {
                                    Token token;
                                    if is_literal == 5 {
                                        token.kind = TokenKind:IntLit;
                                    } elif is_literal == 6 {
                                        token.kind = TokenKind:FloatLit;
                                        if value.buf[value.len - 1] == '.' {
                                            throw(file, i - 1, "expected a value after the decimal point");
                                        };
                                    } else {
                                        token.kind = TokenKind:Ident;
                                    };
                                    token.index = i - value.len;
                                    token.value = value.buf;
                                    tokens.push(token);
                                },
                                _ {
                                    Token token = Token(
                                        kind = value_kind,
                                        index = i - get_token_length(value_kind),
                                    );
                                    tokens.push(token);
                                },
                            };
                            
                            value = vec<u8>:new(8);
                            value.empty();
                            is_literal = 0;
                        },
                    };
                };

                match kind {
                    TokenKind:Empty {},
                    _ {
                        Token token = Token(
                            kind = kind,
                        );
                        match kind {
                            TokenKind:StringLit {
                                token.value = value.buf;
                                token.index = i - value.len;
                                value = vec<u8>:new(8);
                                value.empty();
                            },
                            TokenKind:CharLit {
                                token.value = value.buf;
                                token.index = i - value.len;
                                value = vec<u8>:new(8);
                                value.empty();
                            },
                            _ {
                                token.index = i - get_token_length(kind) + 1;
                            },
                        };
                        tokens.push(token);
                    },
                };
            },
        };
    };

    Token last;
    if tokens.len > 0 {
        last = tokens.buf[tokens.len - 1];
        match last.kind {
            TokenKind:Ident {
                last.index = last.index + str:len(last.value);
            },
            _ {
                last.index = last.index + get_token_length(last.kind);
            },
        };
    } else {
        last.index = 0;
    };
    last.kind = TokenKind:Empty;
    tokens.push(last);

    ret tokens;
};

sub print_tokens(vec<Token> tokens) {
    fmt:print("Tokens:\n");
    for i = 0, (i) < tokens.len, i += 1 {
        TokenKind kind = tokens.buf[i].kind;
        set_color(Color:Magenta);
        fmt:print("  %s", get_token_name(kind));
        set_color(Color:Reset);
        match kind {
            TokenKind:Ident { fmt:print("(%s)", tokens.buf[i].value); },
            TokenKind:StringLit { fmt:print("(%s)", tokens.buf[i].value); },
            TokenKind:CharLit { fmt:print("(%s)", tokens.buf[i].value); },
            TokenKind:IntLit { fmt:print("(%s)", tokens.buf[i].value); },
            TokenKind:FloatLit { fmt:print("(%s)", tokens.buf[i].value); },
        };
        putchar('\n');
    };
    putchar('\n');
};
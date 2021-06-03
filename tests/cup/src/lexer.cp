enum Token {
    Empty,
    Ident,
    StringLit,
    CharLit,
    IntLit,
    FloatLit,
    True,
    False,
    None,
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
    Match,
    Ret,
    Next,
    Jump,
    Try,
    As,

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
    rightBracket,

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
};

sub add_to_value(vec<u8> value, u8 c, u8 is_literal) {
    if (is_literal == 2) & (value.len == 4) {
        ` throw `at ${i} too many characters in character literal`;
    };
    value.push(c);
};

vec<Token> lex(arr<u8> file) {
    vec<Token> tokens = vec<Token>:new(1);
    u8 is_comment = 0;
    u8 is_literal = 0;
    vec<u8> value = vec<u8>:new(8);

    ~l for i = 0, i <= file.len, i += 1 {
        u8 c = file.buf[i];

        if ((((is_literal != 1) & (is_literal != 2)) & (is_literal != 3)) & (is_literal != 4)) & (c == '`') {
            is_comment = 1;
            next ~l;
        };

        if is_comment != 0 {
            if c == '\n' {
                is_comment = 0;
            }
            next ~l;
        };

        Token kind = Token:Empty;

        if (is_literal == 3) | (is_literal == 4) {
            is_literal -= 2;
        };

        if (is_literal == 1) | (is_literal == 2) {
            if c == 0 {
                if is_literal == 1 {
                    throw(file, "test_name", i - 1, "expected end of %s literal", "string");
                } else {
                    throw(file, "test_name", i - 1, "expected end of %s literal", "char");
                };
            }
            elif (is_literal == 1) & (c == '"') { }
            elif (is_literal == 2) & (c == 39) { }
            else {
                add_to_value(value, c, is_literal);
                next ~l;
            };
        };

        if ch:is_space(c) {
            kind = Token:Empty;
        } else {
            if c == '"' {
                if is_literal == 1 {
                    kind = Token:StringLit;
                    is_literal = 0;
                } else {
                    kind = Token:Empty;
                    is_literal = 3;
                };
            } elif c == 39 {
                if is_literal == 2 {
                    kind = Token:CharLit;
                    is_literal = 0;
                } else {
                    kind = Token:Empty;
                    is_literal = 4;
                };
            } elif c == ';' {
                kind = Token:Semicolon;
            } elif c == ':' {
                kind = Token:Colon;
            } elif c == ',' {
                kind = Token:Comma;
            } elif c == '.' {
                u8 n = file.buf[i + 1];
                if (is_literal != 5) | ((n == '_') | ch:is_alpha(n)) {
                    kind = Token:Dot;
                };
            } elif c == '?' {
                kind = Token:QuestionMark;
            } elif c == '~' {
                kind = Token:Tilde;
            } elif c == '#' {
                kind = Token:Hash;
            } elif c == '(' {
                kind = Token:LeftParen;
            } elif c == ')' {
                kind = Token:RightParen;
            } elif c == '{' {
                kind = Token:LeftBrace;
            } elif c == '}' {
                kind = Token:RightBrace;
            } elif c == '[' {
                kind = Token:LeftBracket;
            } elif c == ']' {
                kind = Token:RightBracket;
            } elif c == '@' {
                kind = Token:Deref;
            } elif c == '$' {
                kind = Token:Address;
            } elif c == '&' {
                kind = Token:And;
            } elif c == '|' {
                kind = Token:Or;
            } elif c == '=' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:Equal;
                    i += 1;
                } else {
                    kind = Token:Assign;
                };
            } elif c == '!' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:NotEqual;
                    i += 1;
                } else {
                    kind = Token:Not;
                };
            } elif c == '<' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:LessEqual;
                    i += 1;
                } else {
                    kind = Token:Less;
                };
            } elif c == '>' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:GreaterEqual;
                    i += 1;
                } else {
                    kind = Token:Greater;
                };
            } elif c == '+' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:AddAssign;
                    i += 1;
                } else {
                    kind = Token:Add;
                };
            } elif c == '-' {
                ` if (isBinaryOperator(tokens[tokens.length - 1]) && isNumeric(input[i + 1])) {
                `     break;
                ` }
                ` kind = tokenKind.SUBTRACT;
                ` if (i + 1 < input.length) {
                `     switch (input[i + 1]) {
                `         case '>':
                `             kind = tokenKind.ARROW;
                `             ++i;
                `             break;
                `         case '=':
                `             kind = tokenKind.SUBTRACT_ASSIGN;
                `             ++i;
                `             break;
                `     }
                ` };
            } elif c == '*' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:MultiplyAssign;
                    i += 1;
                } else {
                    kind = Token:Multiply;
                };
            } elif c == '/' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:DivideAssign;
                    i += 1;
                } else {
                    kind = Token:Divide;
                };
            } elif c == '%' {
                if (i + 1 < file.len) & (file.buf[i + 1] == '=') {
                    kind = Token:ModuloAssign;
                    i += 1;
                } else {
                    kind = Token:Modulo;
                };
            };
        };

        match kind {
            Token:Empty {
                if ((((c == '-') | (c == '_')) | (c == '.')) | (c == ':')) | ch:is_alpha_num(c) {
                    if ((value.len == 0) & ch:is_num(c)) | (c == '-') {
                        is_literal = 5;
                    } elif (is_literal == 5) | (is_literal == 6) {
                        if (c == '.') & (is_literal == 5) {
                            is_literal = 6;
                        } elif (c == '_') | ch:is_num(c) { } 
                        else {
                            throw(file, "test_name", i - value.len, "invalid identifier name starting with a digit");
                        };
                    };

                    if ((is_literal != 5) & (is_literal != 6)) | (c != '_') {
                        add_to_value(value, c, is_literal);
                    };
                } else {
                    throw(file, "test_name", i, "unexpected symbol %c", c);
                };
            },
            _ {
                ` if ((value.len > 0) & (kind != Token:StringLit)) & (kind != Token:CharLit) {
                `     const value_kind = keywords[value];
                `     
                `     if value_kind == undefined {
                `         let token = {};
                `         switch (is_literal) {
                `             case 5:
                `                 token.kind = tokenKind.INT_LIT;
                `                 break;
                `             case 6:
                `                 token.kind = tokenKind.FLOAT_LIT;
                `                 if (value[value.length - 1] === '.') {
                `                     throw `at ${i - 1} expected a value after the decimal point`;
                `                 }
                `                 break;
                `             default:
                `                 token.kind = tokenKind.IDENT;
                `                 break;
                `         }
                `         token.value = value;
                `         token.index = i - value.length;
                `         tokens.push(token);
                `     } else {
                `         let token = { kind: value_kind, index: i - value_kind.length };
                `         tokens.push(token);
                `     };
                `     
                `     value = "";
                `     is_literal = 0;
                ` };

                ` if kind != Token:Empty {
                `     Tokens token = { kind: kind };
                `     if (kind == Token:StringLit) | (kind == Token:CharLit) {
                `         token.value = value;
                `         token.index = i - value.length;
                `         value = '';
                `     } else {
                `         token.index = i - kind.length + 1;
                `     };
                `     tokens.push(token);
                ` };
            },
        };
    };

    ret tokens;
};
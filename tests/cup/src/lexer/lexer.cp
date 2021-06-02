enum Token {
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
    Question_mark,
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

vec<lexer:Token> lex() {
    vec<lexer:Token> tokens = vec<lexer:Token>:new(1);
    u8 is_comment = 0;
    u8 is_literal = 0;
    vec<u8> value = vec<u8>:new(8);

    for i = 0, i <= input.length, i += 1 {
        
    };

    ret tokens;
};
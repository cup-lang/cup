#include <stdint.h>
typedef struct
{
    uint8_t *value;
    char _;
} lexer_Token_Ident;
typedef struct
{
    char _;
} lexer_Token_As;
typedef struct
{
    char _;
} lexer_Token_Do;
typedef struct
{
    char _;
} lexer_Token_In;
typedef struct
{
    char _;
} lexer_Token_If;
typedef struct
{
    char _;
} lexer_Token_Pub;
typedef struct
{
    char _;
} lexer_Token_Use;
typedef struct
{
    char _;
} lexer_Token_Mod;
typedef struct
{
    char _;
} lexer_Token_For;
typedef struct
{
    char _;
} lexer_Token_Imp;
typedef struct
{
    char _;
} lexer_Token_Inl;
typedef struct
{
    char _;
} lexer_Token_Fall;
typedef struct
{
    char _;
} lexer_Token_Null;
typedef struct
{
    char _;
} lexer_Token_True;
typedef struct
{
    char _;
} lexer_Token_Rest;
typedef struct
{
    char _;
} lexer_Token_Enum;
typedef struct
{
    char _;
} lexer_Token_Elif;
typedef struct
{
    char _;
} lexer_Token_Else;
typedef struct
{
    char _;
} lexer_Token_Next;
typedef struct
{
    char _;
} lexer_Token_Goto;
typedef struct
{
    char _;
} lexer_Token_Self;
typedef struct
{
    char _;
} lexer_Token_False;
typedef struct
{
    char _;
} lexer_Token_Trait;
typedef struct
{
    char _;
} lexer_Token_Const;
typedef struct
{
    char _;
} lexer_Token_Defer;
typedef struct
{
    char _;
} lexer_Token_While;
typedef struct
{
    char _;
} lexer_Token_Union;
typedef struct
{
    char _;
} lexer_Token_Break;
typedef struct
{
    char _;
} lexer_Token_Match;
typedef struct
{
    char _;
} lexer_Token_Macro;
typedef struct
{
    char _;
} lexer_Token_Struct;
typedef struct
{
    char _;
} lexer_Token_Return;
typedef struct
{
    char _;
} lexer_Token_LeftParen;
typedef struct
{
    char _;
} lexer_Token_RightParen;
typedef struct
{
    char _;
} lexer_Token_LeftBrace;
typedef struct
{
    char _;
} lexer_Token_RightBrace;
typedef struct
{
    char _;
} lexer_Token_LeftSquare;
typedef struct
{
    char _;
} lexer_Token_RightSquare;
typedef struct
{
    char _;
} lexer_Token_Arrow;
typedef struct
{
    char _;
} lexer_Token_EqualArrow;
typedef struct
{
    char _;
} lexer_Token_Semicolon;
typedef struct
{
    char _;
} lexer_Token_Colon;
typedef struct
{
    char _;
} lexer_Token_Comma;
typedef struct
{
    char _;
} lexer_Token_Dot;
typedef struct
{
    char _;
} lexer_Token_Question;
typedef struct
{
    char _;
} lexer_Token_Range;
typedef struct
{
    char _;
} lexer_Token_Assign;
typedef struct
{
    char _;
} lexer_Token_Equal;
typedef struct
{
    char _;
} lexer_Token_NotEqual;
typedef struct
{
    char _;
} lexer_Token_Less;
typedef struct
{
    char _;
} lexer_Token_LessEqual;
typedef struct
{
    char _;
} lexer_Token_Greater;
typedef struct
{
    char _;
} lexer_Token_GreaterEqual;
typedef struct
{
    char _;
} lexer_Token_Not;
typedef struct
{
    char _;
} lexer_Token_And;
typedef struct
{
    char _;
} lexer_Token_Or;
typedef struct
{
    char _;
} lexer_Token_Add;
typedef struct
{
    char _;
} lexer_Token_AddAssign;
typedef struct
{
    char _;
} lexer_Token_Sub;
typedef struct
{
    char _;
} lexer_Token_SubAssign;
typedef struct
{
    char _;
} lexer_Token_Mul;
typedef struct
{
    char _;
} lexer_Token_MulAssign;
typedef struct
{
    char _;
} lexer_Token_Div;
typedef struct
{
    char _;
} lexer_Token_DivAssign;
typedef struct
{
    char _;
} lexer_Token_Modulo;
typedef struct
{
    char _;
} lexer_Token_ModuloAssign;
typedef struct
{
    char _;
} lexer_Token_BitNot;
typedef struct
{
    char _;
} lexer_Token_BitAnd;
typedef struct
{
    char _;
} lexer_Token_BitAndAssign;
typedef struct
{
    char _;
} lexer_Token_BitOr;
typedef struct
{
    char _;
} lexer_Token_BitOrAssign;
typedef struct
{
    char _;
} lexer_Token_BitXor;
typedef struct
{
    char _;
} lexer_Token_BitXorAssign;
typedef struct
{
    char _;
} lexer_Token_LeftShift;
typedef struct
{
    char _;
} lexer_Token_LeftShiftAssign;
typedef struct
{
    char _;
} lexer_Token_RightShift;
typedef struct
{
    char _;
} lexer_Token_RightShiftAssign;
typedef union
{
    lexer_Token_Ident _0;
    lexer_Token_As _1;
    lexer_Token_Do _2;
    lexer_Token_In _3;
    lexer_Token_If _4;
    lexer_Token_Pub _5;
    lexer_Token_Use _6;
    lexer_Token_Mod _7;
    lexer_Token_For _8;
    lexer_Token_Imp _9;
    lexer_Token_Inl _10;
    lexer_Token_Fall _11;
    lexer_Token_Null _12;
    lexer_Token_True _13;
    lexer_Token_Rest _14;
    lexer_Token_Enum _15;
    lexer_Token_Elif _16;
    lexer_Token_Else _17;
    lexer_Token_Next _18;
    lexer_Token_Goto _19;
    lexer_Token_Self _20;
    lexer_Token_False _21;
    lexer_Token_Trait _22;
    lexer_Token_Const _23;
    lexer_Token_Defer _24;
    lexer_Token_While _25;
    lexer_Token_Union _26;
    lexer_Token_Break _27;
    lexer_Token_Match _28;
    lexer_Token_Macro _29;
    lexer_Token_Struct _30;
    lexer_Token_Return _31;
    lexer_Token_LeftParen _32;
    lexer_Token_RightParen _33;
    lexer_Token_LeftBrace _34;
    lexer_Token_RightBrace _35;
    lexer_Token_LeftSquare _36;
    lexer_Token_RightSquare _37;
    lexer_Token_Arrow _38;
    lexer_Token_EqualArrow _39;
    lexer_Token_Semicolon _40;
    lexer_Token_Colon _41;
    lexer_Token_Comma _42;
    lexer_Token_Dot _43;
    lexer_Token_Question _44;
    lexer_Token_Range _45;
    lexer_Token_Assign _46;
    lexer_Token_Equal _47;
    lexer_Token_NotEqual _48;
    lexer_Token_Less _49;
    lexer_Token_LessEqual _50;
    lexer_Token_Greater _51;
    lexer_Token_GreaterEqual _52;
    lexer_Token_Not _53;
    lexer_Token_And _54;
    lexer_Token_Or _55;
    lexer_Token_Add _56;
    lexer_Token_AddAssign _57;
    lexer_Token_Sub _58;
    lexer_Token_SubAssign _59;
    lexer_Token_Mul _60;
    lexer_Token_MulAssign _61;
    lexer_Token_Div _62;
    lexer_Token_DivAssign _63;
    lexer_Token_Modulo _64;
    lexer_Token_ModuloAssign _65;
    lexer_Token_BitNot _66;
    lexer_Token_BitAnd _67;
    lexer_Token_BitAndAssign _68;
    lexer_Token_BitOr _69;
    lexer_Token_BitOrAssign _70;
    lexer_Token_BitXor _71;
    lexer_Token_BitXorAssign _72;
    lexer_Token_LeftShift _73;
    lexer_Token_LeftShiftAssign _74;
    lexer_Token_RightShift _75;
    lexer_Token_RightShiftAssign _76;
} lexer_TokenUnion;
typedef struct
{
    lexer_TokenUnion u;
    unsigned int t;
} lexer_Token;
typedef struct
{
    lexer_Token array;
    int size;
    int capacity;
} lexer_TokenVector;
lexer_TokenVector lexer_new_token_vector(int c)
{
    lexer_TokenVector lexer_v;
    (v.array) = lexer_malloc(lexer_sizeof(Token));
    (v.size) = 0;
    (v.capacity) = c;
    return v;
}
int lexer_lex(char *data)
{
    int lexer_i = 0;
    lexer_TokenVector lexer_tokens = lexer_new_token_vector();
}
int main(int argc, char **argv) {}
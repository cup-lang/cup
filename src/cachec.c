#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct CharVector
{
    char *array;
    int size;
    int capacity;
} CharVector;

CharVector NewCharVector(int capacity)
{
    CharVector vector;
    vector.array = malloc(capacity);
    vector.size = 0;
    vector.capacity = capacity;
    return vector;
}

void PushChar(CharVector *vector, char c)
{
    vector->array[vector->size++] = c;
    if (vector->size == vector->capacity)
    {
        int capacity = vector->capacity *= 2;
        vector->array = realloc(vector->array, capacity);
    }
}

#pragma region Lexer
typedef enum TokenType
{
    Ident,
    Function,
    Variable,
    Struct,
    Enum,
    In,
    Semicolon,
    Colon,
    Comma,
    Dot,
    // Flow Control
    If,
    Else,
    Loop,
    While,
    For,
    Return,
    Break,
    Continue,
    // Brackets
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftSquare,
    RightSquare,
    // Operators
    Range,
    Assign,
    Equal,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Add,
    AddAssign,
    Substract,
    SubstractAssign,
    Multiply,
    MultiplyAssign,
    Divide,
    DivideAssign,
    Modulo,
    ModuloAssign,
    Not,
    NotEqual
} TokenType;

typedef struct Token
{
    TokenType type;
    char *value;
} Token;

typedef struct TokenVector
{
    Token *array;
    int size;
    int capacity;
} TokenVector;

TokenVector NewTokenVector(int capacity)
{
    TokenVector vector;
    vector.array = malloc(sizeof(Token) * capacity);
    vector.size = 0;
    vector.capacity = capacity;
    return vector;
}

void PushToken(TokenVector *vector, Token expr)
{
    vector->array[vector->size++] = expr;
    if (vector->size == vector->capacity)
    {
        int capacity = vector->capacity *= 2;
        vector->array = realloc(vector->array, sizeof(Token) * capacity);
    }
}

Token IdentToken(CharVector *vector)
{
    Token token;
    token.type = Ident;
    token.value = malloc(vector->size + 1);
    memcpy(token.value, vector->array, vector->size);
    token.value[vector->size] = '\0';
    vector->size = 0;
    return token;
}

void AddToken(Token *tokens, size_t *size, Token token)
{
    tokens[*size] = token;
    ++(*size);
}

TokenVector Tokenize(int file_size, char *buffer)
{
    TokenVector tokens = NewTokenVector(file_size / 4);
    CharVector value = NewCharVector(32);
    char is_comment = 0;

    // Loop through all characters and create tokens
    for (int i = 0; i < file_size; ++i)
    {
        char c = buffer[i];

        if (is_comment)
        {
            if (c == '\n')
            {
                is_comment = 0;
            }
            continue;
        }

        if (value.size != 0 && value.array[0] == '"')
        {
            PushChar(&value, c);
            if (c == '"')
            {
                PushToken(&tokens, IdentToken(&value));
            }
        }
        else
        {
            Token token;
            token.type = -1;
            token.value = NULL;

            // Check assign
            char is_assign = i + 1 < file_size && buffer[i + 1] == '=';

            switch (c)
            {
            case '=':
                token.type = Assign + is_assign;
                break;
            case '<':
                token.type = Less + is_assign;
                break;
            case '>':
                token.type = Greater + is_assign;
                break;
            case '+':
                token.type = Add + is_assign;
                break;
            case '-':
                token.type = Substract + is_assign;
                break;
            case '*':
                token.type = Multiply + is_assign;
                break;
            case '/':
                if (i + 1 < file_size && buffer[i + 1] == '/')
                {
                    is_comment = 1;
                }
                else
                {
                    token.type = Divide + is_assign;
                }
                break;
            case '%':
                token.type = Modulo + is_assign;
                break;
            case '!':
                token.type = Not + is_assign;
                break;
            case '(':
                token.type = LeftParen;
                break;
            case ')':
                token.type = RightParen;
                break;
            case '{':
                token.type = LeftBrace;
                break;
            case '}':
                token.type = RightBrace;
                break;
            case '[':
                token.type = LeftSquare;
                break;
            case ']':
                token.type = RightSquare;
                break;
            case ';':
                token.type = Semicolon;
                break;
            case ':':
                token.type = Colon;
                break;
            case ',':
                token.type = Comma;
                break;
            case '.':
                token.type = Dot;
                if (i + 1 < file_size && buffer[i + 1] == '.')
                {
                    token.type = Range;
                    i += 1;
                }
                break;
            case 'v':
                if (i + 3 < file_size && buffer[i + 1] == 'a' && buffer[i + 2] == 'r' && isspace(buffer[i + 3]))
                {
                    token.type = Variable;
                    i += 3;
                }
                break;
            case 's':
                if (i + 6 < file_size && buffer[i + 1] == 't' && buffer[i + 2] == 'r' && buffer[i + 3] == 'u' && buffer[i + 4] == 'c' && buffer[i + 5] == 't' && isspace(buffer[i + 6]))
                {
                    token.type = Struct;
                    i += 6;
                }
                break;
            case 'i':
                if (i + 2 < file_size && buffer[i + 1] == 'f' && isspace(buffer[i + 2]))
                {
                    token.type = If;
                    i += 2;
                }
                break;
            case 'e':
                if (i + 4 < file_size && buffer[i + 1] == 'n' && buffer[i + 2] == 'u' && buffer[i + 3] == 'm' && isspace(buffer[i + 4]))
                {
                    token.type = Enum;
                    i += 4;
                }
                else if (i + 4 < file_size && buffer[i + 1] == 'l' && buffer[i + 2] == 's' && buffer[i + 3] == 'e' && isspace(buffer[i + 4]))
                {
                    token.type = Else;
                    i += 4;
                }
                break;
            case 'l':
                if (i + 4 < file_size && buffer[i + 1] == 'o' && buffer[i + 2] == 'o' && buffer[i + 3] == 'p' && (isspace(buffer[i + 4]) || buffer[i + 4] == '{'))
                {
                    token.type = Loop;
                    i += 3;
                }
                break;
            case 'w':
                if (i + 5 < file_size && buffer[i + 1] == 'h' && buffer[i + 2] == 'i' && buffer[i + 3] == 'l' && buffer[i + 4] == 'e' && isspace(buffer[i + 5]))
                {
                    token.type = While;
                    i += 5;
                }
                break;
            case 'f':
                if (i + 2 < file_size && buffer[i + 1] == 'n' && isspace(buffer[i + 2]))
                {
                    token.type = Function;
                    i += 2;
                }
                break;
            case 'r':
                if (i + 6 < file_size && buffer[i + 1] == 'e' && buffer[i + 2] == 't' && buffer[i + 3] == 'u' && buffer[i + 4] == 'r' && buffer[i + 5] == 'n' && (isspace(buffer[i + 6]) || buffer[i + 6] == ';'))
                {
                    token.type = Return;
                    i += 5;
                }
                break;
            case 'b':
                if (i + 5 < file_size && buffer[i + 1] == 'r' && buffer[i + 2] == 'e' && buffer[i + 3] == 'a' && buffer[i + 4] == 'k' && (isspace(buffer[i + 5]) || buffer[i + 5] == ';'))
                {
                    token.type = Break;
                    i += 4;
                }
                break;
            case 'c':
                if (i + 8 < file_size && buffer[i + 1] == 'o' && buffer[i + 2] == 'n' && buffer[i + 3] == 't' && buffer[i + 4] == 'i' && buffer[i + 5] == 'n' && buffer[i + 6] == 'u' && buffer[i + 7] == 'e' && (isspace(buffer[i + 8]) || buffer[i + 8] == ';'))
                {
                    token.type = Continue;
                    i += 7;
                }
                break;
            }

            // Push the token
            if (token.type != -1)
            {
                if (value.size != 0)
                {
                    PushToken(&tokens, IdentToken(&value));
                }
                PushToken(&tokens, token);
            }
            // Update value
            else if (!is_comment && !isspace(c))
            {
                PushChar(&value, c);
            }
        }
    }

    free(value.array);
    return tokens;
}
#pragma endregion

#pragma region Parser
typedef struct Expr Expr;

typedef struct ExprVector
{
    Expr *array;
    int size;
    int capacity;
} ExprVector;

struct FnDef
{
    char *name;
    char *type;
    ExprVector args;
    ExprVector body;
};

struct Arg
{
    char *name;
    char *type;
};

struct FnCall
{
    char *name;
    ExprVector args;
};

struct VarDef
{
    char *name;
    char *type;
    Expr *val;
};

struct Struct
{
    char *name;
    ExprVector body;
};

struct Enum
{
    char *name;
};

struct VarUse
{
    char *name;
};

struct Op
{
    TokenType type;
    Expr *lhs;
    Expr *rhs;
};

struct Value
{
    char *type;
    char *val;
};

struct Array
{
    ExprVector elems;
};

struct Block
{
    ExprVector body;
};

struct If
{
    char elif;
    Expr *con;
    ExprVector body;
};

struct Else
{
    ExprVector body;
};

struct Loop
{
    ExprVector body;
};

struct While
{
    Expr *con;
    ExprVector body;
};

struct For
{
    char *iter;
    Expr *range;
    ExprVector body;
};

struct Return
{
    Expr *expr;
};

typedef enum ExprType
{
    FnDefExpr,
    ArgExpr,
    FnCallExpr,
    VarDefExpr,
    StructExpr,
    EnumExpr,
    VarUseExpr,
    OpExpr,
    ValueExpr,
    ArrayExpr,
    BlockExpr,
    IfExpr,
    ElseExpr,
    LoopExpr,
    WhileExpr,
    ForExpr,
    ReturnExpr,
    BreakExpr,
    ContinueExpr
} ExprType;

typedef union ExprUnion
{
    struct FnDef _fn_def;
    struct Arg _arg;
    struct FnCall _fn_call;
    struct VarDef _var_def;
    struct Struct _struct;
    struct Enum _enum;
    struct VarUse _var_use;
    struct Op _op;
    struct Value _value;
    struct Array _array;
    struct Block _block;
    struct If _if;
    struct Else _else;
    struct Loop _loop;
    struct While _while;
    struct For _for;
    struct Return _return;
} ExprUnion;

typedef struct Expr
{
    ExprType type;
    ExprUnion expr;
} Expr;

ExprVector NewExprVector(int capacity)
{
    ExprVector vector;
    vector.array = malloc(sizeof(Expr) * capacity);
    vector.size = 0;
    vector.capacity = capacity;
    return vector;
}

void PushExpr(ExprVector *vector, Expr expr)
{
    vector->array[vector->size++] = expr;
    if (vector->size == vector->capacity)
    {
        int capacity = vector->capacity *= 2;
        vector->array = realloc(vector->array, sizeof(Expr) * capacity);
    }
}

int NextTokenOfType(TokenVector tokens, int start, TokenType type, int end)
{
    for (int i = start; i < end; ++i)
    {
        if (tokens.array[i].type == type)
        {
            return i;
        }
    }
    return -1;
}

Expr *Parse(TokenVector tokens, int start, int end)
{
    int op_level = 0;
    int op_count = 0;
    int op_index;
    TokenType op_type;

    for (int i = start; i < end; ++i)
    {
        ++op_count;
        TokenType type = tokens.array[i].type;
        switch (type)
        {
        case Assign:
        case AddAssign:
        case SubstractAssign:
        case MultiplyAssign:
        case DivideAssign:
        case ModuloAssign:
            if (op_level < 6)
            {
                op_level = 6;
            }
            if (op_level == 6)
            {
                op_type = type;
                op_index = i;
            }
            break;
        case Equal:
        case NotEqual:
            if (op_level < 5)
            {
                op_level = 5;
            }
            if (op_level == 5)
            {
                op_type = type;
                op_index = i;
            }
            break;
        case Less:
        case LessEqual:
        case Greater:
        case GreaterEqual:
            if (op_level < 4)
            {
                op_level = 4;
            }
            if (op_level == 4)
            {
                op_type = type;
                op_index = i;
            }
            break;
        case Add:
        case Substract:
            if (op_level < 3)
            {
                op_level = 3;
            }
            if (op_level == 3)
            {
                op_type = type;
                op_index = i;
            }
            break;
        case Multiply:
        case Divide:
        case Modulo:
            if (op_level < 2)
            {
                op_level = 2;
            }
            if (op_level == 2)
            {
                op_type = type;
                op_index = i;
            }
            break;
        case Not:
            if (op_level == 0)
            {
                op_level = 1;
            }
            if (op_level == 1)
            {
                op_type = type;
                op_index = i;
            }
            break;
        default:
            --op_count;
            break;
        }
    }

    if (op_count == 0)
    {
        Expr *expr = malloc(sizeof(Expr));
        char *value = tokens.array[start].value;

        switch (tokens.array[start + 1].type)
        {
        case LeftParen:
            expr->type = FnCallExpr;
            expr->expr._fn_call.name = value;
            int paren = NextTokenOfType(tokens, start, RightParen, tokens.size);
            if (start + 2 == paren)
            {
                expr->expr._fn_call.args = NewExprVector(0);
            }
            else
            {
                expr->expr._fn_call.args = NewExprVector(2);
                int comma = NextTokenOfType(tokens, start, Comma, paren);
                int index = start + 2;
                while (comma != -1 && comma + 1 < paren)
                {
                    PushExpr(&expr->expr._fn_call.args, *Parse(tokens, index, comma));
                    index = comma + 1;
                    comma = NextTokenOfType(tokens, index, Comma, paren);
                }
                PushExpr(&expr->expr._fn_call.args, *Parse(tokens, index, paren));
            }
            return expr;
        default:
            if (*value == '"')
            {
                expr->type = ValueExpr;
                expr->expr._value.val = value;
                expr->expr._value.type = "string";
                return expr;
            }
            else if (isdigit(*value))
            {
                expr->type = ValueExpr;
                expr->expr._value.val = value;
                expr->expr._value.type = "i32"; // FIX: Infer the type
                return expr;
            }
            else
            {
                expr->type = VarUseExpr;
                expr->expr._var_use.name = value;
                return expr;
            }
            break;
        }
    }
    else
    {
        Expr *expr = malloc(sizeof(Expr));
        expr->type = OpExpr;
        expr->expr._op.type = op_type;
        expr->expr._op.lhs = Parse(tokens, start, op_index);
        expr->expr._op.rhs = Parse(tokens, op_index + 1, end);
        return expr;
    }
}

ExprVector ParseBlock(TokenVector tokens, int *index)
{
    ExprVector vector = NewExprVector(8);
    int expr_index = 0;
    int expr_state = 0;

    while (*index < tokens.size)
    {
        Token token = tokens.array[*index];

        if (expr_index == vector.size)
        {
            Expr expr;
            expr.type = -1;

            switch (token.type)
            {
            case RightBrace:
                return vector;
            case Function:
                expr.type = FnDefExpr;
                expr.expr._fn_def.args = NewExprVector(2);
                expr.expr._fn_def.type = NULL;
                break;
            case Ident:
            {
                int semicolon = NextTokenOfType(tokens, *index, Semicolon, tokens.size);
                Expr *temp = Parse(tokens, *index, semicolon);
                expr = *temp;
                free(temp);
                *index = semicolon;
                ++expr_index;
                break;
            }
            case Variable:
                expr.type = VarDefExpr;
                expr.expr._var_def.type = NULL;
                expr.expr._var_def.val = NULL;
                break;
            case Struct:
                expr.type = StructExpr;
                break;
            case Enum:
                expr.type = EnumExpr;
                break;
            case LeftBrace:
                expr.type = BlockExpr;
                ++*index;
                expr.expr._block.body = ParseBlock(tokens, index);
                ++expr_index;
                break;
            case If:
                expr.type = IfExpr;
                expr.expr._if.elif = 0;
                break;
            case Else:
                if (*index + 1 < tokens.size && tokens.array[*index + 1].type == If)
                {
                    expr.type = IfExpr;
                    expr.expr._if.elif = 1;
                    ++*index;
                }
                else
                {
                    expr.type = ElseExpr;
                }
                break;
            case Loop:
                expr.type = LoopExpr;
                break;
            case While:
                expr.type = WhileExpr;
                break;
            case For:
                expr.type = ForExpr;
                break;
            case Return:
                expr.type = ReturnExpr;
                int semicolon = NextTokenOfType(tokens, *index, Semicolon, tokens.size);
                if (semicolon == *index + 1)
                {
                    expr.expr._return.expr = NULL;
                }
                else
                {
                    expr.expr._return.expr = Parse(tokens, *index + 1, semicolon);
                }
                *index = semicolon;
                ++expr_index;
                break;
            case Break:
                expr.type = BreakExpr;
                break;
            case Continue:
                expr.type = ContinueExpr;
                break;
            }

            if (expr.type != -1)
            {
                PushExpr(&vector, expr);
                expr_state = 0;
            }
        }
        else
        {
            Expr *expr = vector.array + expr_index;

            switch (expr->type)
            {
            case FnDefExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.name = token.value;
                        ++expr_state;
                    }
                    break;
                case 1:
                    if (token.type == LeftParen)
                    {
                        ++expr_state;
                    }
                    break;
                case 2:
                    if (token.type == RightParen)
                    {
                        expr_state = 6;
                    }
                    else if (token.type == Ident)
                    {
                        Expr arg;
                        arg.type = ArgExpr;
                        arg.expr._arg.name = token.value;
                        PushExpr(&expr->expr._fn_def.args, arg);
                        ++expr_state;
                    }
                    break;
                case 3:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    break;
                case 4:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.args.array[expr->expr._fn_def.args.size - 1].expr._arg.type = token.value;
                        expr_state = 5;
                    }
                    break;
                case 5:
                    if (token.type == RightParen)
                    {
                        expr_state = 6;
                    }
                    else if (token.type == Comma)
                    {
                        expr_state = 2;
                    }
                    break;
                case 6:
                    if (token.type == LeftBrace)
                    {
                        expr_state = 8;
                    }
                    else if (expr->expr._fn_def.type == NULL && token.type == Colon)
                    {
                        ++expr_state;
                    }
                    break;
                case 7:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.type = token.value;
                        expr_state = 6;
                    }
                    break;
                case 8:
                    expr->expr._fn_def.body = ParseBlock(tokens, index);
                    ++expr_index;
                    break;
                }
                break;
            case VarDefExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._var_def.name = token.value;
                        ++expr_state;
                    }
                    break;
                case 1:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    else if (token.type == Assign)
                    {
                        int semicolon = NextTokenOfType(tokens, *index + 1, Semicolon, tokens.size);
                        expr->expr._var_def.val = Parse(tokens, *index + 1, semicolon);
                        expr->expr._var_def.type = "i32"; // FIX: Infer the type
                        *index = semicolon;
                        ++expr_index;
                    }
                    else if (token.type == Semicolon && expr->expr._var_def.type != NULL)
                    {
                        ++expr_index;
                    }
                    break;
                case 2:
                    if (token.type == Ident)
                    {
                        expr->expr._var_def.type = token.value;
                        expr_state = 1;
                    }
                    break;
                }
                break;
            case StructExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._struct.name = token.value;
                        ++expr_state;
                    }
                    break;
                case 1:
                    if (token.type == LeftBrace)
                    {
                        expr->expr._struct.body = NewExprVector(2);
                        ++expr_state;
                    }
                case 2:
                    if (token.type == RightBrace)
                    {
                        ++expr_index;
                    }
                    else if (token.type == Ident)
                    {
                        Expr var;
                        var.type = ArgExpr;
                        var.expr._arg.name = token.value;
                        PushExpr(&expr->expr._struct.body, var);
                        ++expr_state;
                    }
                    else if (token.type == Comma)
                    {
                    }
                    break;
                case 3:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    break;
                case 4:
                    if (token.type == Ident)
                    {
                        expr->expr._struct.body.array[expr->expr._struct.body.size - 1].expr._arg.type = token.value;
                        expr_state = 2;
                    }
                    break;
                }
                break;
            case EnumExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._enum.name = token.value;
                        ++expr_state;
                    }
                    break;
                case 1:
                    if (token.type == LeftBrace)
                    {
                        ++expr_state;
                    }
                    break;
                }
            case IfExpr:
            {
                int brace = NextTokenOfType(tokens, *index, LeftBrace, tokens.size);
                expr->expr._if.con = Parse(tokens, *index, brace);
                *index = brace + 1;
                expr->expr._if.body = ParseBlock(tokens, index);
                ++expr_index;
                break;
            }
            case ElseExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == LeftBrace)
                    {
                        ++expr_state;
                    }
                    break;
                case 1:
                    expr->expr._else.body = ParseBlock(tokens, index);
                    ++expr_index;
                    break;
                }
                break;
            case LoopExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == LeftBrace)
                    {
                        ++expr_state;
                    }
                    break;
                case 1:
                    expr->expr._loop.body = ParseBlock(tokens, index);
                    ++expr_index;
                    break;
                }
                break;
            case WhileExpr: // TODO
                break;
            case BreakExpr:
            case ContinueExpr:
                if (token.type == Semicolon)
                {
                    ++expr_index;
                }
                break;
            }
        }

        ++*index;
    }

    return vector;
}
#pragma endregion

#pragma region Generator
void GenerateType(char *type, FILE *fp)
{
    if (type == NULL)
    {
        fputs("void", fp);
    }
    else if (strcmp(type, "i32") == 0)
    {
        fputs("int32_t", fp);
    }
    else if (strcmp(type, "f32") == 0)
    {
        fputs("float", fp);
    }
    else if (strcmp(type, "i8") == 0)
    {
        fputs("int8_t", fp);
    }
    else if (strcmp(type, "f64") == 0)
    {
        fputs("double", fp);
    }
    else if (strcmp(type, "u32") == 0)
    {
        fputs("uint32_t", fp);
    }
    else if (strcmp(type, "i64") == 0)
    {
        fputs("int64_t", fp);
    }
    else if (strcmp(type, "i16") == 0)
    {
        fputs("int16_t", fp);
    }
    else if (strcmp(type, "u8") == 0)
    {
        fputs("uint8_t", fp);
    }
    else if (strcmp(type, "u64") == 0)
    {
        fputs("uint64_t", fp);
    }
    else if (strcmp(type, "u16") == 0)
    {
        fputs("uint16_t", fp);
    }
    else
    {
        fputs(type, fp);
    }
}

void GenerateVector(ExprVector vector, FILE *fp, char semicolon, char comma);

void GenerateExpr(Expr expr, FILE *fp, char last, char semicolon)
{
    switch (expr.type)
    {
    case FnDefExpr:
        GenerateType(expr.expr._fn_def.type, fp);
        fprintf(fp, " %s(", expr.expr._fn_def.name);
        GenerateVector(expr.expr._fn_def.args, fp, 0, 0);
        fputs("){", fp);
        GenerateVector(expr.expr._fn_def.body, fp, 1, 0);
        fputs("}", fp);
        break;
    case ArgExpr:
        GenerateType(expr.expr._arg.type, fp);
        fprintf(fp, " %s", expr.expr._arg.name);
        if (semicolon)
        {
            fputc(';', fp);
        }
        else if (!last)
        {
            fputc(',', fp);
        }
        break;
    case FnCallExpr:
        fprintf(fp, "%s(", expr.expr._fn_call.name);
        GenerateVector(expr.expr._fn_call.args, fp, 0, 1);
        fputc(')', fp);
        if (semicolon)
        {
            fputc(';', fp);
        }
        break;
    case VarDefExpr:
        GenerateType(expr.expr._var_def.type, fp);
        fprintf(fp, " %s", expr.expr._var_def.name);
        if (expr.expr._var_def.val != NULL)
        {
            fputc('=', fp);
            GenerateExpr(*expr.expr._var_def.val, fp, 0, 1);
        }
        else
        {
            fputc(';', fp);
        }
        break;
    case StructExpr:
        fprintf(fp, "typedef struct %s{", expr.expr._struct.name);
        GenerateVector(expr.expr._struct.body, fp, 1, 0);
        fprintf(fp, "}%s;", expr.expr._struct.name);
        break;
    case VarUseExpr:
        fputs(expr.expr._var_use.name, fp);
        if (semicolon)
        {
            fputc(';', fp);
        }
        break;
    case OpExpr:
        fputc('(', fp);
        GenerateExpr(*expr.expr._op.lhs, fp, 0, 0);
        switch (expr.expr._op.type)
        {
        case Assign:
            fputc('=', fp);
            break;
        case Equal:
            fputs("==", fp);
            break;
        case Less:
            fputc('<', fp);
            break;
        case LessEqual:
            fputs("<=", fp);
            break;
        case Greater:
            fputc('>', fp);
            break;
        case GreaterEqual:
            fputs(">=", fp);
            break;
        case Add:
            fputc('+', fp);
            break;
        case AddAssign:
            fputs("+=", fp);
            break;
        case Substract:
            fputc('-', fp);
            break;
        case SubstractAssign:
            fputs("-=", fp);
            break;
        case Multiply:
            fputc('*', fp);
            break;
        case MultiplyAssign:
            fputs("*=", fp);
            break;
        case Divide:
            fputc('/', fp);
            break;
        case DivideAssign:
            fputs("/=", fp);
            break;
        case Modulo:
            fputc('%', fp);
            break;
        case ModuloAssign:
            fputs("%=", fp);
            break;
        case Not:
            fputc('!', fp);
            break;
        case NotEqual:
            fputs("!=", fp);
            break;
        }
        GenerateExpr(*expr.expr._op.rhs, fp, 0, 0);
        fputc(')', fp);
        if (semicolon)
        {
            fputc(';', fp);
        }
        break;
    case ValueExpr:
        fputs(expr.expr._value.val, fp);
        if (semicolon)
        {
            fputc(';', fp);
        }
        break;
    case ArrayExpr:
        fputs("array ", fp);
        GenerateVector(expr.expr._array.elems, fp, 0, 0);
        break;
    case BlockExpr:
        fputc('{', fp);
        GenerateVector(expr.expr._block.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case IfExpr:
        if (expr.expr._if.elif)
        {
            fputs("else ", fp);
        }
        fputs("if(", fp);
        GenerateExpr(*expr.expr._if.con, fp, 0, 0);
        fputs("){", fp);
        GenerateVector(expr.expr._if.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case ElseExpr:
        fputs("else{", fp);
        GenerateVector(expr.expr._else.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case LoopExpr:
        fputs("while(1){", fp);
        GenerateVector(expr.expr._loop.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case WhileExpr:
        fputs("while(", fp);
        GenerateExpr(*expr.expr._while.con, fp, 0, 0);
        fputs("){", fp);
        GenerateVector(expr.expr._while.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case ForExpr:
        fputs("for ", fp);
        GenerateVector(expr.expr._for.body, fp, 1, 0);
        break;
    case ReturnExpr:
        fputs("return", fp);
        if (expr.expr._return.expr != NULL)
        {
            fputc(' ', fp);
            GenerateExpr(*expr.expr._return.expr, fp, 0, 1);
        }
        else
        {
            fputc(';', fp);
        }
        break;
    case BreakExpr:
        fputs("break;", fp);
        break;
    case ContinueExpr:
        fputs("continue;", fp);
        break;
    }
}

void GenerateVector(ExprVector vector, FILE *fp, char semicolon, char comma)
{
    for (int i = 0; i < vector.size; ++i)
    {
        char last = i + 1 == vector.size;
        GenerateExpr(vector.array[i], fp, last, semicolon);
        if (comma && !last)
        {
            fputc(',', fp);
        }
    }
}
#pragma endregion

void FreeExprVector(ExprVector vector);

void FreeExpr(Expr *expr)
{
    if (expr == NULL)
    {
        return;
    }

    // FIX: free the strings
    switch (expr->type)
    {
    case FnDefExpr:
        // free(expr->expr._fn_def.name);
        // free(expr->expr._fn_def.type);
        FreeExprVector(expr->expr._fn_def.args);
        FreeExprVector(expr->expr._fn_def.body);
        break;
    case ArgExpr:
        // free(expr->expr._arg.name);
        // free(expr->expr._arg.type);
        break;
    case ReturnExpr:
        FreeExpr(expr->expr._return.expr);
        break;
    case FnCallExpr:
        // free(expr->expr._fn_call.name);
        FreeExprVector(expr->expr._fn_call.args);
        break;
    case VarDefExpr:
        // free(expr->expr._var_def.name);
        // free(expr->expr._var_def.type);
        FreeExpr(expr->expr._var_def.val);
        break;
    case VarUseExpr:
        // free(expr->expr._var_use.name);
        break;
    case OpExpr:
        FreeExpr(expr->expr._op.lhs);
        FreeExpr(expr->expr._op.rhs);
        break;
    case ValueExpr:
        // free(expr->expr._value.type);
        // free(expr->expr._value.val);
        break;
    case ArrayExpr:
        FreeExprVector(expr->expr._array.elems);
        break;
    case BlockExpr:
        FreeExprVector(expr->expr._block.body);
        break;
    case IfExpr:
        FreeExpr(expr->expr._if.con);
        FreeExprVector(expr->expr._if.body);
        break;
    case ElseExpr:
        FreeExprVector(expr->expr._else.body);
        break;
    case LoopExpr:
        FreeExprVector(expr->expr._loop.body);
        break;
    case WhileExpr:
        FreeExpr(expr->expr._while.con);
        FreeExprVector(expr->expr._while.body);
        break;
    case ForExpr:
        // free(expr->expr._for.iter);
        FreeExpr(expr->expr._for.range);
        FreeExprVector(expr->expr._for.body);
        break;
    }
}

void FreeExprVector(ExprVector vector)
{
    for (int i = 0; i < vector.size; ++i)
    {
        FreeExpr(&vector.array[i]);
    }

    free(vector.array);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Error: no input file");
        return 1;
    }

    char help = 0;
    char *input = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0)
        {
            help = 1;
        }
        else
        {
            input = argv[i];
        }
    }

    // Print help informations
    if (help)
    {
        printf("Usage: cachec [OPTIONS] INPUT\n\nOptions:\n    -h, --help          Display available options\n");
        return 0;
    }

    // Open the file
    FILE *filepoint;
    if (fopen_s(&filepoint, input, "rb"))
    {
        printf("Error: no such file or directory: '%s'", input);
        return 1;
    }

    // Get the size of the file
    fseek(filepoint, 0L, SEEK_END);
    int file_size = ftell(filepoint);
    rewind(filepoint);

    // Allocate the buffer, read contents and close the file
    char *buffer = malloc(file_size);
    fread(buffer, file_size, 1, filepoint);
    fclose(filepoint);

    // Tokenize the file
    TokenVector tokens = Tokenize(file_size, buffer);

    // Parse the tokens
    int index = 0;
    ExprVector ast = ParseBlock(tokens, &index);

    // Generate output file
    fopen_s(&filepoint, "main.c", "w");
    fputs("#include <stdint.h>\n", filepoint);
    GenerateVector(ast, filepoint, 0, 0);
    fclose(filepoint);

    // Free the memory
    free(buffer);
    for (int i = 0; i < tokens.size; ++i)
    {
        free(tokens.array[i].value);
    }
    free(tokens.array);
    FreeExprVector(ast);

    return 0;
}
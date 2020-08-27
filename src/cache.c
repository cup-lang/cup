#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
HANDLE console;
#endif

#define VECTOR(n, t)     \
    VECTOR_STRUCT(n, t); \
    VECTOR_FUNC(n, t);

#define VECTOR_STRUCT(n, t)  \
    typedef struct n##Vector \
    {                        \
        t *array;            \
        int size;            \
        int capacity;        \
    } n##Vector;

#define VECTOR_FUNC(n, t)                                \
    n##Vector New##n##Vector(int c)                      \
    {                                                    \
        n##Vector v;                                     \
        v.array = malloc(sizeof(t) * c);                 \
        v.size = 0;                                      \
        v.capacity = c;                                  \
        return v;                                        \
    };                                                   \
    void Push##n(n##Vector *v, t o)                      \
    {                                                    \
        v->array[v->size++] = o;                         \
        if (v->size == v->capacity)                      \
        {                                                \
            int c = v->capacity *= 2;                    \
            v->array = realloc(v->array, sizeof(t) * c); \
        }                                                \
    };

VECTOR(Char, char);
VECTOR(String, CharVector);

char *file_name = NULL;
int file_size;
char *file;

#pragma region Lexer
typedef enum _TokenType
{
    Ident,
    Public, // TODO
    Module,
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
    Elif,
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
} _TokenType;

typedef struct Token
{
    _TokenType type;
    char *value;
    int index;
} Token;

VECTOR(Token, Token);

Token IdentToken(CharVector *vector, int index)
{
    Token token;
    token.type = Ident;
    token.index = index;
    token.value = malloc(vector->size);
    memcpy(token.value, vector->array, vector->size);
    token.value[vector->size] = '\0';
    vector->size = 0;
    return token;
}

TokenVector Tokenize()
{
    TokenVector tokens = NewTokenVector(file_size / 4);
    CharVector value = NewCharVector(32);
    int index;
    char is_comment = 0;

    // Loop through all characters and create tokens
    for (int i = 0; i < file_size; ++i)
    {
        char c = file[i];

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
                PushToken(&tokens, IdentToken(&value, index));
            }
        }
        else
        {
            Token token;
            token.index = i;
            token.type = -1;
            token.value = NULL;

            // Check assign
            char is_assign = i + 1 < file_size && file[i + 1] == '=';

            switch (c)
            {
            case '#':
                is_comment = 1;
                continue;
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
                token.type = Divide + is_assign;
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
                if (i + 1 < file_size && file[i + 1] == '.')
                {
                    token.type = Range;
                    i += 1;
                }
                break;
            case 'm':
                if (i + 3 < file_size && file[i + 1] == 'o' && file[i + 2] == 'd' && isspace(file[i + 3]))
                {
                    token.type = Module;
                    i += 3;
                }
                break;
            case 'v':
                if (i + 3 < file_size && file[i + 1] == 'a' && file[i + 2] == 'r' && isspace(file[i + 3]))
                {
                    token.type = Variable;
                    i += 3;
                }
                break;
            case 's':
                if (i + 6 < file_size && file[i + 1] == 't' && file[i + 2] == 'r' && file[i + 3] == 'u' && file[i + 4] == 'c' && file[i + 5] == 't' && isspace(file[i + 6]))
                {
                    token.type = Struct;
                    i += 6;
                }
                break;
            case 'i':
                if (i + 2 < file_size && file[i + 1] == 'f' && isspace(file[i + 2]))
                {
                    token.type = If;
                    i += 2;
                }
                break;
            case 'e':
                if (i + 4 < file_size && file[i + 1] == 'n' && file[i + 2] == 'u' && file[i + 3] == 'm' && isspace(file[i + 4]))
                {
                    token.type = Enum;
                    i += 4;
                }
                else if (i + 4 < file_size && file[i + 1] == 'l' && file[i + 2] == 'i' && file[i + 3] == 'f' && isspace(file[i + 4]))
                {
                    token.type = Elif;
                    i += 4;
                }
                else if (i + 4 < file_size && file[i + 1] == 'l' && file[i + 2] == 's' && file[i + 3] == 'e' && isspace(file[i + 4]))
                {
                    token.type = Else;
                    i += 4;
                }
                break;
            case 'l':
                if (i + 4 < file_size && file[i + 1] == 'o' && file[i + 2] == 'o' && file[i + 3] == 'p' && (isspace(file[i + 4]) || file[i + 4] == '{'))
                {
                    token.type = Loop;
                    i += 3;
                }
                break;
            case 'w':
                if (i + 5 < file_size && file[i + 1] == 'h' && file[i + 2] == 'i' && file[i + 3] == 'l' && file[i + 4] == 'e' && isspace(file[i + 5]))
                {
                    token.type = While;
                    i += 5;
                }
                break;
            case 'f':
                if (i + 2 < file_size && file[i + 1] == 'n' && isspace(file[i + 2]))
                {
                    token.type = Function;
                    i += 2;
                }
                break;
            case 'r':
                if (i + 6 < file_size && file[i + 1] == 'e' && file[i + 2] == 't' && file[i + 3] == 'u' && file[i + 4] == 'r' && file[i + 5] == 'n' && (isspace(file[i + 6]) || file[i + 6] == ';'))
                {
                    token.type = Return;
                    i += 5;
                }
                break;
            case 'b':
                if (i + 5 < file_size && file[i + 1] == 'r' && file[i + 2] == 'e' && file[i + 3] == 'a' && file[i + 4] == 'k' && (isspace(file[i + 5]) || file[i + 5] == ';'))
                {
                    token.type = Break;
                    i += 4;
                }
                break;
            case 'c':
                if (i + 8 < file_size && file[i + 1] == 'o' && file[i + 2] == 'n' && file[i + 3] == 't' && file[i + 4] == 'i' && file[i + 5] == 'n' && file[i + 6] == 'u' && file[i + 7] == 'e' && (isspace(file[i + 8]) || file[i + 8] == ';'))
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
                    PushToken(&tokens, IdentToken(&value, index));
                }
                PushToken(&tokens, token);
            }
            // Update value
            else if (!isspace(c))
            {
                if (value.size == 0)
                {
                    index = i;
                }
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

VECTOR_STRUCT(Expr, Expr);

struct Mod
{
    char *name;
    ExprVector body;
};

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
    ExprVector body;
};

struct VarUse
{
    char *name;
};

struct Op
{
    _TokenType type;
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
    Expr *con;
    ExprVector body;
};

struct Elif
{
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
    ModExpr,
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
    ElifExpr,
    ElseExpr,
    LoopExpr,
    WhileExpr,
    ForExpr,
    ReturnExpr,
    BreakExpr,
    ContinueExpr
} ExprType;

typedef union ExprUnion {
    struct Mod _mod;
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
    struct Elif _elif;
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

VECTOR_FUNC(Expr, Expr);

int NextTokenOfType(TokenVector tokens, int start, _TokenType type, int end)
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

typedef struct Location
{
    int line;
    int column;
} Location;

Location GetLocation(int index)
{
    Location location;
    location.line = 1;
    location.column = 1;
    for (int i = 0; i < index; ++i)
    {
        if (file[i] == '\n')
        {
            ++location.line;
            location.column = 1;
        }
        else
        {
            ++location.column;
        }
    }
    return location;
}

#ifdef _WIN32
#define COLOR(c) SetConsoleTextAttribute(console, c);
#define RESET 7
#define RED 12
#else
#define COLOR(c) printf(c);
#define RESET "\033[0m;"
#define RED “\033[0;31m”
#endif

void PrintSnippet(Location location)
{
    printf(" %i | ", location.line);
    int i = location.line;
    int length = 2;
    while (i)
    {
        ++length;
        i /= 10;
    }
    int line_index = 1;
    for (int i = 0; i < file_size; ++i)
    {
        if (file[i] == '\n')
        {
            if (++line_index > location.line)
            {
                break;
            }
        }
        else if (line_index == location.line)
        {
            printf("%c", file[i]);
        }
    }
    printf("\n");
    for (int i = 0; i < length; ++i)
    {
        printf(" ");
    }
    printf("|");
    for (int i = 0; i < location.column; ++i)
    {
        printf(" ");
    }
    COLOR(RED);
    printf("^");
    COLOR(RESET);
}

#define EXPECTED(e) \
    else { THROW("expected " e) }
#define THROW(e)                                                         \
    {                                                                    \
        Location location = GetLocation(token.index);                    \
        printf("%s:%i:%i: ", file_name, location.line, location.column); \
        COLOR(RED);                                                      \
        printf("error: ");                                               \
        SetConsoleTextAttribute(console, 7);                             \
        printf(e "\n", token.value);                                     \
        PrintSnippet(location);                                          \
        exit(1);                                                         \
    }

VECTOR(Var, struct Arg);

typedef struct VarStack VarStack;

typedef struct VarStack
{
    VarVector vars;
    VarStack *before;
} VarStack;

VarStack *NewVarStack()
{
    VarStack *stack = malloc(sizeof(VarStack));
    stack->vars = NewVarVector(2);
    stack->before = NULL;
    return stack;
}

VarStack *var_stack = NULL;

char VariableDefined(VarStack stack, char *name)
{
    for (int i = 0; i < stack.vars.size; ++i)
    {
        if (strcmp(stack.vars.array[i].name, name) == 0)
        {
            return 1;
        }
    }
    if (stack.before != NULL && VariableDefined(*stack.before, name))
    {
        return 1;
    }
    return 0;
}

Expr *Parse(TokenVector tokens, int start, int end)
{
    int op_level = 0;
    int op_count = 0;
    int op_index;
    _TokenType op_type;

    for (int i = start; i < end; ++i)
    {
        ++op_count;
        _TokenType type = tokens.array[i].type;
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
        Token token = tokens.array[start];

        if (tokens.array[start].type != Ident)
        {
            THROW("expected identifier");
        }

        switch (tokens.array[start + 1].type)
        {
        case LeftParen:
            expr->type = FnCallExpr;
            expr->expr._fn_call.name = token.value;
            int paren = NextTokenOfType(tokens, start, RightParen, tokens.size);
            if (paren == -1)
            {
                THROW("expected ')' after '('");
            }
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
            if (*token.value == '"')
            {
                expr->type = ValueExpr;
                expr->expr._value.val = token.value;
                expr->expr._value.type = "string";
                return expr;
            }
            else if (isdigit(*token.value))
            {
                expr->type = ValueExpr;
                expr->expr._value.val = token.value;
                expr->expr._value.type = "i32"; // FIX: Infer the type
                return expr;
            }
            else
            {
                if (!VariableDefined(*var_stack, token.value))
                {
                    THROW("variable '%s' is undefined");
                }
                expr->type = VarUseExpr;
                expr->expr._var_use.name = token.value;
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
    VarStack *old = var_stack;
    var_stack = NewVarStack();
    var_stack->before = old;

    char global_scope = *index == 0;

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
            {
                VarStack *old = var_stack;
                var_stack = var_stack->before;
                free(old);
                return vector;
            }
            case Module:
                expr.type = ModExpr;
                break;
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
                break;
            case Elif:
                expr.type = ElifExpr;
                break;
            case Else:
                expr.type = ElseExpr;
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
            case ModExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._mod.name = token.value;
                        ++expr_state;
                    }
                    EXPECTED("module name after 'mod'");
                    break;
                case 1:
                    if (token.type == LeftBrace)
                    {
                        ++expr_state;
                    }
                    EXPECTED("'{' after 'mod' name");
                    break;
                case 2:
                    expr->expr._mod.body = ParseBlock(tokens, index);
                    ++expr_index;
                    break;
                }
                break;
            case FnDefExpr:
                switch (expr_state)
                {
                case 0:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.name = token.value;
                        ++expr_state;
                    }
                    EXPECTED("function name after 'fn'");
                    break;
                case 1:
                    if (token.type == LeftParen)
                    {
                        ++expr_state;
                    }
                    EXPECTED("'(' after function name");
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
                    EXPECTED("argument name or ')' after '('");
                    break;
                case 3:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    EXPECTED("':' after argument name");
                    break;
                case 4:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.args.array[expr->expr._fn_def.args.size - 1].expr._arg.type = token.value;
                        expr_state = 5;
                    }
                    EXPECTED("argument type after ':'");
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
                    EXPECTED("',' or ')' after argument type");
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
                    EXPECTED("':' or '{' after ')'");
                    break;
                case 7:
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.type = token.value;
                        expr_state = 6;
                    }
                    EXPECTED("funtion return type after ':'");
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
                        struct Arg var;
                        var.name = token.value;
                        PushVar(&var_stack->vars, var);
                        ++expr_state;
                    }
                    EXPECTED("variable name after 'var'");
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
                        if (expr->expr._var_def.type == NULL)
                        {
                            expr->expr._var_def.type = "i32"; // FIX: Infer the type
                        }
                        *index = semicolon;
                        ++expr_index;
                    }
                    else if (token.type == Semicolon && expr->expr._var_def.type != NULL)
                    {
                        ++expr_index;
                    }
                    EXPECTED("':' or '=' after variable name");
                    break;
                case 2:
                    if (token.type == Ident)
                    {
                        expr->expr._var_def.type = token.value;
                        expr_state = 1;
                    }
                    EXPECTED("variable type after ':'");
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
                    EXPECTED("struct name after 'struct'");
                    break;
                case 1:
                    if (token.type == LeftBrace)
                    {
                        expr->expr._struct.body = NewExprVector(2);
                        expr_state = 3;
                    }
                    EXPECTED("'{' after struct name");
                    break;
                case 2:
                    if (token.type == Comma)
                    {
                        ++expr_state;
                    }
                    else if (token.type == RightBrace)
                    {
                        ++expr_index;
                    }
                    EXPECTED("',' or '}' after variable type");
                    break;
                case 3:
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
                    EXPECTED("field name or '}' after '{'");
                    break;
                case 4:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    EXPECTED("':' after variable name");
                    break;
                case 5:
                    if (token.type == Ident)
                    {
                        expr->expr._struct.body.array[expr->expr._struct.body.size - 1].expr._arg.type = token.value;
                        expr_state = 2;
                    }
                    EXPECTED("field type after ':'");
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
                    EXPECTED("enum name after 'enum'");
                    break;
                case 1:
                    if (token.type == LeftBrace)
                    {
                        expr->expr._enum.body = NewExprVector(2);
                        expr_state = 3;
                    }
                    EXPECTED("'{' after enum name");
                    break;
                case 2:
                    if (token.type == Comma)
                    {
                        ++expr_state;
                    }
                    else if (token.type == RightBrace)
                    {
                        ++expr_index;
                    }
                    EXPECTED("'}' or ',' after '{'");
                    break;
                case 3:
                    if (token.type == RightBrace)
                    {
                        ++expr_index;
                    }
                    else if (token.type == Ident)
                    {
                        Expr opt;
                        opt.type = StructExpr;
                        opt.expr._struct.name = token.value;
                        opt.expr._struct.body = NewExprVector(2);
                        PushExpr(&expr->expr._enum.body, opt);
                        ++expr_state;
                    }
                    EXPECTED("option name or '}' after '{'");
                    break;
                case 4:
                    if (token.type == LeftParen)
                    {
                        expr_state = 6;
                    }
                    else if (token.type == Comma)
                    {
                        expr_state = 3;
                    }
                    else if (token.type == RightBrace)
                    {
                        ++expr_index;
                    }
                    EXPECTED("'(' or ',' after option name");
                    break;
                case 5:
                    if (token.type == Comma)
                    {
                        ++expr_state;
                    }
                    else if (token.type == RightParen)
                    {
                        expr_state = 2;
                    }
                    EXPECTED("',' or ')' after field type");
                    break;
                case 6:
                    if (token.type == RightParen)
                    {
                        expr_state = 2;
                    }
                    else if (token.type == Ident)
                    {
                        Expr arg;
                        arg.type = ArgExpr;
                        arg.expr._arg.name = token.value;
                        PushExpr(&expr->expr._enum.body.array[expr->expr._enum.body.size - 1].expr._struct.body, arg);
                        ++expr_state;
                    }
                    EXPECTED("field name or ')' after '('");
                    break;
                case 7:
                    if (token.type == Colon)
                    {
                        ++expr_state;
                    }
                    EXPECTED("':' after field name");
                    break;
                case 8:
                    if (token.type == Ident)
                    {
                        ExprVector vector = expr->expr._enum.body.array[expr->expr._enum.body.size - 1].expr._struct.body;
                        vector.array[vector.size - 1].expr._arg.type = token.value;
                        expr_state = 5;
                    }
                    EXPECTED("field type after ':'");
                    break;
                }
                break;
            case IfExpr:
            case ElifExpr:
            case WhileExpr:
            {
                int brace = NextTokenOfType(tokens, *index, LeftBrace, tokens.size);
                if (brace != -1)
                {
                    expr->expr._if.con = Parse(tokens, *index, brace);
                    *index = brace + 1;
                    expr->expr._if.body = ParseBlock(tokens, index);
                    ++expr_index;
                }
                else
                {
                    switch (expr->type)
                    {
                    case IfExpr:
                        THROW("expected '{' after 'if'");
                        break;
                    case ElifExpr:
                        THROW("expected '{' after 'elif'");
                        break;
                    case WhileExpr:
                        THROW("expected '{' after 'while'");
                        break;
                    }
                }
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
                    EXPECTED("'{' after 'else'");
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
                    EXPECTED("'{' after 'loop'");
                    break;
                case 1:
                    expr->expr._loop.body = ParseBlock(tokens, index);
                    ++expr_index;
                    break;
                }
                break;
            case BreakExpr:
                if (token.type == Semicolon)
                {
                    ++expr_index;
                }
                EXPECTED("';' after 'break'");
                break;
            case ContinueExpr:
                if (token.type == Semicolon)
                {
                    ++expr_index;
                }
                EXPECTED("';' after 'continue'");
                break;
            }
        }

        ++*index;
    }

    if (global_scope)
    {
        free(var_stack);
        return vector;
    }

    Token token = tokens.array[tokens.size - 1];
    THROW("expected '}' token");
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

void GenerateExpr(Expr expr, FILE *fp, char last, char semicolon, char parenths)
{
    switch (expr.type)
    {
    case ModExpr:
        GenerateVector(expr.expr._mod.body, fp, 0, 0);
        break;
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
            GenerateExpr(*expr.expr._var_def.val, fp, 0, 1, 0);
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
    case EnumExpr:
        GenerateVector(expr.expr._enum.body, fp, 1, 0);
        fprintf(fp, "typedef union %sUnion{", expr.expr._enum.name);
        for (int i = 0; i < expr.expr._enum.body.size; ++i)
        {
            fprintf(fp, "struct %s _%i;", expr.expr._enum.body.array[i].expr._struct.name, i);
        }
        fprintf(fp, "}%sUnion;", expr.expr._enum.name);
        fprintf(fp, "typedef struct %s{int type;%sUnion u;}%s;", expr.expr._enum.name, expr.expr._enum.name, expr.expr._enum.name);
        break;
    case VarUseExpr:
        fputs(expr.expr._var_use.name, fp);
        if (semicolon)
        {
            fputc(';', fp);
        }
        break;
    case OpExpr:
        if (parenths)
        {
            fputc('(', fp);
        }
        GenerateExpr(*expr.expr._op.lhs, fp, 0, 0, parenths + 1);
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
        GenerateExpr(*expr.expr._op.rhs, fp, 0, 0, parenths + 1);
        if (parenths)
        {
            fputc(')', fp);
        }
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
    case ElifExpr:
        fputs("else ", fp);
        expr.type = IfExpr;
    case IfExpr:
        fputs("if(", fp);
        GenerateExpr(*expr.expr._if.con, fp, 0, 0, 0);
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
        fputs("for(;;){", fp);
        GenerateVector(expr.expr._loop.body, fp, 1, 0);
        fputc('}', fp);
        break;
    case WhileExpr:
        fputs("while(", fp);
        GenerateExpr(*expr.expr._while.con, fp, 0, 0, 0);
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
            GenerateExpr(*expr.expr._return.expr, fp, 0, 1, 0);
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
        GenerateExpr(vector.array[i], fp, last, semicolon, 0);
        if (comma && !last)
        {
            fputc(',', fp);
        }
    }
}
#pragma endregion

int main(int argc, char **argv)
{
#ifdef _WIN32
    console = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    if (argc == 1)
    {
        printf("Error: no input file specified");
        return 1;
    }

    char help = 0;
    char *output = NULL;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0)
        {
            help = 1;
        }
        else if (strcmp("-o", argv[i]) == 0 || strcmp("--output", argv[i]) == 0)
        {
            if (i + 1 < argc)
            {
                output = argv[++i];
            }
            else
            {
                printf("Error: no output file specified");
                return 1;
            }
        }
        else if (file_name == NULL)
        {
            file_name = argv[i];
        }
        else
        {
            printf("Error: invalid argument '%s'", argv[i]);
        }
    }

    // Print help informations
    if (help)
    {
        printf("Usage: cache [OPTIONS] INPUT\n\nOptions:\n    -h, --help          Display available options\n    -o, --output        Specify the output file name");
        return 0;
    }

    // Open the file
    FILE *file_point;
    if (fopen_s(&file_point, file_name, "rb"))
    {
        printf("Error: no such file or directory: '%s'", file_name);
        return 1;
    }

    // Get the size of the file
    fseek(file_point, 0L, SEEK_END);
    file_size = ftell(file_point);
    rewind(file_point);

    // Allocate the buffer, read contents and close the file
    file = malloc(file_size);
    fread(file, file_size, 1, file_point);
    fclose(file_point);

    // Tokenize the file
    TokenVector tokens = Tokenize();

    // Parse the tokens
    int index = 0;
    ExprVector ast = ParseBlock(tokens, &index);

    // Generate output file
    if (output != NULL)
    {
        fopen_s(&file_point, output, "w");
    }
    else
    {
        fopen_s(&file_point, "main.c", "w");
    }
    fputs("#include <stdint.h>\n", file_point);
    GenerateVector(ast, file_point, 0, 0);
    fclose(file_point);

    printf("Compilation successful in %fs\n", (double)clock() / CLOCKS_PER_SEC);

    return 0;
}
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
    AddEqual,
    Substract,
    SubstractEqual,
    Multiply,
    MultiplyEqual,
    Divide,
    DivideEqual,
    Modulo,
    ModuloEqual,
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
    ExprVector args;
    ExprVector body;
};

struct Arg
{
    char *name;
    char *type;
};

struct Return
{
    Expr *expr;
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

struct VarUse
{
    char *name;
};

struct Op
{
    int type;
    Expr *lhs;
    Expr *rshs;
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
    Expr *cond;
    ExprVector body;
};

struct For
{
    char *iter;
    Expr *range;
    ExprVector body;
};

typedef enum ExprType
{
    FnDefExpr,
    ArgExpr,
    ReturnExpr,
    FnCallExpr,
    VarDefExpr,
    VarUseExpr,
    OpExpr,
    ValueExpr,
    ArrayExpr,
    IfExpr,
    ElifExpr,
    ElseExpr,
    LoopExpr,
    WhileExpr,
    ForExpr
} ExprType;

typedef union ExprUnion
{
    struct FnDef _fn_def;
    struct Arg _arg;
    struct Return _return;
    struct FnCall _fn_call;
    struct VarDef _var_def;
    struct VarUse _var_use;
    struct Op _op;
    struct Value _value;
    struct Array _array;
    struct If _if;
    struct Elif _elif;
    struct Else _else;
    struct Loop _loop;
    struct While _while;
    struct For _for;
} ExprUnion;

typedef struct Expr
{
    ExprType type;
    ExprUnion expr;
    int state;
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

ExprVector EvaluteTokens(TokenVector tokens, int *index, TokenType ending)
{
    ExprVector vector = NewExprVector(8);

    int expr_index = 0;

    while (*index < tokens.size)
    {
        Token token = tokens.array[*index];

        if (expr_index == vector.size)
        {
            if (token.type == ending)
            {
                return vector;
            }

            Expr expr;
            expr.type = -1;

            switch (token.type)
            {
            case Function:
                expr.type = FnDefExpr;
                break;
            case Ident:
                if (*index + 1 < tokens.size)
                {
                    switch (tokens.array[*index + 1].type)
                    {
                    // Argument declaration
                    case Colon:
                        expr.type = ArgExpr;
                        expr.expr._arg.name = token.value;
                        ++*index;
                        break;
                    // Function call
                    case LeftParen:
                        expr.type = FnCallExpr;
                        expr.expr._fn_call.name = token.value;
                        break;
                    default:
                        // Value
                        if (token.value[0] == '"')
                        {
                            expr.type = ValueExpr;
                            expr.expr._value.type = "string";
                            expr.expr._value.val = token.value;
                            ++expr_index;
                        }
                        else if (isdigit(token.value[0]))
                        {
                            expr.type = ValueExpr;
                            expr.expr._value.type = "i32"; // FIX: Evaluate the type
                            expr.expr._value.val = token.value;
                            ++expr_index;
                        }
                        // Variable use
                        else
                        {
                            expr.type = VarUseExpr;
                            expr.expr._var_use.name = token.value;
                            ++expr_index;
                        }
                        break;
                    }
                }
                break;
            case Variable:
                expr.type = VarDefExpr;
                expr.expr._var_def.type = "i32"; // FIX: Evaluate the type
                break;
            }

            // Add the new Expr
            if (expr.type != -1)
            {
                expr.state = 0;
                PushExpr(&vector, expr);
            }
        }
        else
        {
            Expr *expr = &vector.array[expr_index];

            switch (expr->type)
            {
            case FnDefExpr:
                switch (expr->state)
                {
                case 0:
                    // Set function name
                    if (token.type == Ident)
                    {
                        expr->expr._fn_def.name = token.value;
                        ++expr->state;
                    }
                    break;
                case 1:
                    // Get arguments
                    if (token.type == LeftParen)
                    {
                        expr->expr._fn_def.args = EvaluteTokens(tokens, index, RightParen);
                        ++expr->state;
                    }
                    break;
                case 2:
                    // Block start
                    if (token.type == LeftBrace)
                    {
                        expr->expr._fn_def.body = EvaluteTokens(tokens, index, RightBrace);
                        ++expr_index;
                    }
                    break;
                }
                break;
            case ArgExpr:
                // Set argument type
                if (token.type == Ident)
                {
                    expr->expr._arg.type = token.value;
                    ++expr_index;
                }
                break;
            case FnCallExpr:
                expr->expr._fn_call.args = EvaluteTokens(tokens, index, RightParen);
                ++expr_index;
                break;
            case VarDefExpr:
                switch (expr->state)
                {
                case 0:
                    // Set variable name
                    if (token.type == Ident)
                    {
                        expr->expr._var_def.name = token.value;
                        ++expr->state;
                    }
                    break;
                case 1:
                    // Assign
                    if (token.type == Assign)
                    {
                        ++expr->state;
                    }
                    break;
                case 2:
                    // Set variable value
                    expr->expr._var_def.val = &EvaluteTokens(tokens, index, Semicolon).array[0];
                    ++expr_index;
                    break;
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
    if (strcmp(type, "i32") == 0)
    {
        fputs("long", fp);
    }
    else if (strcmp(type, "f32") == 0)
    {
        fputs("float", fp);
    }
    else if (strcmp(type, "i8") == 0)
    {
        fputs("signed char", fp);
    }
    else if (strcmp(type, "f64") == 0)
    {
        fputs("double", fp);
    }
    else if (strcmp(type, "u32") == 0)
    {
        fputs("unsigned long", fp);
    }
    else if (strcmp(type, "i64") == 0)
    {
        fputs("long long", fp);
    }
    else if (strcmp(type, "i16") == 0)
    {
        fputs("int", fp);
    }
    else if (strcmp(type, "u8") == 0)
    {
        fputs("unsigned char", fp);
    }
    else if (strcmp(type, "u64") == 0)
    {
        fputs("unsigned long long", fp);
    }
    else if (strcmp(type, "u16") == 0)
    {
        fputs("unsigned int", fp);
    }
    else if (strcmp(type, "i128") == 0)
    {
        fputs("__int128", fp);
    }
    else if (strcmp(type, "u128") == 0)
    {
        fputs("unsigned __int128 ", fp);
    }
    else
    {
        fputs(type, fp);
    }
}

void GenerateVector(ExprVector vector, FILE *fp);

void GenerateExpr(Expr expr, FILE *fp, char last)
{
    switch (expr.type)
    {
    case FnDefExpr:
        fprintf(fp, "void %s(", expr.expr._fn_def.name);
        GenerateVector(expr.expr._fn_def.args, fp);
        fputs(") {\n", fp);
        GenerateVector(expr.expr._fn_def.body, fp);
        fputs("\n}", fp);
        break;
    case ArgExpr:
        GenerateType(expr.expr._arg.type, fp);
        fprintf(fp, " %s", expr.expr._arg.name);
        if (!last)
        {
            fputc(',', fp);
        }
        break;
    case ReturnExpr:
        fputs("return ", fp);
        break;
    case FnCallExpr:
        fprintf(fp, "%s(", expr.expr._fn_call.name);
        GenerateVector(expr.expr._fn_call.args, fp);
        fputs(");", fp);
        break;
    case VarDefExpr:
        GenerateType(expr.expr._var_def.type, fp);
        fprintf(fp, " %s=", expr.expr._var_def.name);
        GenerateExpr(*expr.expr._var_def.val, fp, 1);
        break;
    case VarUseExpr:
        fputs(expr.expr._var_use.name, fp);
        break;
    case OpExpr:
        fputs("op ", fp);
        break;
    case ValueExpr:
        fputs(expr.expr._value.val, fp);
        break;
    case ArrayExpr:
        fputs("array ", fp);
        GenerateVector(expr.expr._array.elems, fp);
        break;
    case IfExpr:
        fputs("if ", fp);
        GenerateVector(expr.expr._if.body, fp);
        break;
    case ElifExpr:
        fputs("elif ", fp);
        GenerateVector(expr.expr._elif.body, fp);
        break;
    case ElseExpr:
        fputs("else ", fp);
        GenerateVector(expr.expr._else.body, fp);
        break;
    case LoopExpr:
        fputs("loop ", fp);
        GenerateVector(expr.expr._loop.body, fp);
        break;
    case WhileExpr:
        fputs("while ", fp);
        GenerateVector(expr.expr._while.body, fp);
        break;
    case ForExpr:
        fputs("for ", fp);
        GenerateVector(expr.expr._for.body, fp);
        break;
    }
}

void GenerateVector(ExprVector vector, FILE *fp)
{
    for (int i = 0; i < vector.size; ++i)
    {
        GenerateExpr(vector.array[i], fp, i + 1 == vector.size);
    }
}

#pragma endregion

int main()
{
    // Open the file
    FILE *filepoint;
    fopen_s(&filepoint, "main.ch", "rb");

    // Get the size of the file
    fseek(filepoint, 0L, SEEK_END);
    int file_size = ftell(filepoint);
    rewind(filepoint);

    // Allocate the buffer, read contents and close the file
    char *buffer = malloc(file_size);
    fread(buffer, file_size, 1, filepoint);
    fclose(filepoint);

#pragma region Lexer

    TokenVector tokens = NewTokenVector(file_size / 4);
    CharVector value = NewCharVector(32);

    // Loop through all characters and create tokens
    for (int i = 0; i < file_size; ++i)
    {
        char c = buffer[i];

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

            // Check value
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
                if (i + 1 < file_size && buffer[i + 1] == '.')
                {
                    token.type = Range;
                    i += 1;
                }
                break;
            case 'f':
                if (i + 2 < file_size && buffer[i + 1] == 'n' && isspace(buffer[i + 2]))
                {
                    token.type = Function;
                    i += 2;
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
            case 'e':
                if (i + 4 < file_size && buffer[i + 1] == 'n' && buffer[i + 2] == 'u' && buffer[i + 3] == 'm' && isspace(buffer[i + 4]))
                {
                    token.type = Enum;
                    i += 4;
                }
                else if (i + 4 < file_size && buffer[i + 1] == 'l' && buffer[i + 2] == 's' && buffer[i + 3] == 'e' && isspace(buffer[i + 4]))
                {
                    token.type = If;
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
            case 'i':
                if (i + 2 < file_size && buffer[i + 1] == 'f' && isspace(buffer[i + 2]))
                {
                    token.type = If;
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
            else if (!isspace(c))
            {
                PushChar(&value, c);
            }
        }
    }

#pragma endregion
#pragma region Parser

    int index = 0;
    int expr_index = 0;
    ExprVector ast = EvaluteTokens(tokens, &index, -1);

    // for (int i = 0; i < ast.array[0].expr._fn_def.body.size; i++)
    // {
    // }
    // printf("%i\n", ast.array[0].expr._fn_def.body.array[1].expr._var_def.val->type);
    // printf("%i - size", ast.array[0].expr._fn_def.body.size);

#pragma endregion
#pragma region Generator

    fopen_s(&filepoint, "main.c", "w");

    GenerateVector(ast, filepoint);

    fclose(filepoint);

#pragma endregion

    // Free the memory
    free(buffer);
    free(value.array);
    for (int i = 0; i < tokens.size; ++i)
    {
        free(tokens.array[i].value);
    }
    free(tokens.array);
    free(ast.array);

    return 0;
}
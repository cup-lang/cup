#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define VECTOR(name, lower, type)                           \
    typedef struct                                          \
    {                                                       \
        type *array;                                        \
        int size;                                           \
        int capacity;                                       \
    } name;                                                 \
    name lower##_new(int c)                                 \
    {                                                       \
        name v;                                             \
        v.array = calloc(c, sizeof(type));                  \
        v.size = 0;                                         \
        v.capacity = c;                                     \
        return v;                                           \
    }                                                       \
    void lower##_push(name *v, type e)                      \
    {                                                       \
        v->array[v->size++] = e;                            \
        if (v->size == v->capacity)                         \
        {                                                   \
            int c = v->capacity *= 2;                       \
            v->array = realloc(v->array, sizeof(type) * c); \
        }                                                   \
    }

VECTOR(String, string, char);

#define THROW(...) fprintf(stderr, __VA_ARGS__);exit(1);

typedef enum
{
    IDENT,
    PUB,
    MOD,
    USE,
    STRUCT,
    ENUM,
    UNION,
    TRAIT,
    IMP,
    SELF,
    REST,
    INL,
    MACRO,

    _CONST,
    _TRUE,
    _FALSE,
    _NULL,

    IF,
    ELIF,
    ELSE,
    DO,
    WHILE,
    FOR,
    _IN,
    MATCH,
    FALL,
    BREAK,
    NEXT,
    RETURN,
    DEFER,
    GOTO,
    AS,

    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    TYPE_ARROW,
    MATCH_ARROW,
    QUESTION_MARK,
    BACKTICK,
    AT,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_SQUARE,
    RIGHT_SQUARE,

    RANGE,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    NOT,
    AND,
    OR,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    ADD,
    ADD_ASSIGN,
    SUB,
    SUB_ASSIGN,
    MUL,
    MUL_ASSIGN,
    DIV,
    DIV_ASSIGN,
    MODULO,
    MODULO_ASSIGN,
    BIT_NOT,
    BIT_NOT_ASSIGN,
    BIT_AND,
    BIT_AND_ASSIGN,
    BIT_OR,
    BIT_OR_ASSIGN,
    BIT_XOR,
    BIT_XOR_ASSIGN,
    LEFT_SHIFT,
    LEFT_SHIFT_ASSIGN,
    RIGHT_SHIFT,
    RIGHT_SHIFT_ASSIGN
} TokenKind;

typedef struct
{
    TokenKind kind;
    char *value;
} Token;

VECTOR(TokenVector, token_vector, Token);

TokenVector lex(String input)
{
    TokenVector tokens = token_vector_new(input.size / 4);
    char is_comment = 0;
    String value = string_new(32);

    for (int i = 0; i < input.size + 1; ++i)
    {
        char c = input.array[i];

        if (is_comment)
        {
            if (c == '\n')
            {
                is_comment = 0;
            }
            continue;
        }

        TokenKind kind = -1;

        if (isspace(c) || c == '\0')
        {
            kind = 0;
        }
        else
        {
            switch (c)
            {
            case '#':
                is_comment = 1;
                kind = 0;
                break;
            case ';':
                kind = SEMICOLON;
                break;
            case ':':
                kind = COLON;
                break;
            case ',':
                kind = COLON;
                break;
            case '?':
                kind = QUESTION_MARK;
                break;
            case '`':
                kind = BACKTICK;
                break;
            case '@':
                kind = AT;
                break;
            case '(':
                kind = LEFT_PAREN;
                break;
            case ')':
                kind = RIGHT_PAREN;
                break;
            case '{':
                kind = LEFT_BRACE;
                break;
            case '}':
                kind = RIGHT_BRACE;
                break;
            case '[':
                kind = LEFT_SQUARE;
                break;
            case ']':
                kind = RIGHT_SQUARE;
                break;
            case '.':
                if (i + 1 < input.size && input.array[i + 1] == '.')
                {
                    kind = RANGE;
                    ++i;
                }
                else
                {
                    kind = DOT;
                }
                break;
            case '=':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '>':
                        kind = MATCH_ARROW;
                        ++i;
                        break;
                    case '=':
                        kind = EQUAL;
                        ++i;
                        break;
                    default:
                        kind = ASSIGN;
                    }
                }
                else
                {
                    kind = ASSIGN;
                }
                break;
            case '!':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = NOT_EQUAL;
                    ++i;
                }
                else
                {
                    kind = NOT;
                }
                break;
            case '+':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = ADD_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = ADD;
                }
                break;
            case '-':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '>':
                        kind = TYPE_ARROW;
                        ++i;
                        break;
                    case '=':
                        kind = SUB_ASSIGN;
                        ++i;
                        break;
                    default:
                        kind = SUB;
                    }
                }
                else
                {
                    kind = SUB;
                }
                break;
            case '*':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = MUL_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = MUL;
                }
                break;
            case '/':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = DIV_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = DIV;
                }
                break;
            case '%':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = MODULO_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = MODULO;
                }
                break;
            case '~':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = BIT_NOT_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = BIT_NOT;
                }
                break;
            case '&':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '&':
                        kind = AND;
                        ++i;
                        break;
                    case '=':
                        kind = BIT_AND_ASSIGN;
                        ++i;
                        break;
                    default:
                        kind = BIT_AND;
                    }
                }
                else
                {
                    kind = BIT_AND;
                }
                break;
            case '|':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '|':
                        kind = OR;
                        ++i;
                        break;
                    case '=':
                        kind = BIT_OR_ASSIGN;
                        ++i;
                        break;
                    default:
                        kind = BIT_OR;
                    }
                }
                else
                {
                    kind = BIT_OR;
                }
                break;
            case '^':
                if (i + 1 < input.size && input.array[i + 1] == '=')
                {
                    kind = BIT_XOR_ASSIGN;
                    ++i;
                }
                else
                {
                    kind = BIT_XOR;
                }
                break;
            case '<':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '=':
                        kind = LESS_EQUAL;
                        ++i;
                        break;
                    case '<':
                        if (i + 2 < input.size && input.array[i + 2] == '=')
                        {
                            kind = LEFT_SHIFT_ASSIGN;
                            i += 2;
                        }
                        else
                        {
                            kind = LEFT_SHIFT;
                            i += 2;
                        }
                        break;
                    default:
                        kind = LESS;
                    }
                }
                else
                {
                    kind = LESS;
                }
                break;
            case '>':
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '=':
                        kind = GREATER_EQUAL;
                        ++i;
                        break;
                    case '>':
                        if (i + 2 < input.size)
                        {
                            char next = input.array[i + 2];
                            if (next == '=')
                            {
                                kind = RIGHT_SHIFT_ASSIGN;
                                i += 2;
                                break;
                            }
                            else if (next == '_' || isalnum(next))
                            {
                                kind = RIGHT_SHIFT;
                                i += 2;
                                break;
                            }
                        }
                    default:
                        kind = GREATER;
                    }
                }
                else
                {
                    kind = GREATER;
                }
                break;
            }
        }

        if (kind == -1)
        {
            if (c == '_' || isalnum(c))
            {
                string_push(&value, c);
            }
            else
            {
                THROW("Unexpected symbol '%c'", c);
            }
        }
        else
        {
            if (value.size)
            {
                TokenKind value_kind = -1;

                if (strcmp(value.array, "pub") == 0)
                {
                    value_kind = PUB;
                }
                else if (strcmp(value.array, "mod") == 0)
                {
                    value_kind = MOD;
                }
                else if (strcmp(value.array, "use") == 0)
                {
                    value_kind = USE;
                }
                else if (strcmp(value.array, "struct") == 0)
                {
                    value_kind = STRUCT;
                }
                else if (strcmp(value.array, "enum") == 0)
                {
                    value_kind = ENUM;
                }
                else if (strcmp(value.array, "union") == 0)
                {
                    value_kind = UNION;
                }
                else if (strcmp(value.array, "trait") == 0)
                {
                    value_kind = TRAIT;
                }
                else if (strcmp(value.array, "imp") == 0)
                {
                    value_kind = IMP;
                }
                else if (strcmp(value.array, "self") == 0)
                {
                    value_kind = SELF;
                }
                else if (strcmp(value.array, "rest") == 0)
                {
                    value_kind = REST;
                }
                else if (strcmp(value.array, "inl") == 0)
                {
                    value_kind = INL;
                }
                else if (strcmp(value.array, "macro") == 0)
                {
                    value_kind = MACRO;
                }
                else if (strcmp(value.array, "const") == 0)
                {
                    value_kind = _CONST;
                }
                else if (strcmp(value.array, "true") == 0)
                {
                    value_kind = _TRUE;
                }
                else if (strcmp(value.array, "false") == 0)
                {
                    value_kind = _FALSE;
                }
                else if (strcmp(value.array, "null") == 0)
                {
                    value_kind = _NULL;
                }
                else if (strcmp(value.array, "if") == 0)
                {
                    value_kind = IF;
                }
                else if (strcmp(value.array, "elif") == 0)
                {
                    value_kind = ELIF;
                }
                else if (strcmp(value.array, "else") == 0)
                {
                    value_kind = ELSE;
                }
                else if (strcmp(value.array, "do") == 0)
                {
                    value_kind = DO;
                }
                else if (strcmp(value.array, "while") == 0)
                {
                    value_kind = WHILE;
                }
                else if (strcmp(value.array, "for") == 0)
                {
                    value_kind = FOR;
                }
                else if (strcmp(value.array, "in") == 0)
                {
                    value_kind = _IN;
                }
                else if (strcmp(value.array, "match") == 0)
                {
                    value_kind = MATCH;
                }
                else if (strcmp(value.array, "fall") == 0)
                {
                    value_kind = FALL;
                }
                else if (strcmp(value.array, "break") == 0)
                {
                    value_kind = BREAK;
                }
                else if (strcmp(value.array, "next") == 0)
                {
                    value_kind = NEXT;
                }
                else if (strcmp(value.array, "return") == 0)
                {
                    value_kind = RETURN;
                }
                else if (strcmp(value.array, "defer") == 0)
                {
                    value_kind = DEFER;
                }
                else if (strcmp(value.array, "goto") == 0)
                {
                    value_kind = GOTO;
                }
                else if (strcmp(value.array, "as") == 0)
                {
                    value_kind = AS;
                }

                if (value_kind == -1)
                {
                    Token token;
                    token.kind = IDENT;
                    token.value = malloc(value.size);
                    memcpy(token.value, value.array, value.size);
                    token.value[value.size] = '\0';
                    token_vector_push(&tokens, token);
                }
                else
                {
                    Token token;
                    token.kind = value_kind;
                    token_vector_push(&tokens, token);
                }

                value.size = 0;
            }

            if (kind)
            {
                Token token;
                token.kind = kind;
                token_vector_push(&tokens, token);
            }
        }
    }

    free(value.array);

    const char *const token_names[] =
        {
            [IDENT] = "IDENT",
            [PUB] = "PUB",
            [MOD] = "MOD",
            [USE] = "USE",
            [STRUCT] = "STRUCT",
            [ENUM] = "ENUM",
            [UNION] = "UNION",
            [TRAIT] = "TRAIT",
            [IMP] = "IMP",
            [SELF] = "SELF",
            [REST] = "REST",
            [INL] = "INL",
            [MACRO] = "MACRO",

            [_CONST] = "CONST",
            [_TRUE] = "TRUE",
            [_FALSE] = "_FALSE",
            [_NULL] = "NULL",

            [IF] = "IF",
            [ELIF] = "ELIF",
            [ELSE] = "ELSE",
            [DO] = "DO",
            [WHILE] = "WHILE",
            [FOR] = "FOR",
            [_IN] = "IN",
            [MATCH] = "MATCH",
            [FALL] = "FALL",
            [BREAK] = "BREAK",
            [NEXT] = "NEXT",
            [RETURN] = "RETURN",
            [DEFER] = "DEFER",
            [GOTO] = "GOTO",
            [AS] = "AS",

            [SEMICOLON] = "SEMICOLON",
            [COLON] = "COLON",
            [COMMA] = "COMMA",
            [DOT] = "DOT",
            [TYPE_ARROW] = "TYPE_ARROW",
            [MATCH_ARROW] = "MATCH_ARROW",
            [QUESTION_MARK] = "QUESTION_MARK",
            [BACKTICK] = "BACKTICK",
            [AT] = "AT",
            [LEFT_PAREN] = "LEFT_PAREN",
            [RIGHT_PAREN] = "RIGHT_PAREN",
            [LEFT_BRACE] = "LEFT_BRACE",
            [RIGHT_BRACE] = "RIGHT_BRACE",
            [LEFT_SQUARE] = "LEFT_SQUARE",
            [RIGHT_SQUARE] = "RIGHT_SQUARE",

            [RANGE] = "RANGE",
            [ASSIGN] = "ASSIGN",
            [EQUAL] = "EQUAL",
            [NOT_EQUAL] = "NOT_EQUAL",
            [NOT] = "NOT",
            [AND] = "AND",
            [OR] = "OR",
            [LESS] = "LESS",
            [LESS_EQUAL] = "LESS_EQUAL",
            [GREATER] = "GREATER",
            [GREATER_EQUAL] = "GREATER_EQUAL",
            [ADD] = "ADD",
            [ADD_ASSIGN] = "ADD_ASSIGN",
            [SUB] = "SUB",
            [SUB_ASSIGN] = "SUB_ASSIGN",
            [MUL] = "MUL",
            [MUL_ASSIGN] = "MUL_ASSIGN",
            [DIV] = "DIV",
            [DIV_ASSIGN] = "DIV_ASSIGN",
            [MODULO] = "MODULO",
            [MODULO_ASSIGN] = "MODULO_ASSIGN",
            [BIT_NOT] = "BIT_NOT",
            [BIT_NOT_ASSIGN] = "BIT_NOT_ASSIGN",
            [BIT_AND] = "BIT_AND",
            [BIT_AND_ASSIGN] = "BIT_AND_ASSIGN",
            [BIT_OR] = "BIT_OR",
            [BIT_OR_ASSIGN] = "BIT_OR_ASSIGN",
            [BIT_XOR] = "BIT_XOR",
            [BIT_XOR_ASSIGN] = "BIT_XOR_ASSIGN",
            [LEFT_SHIFT] = "LEFT_SHIFT",
            [LEFT_SHIFT_ASSIGN] = "LEFT_SHIFT_ASSIGN",
            [RIGHT_SHIFT] = "RIGHT_SHIFT",
            [RIGHT_SHIFT_ASSIGN] = "RIGHT_SHIFT_ASSIGN"};

    puts("Tokens:");
    for (int i = 0; i < tokens.size; ++i)
    {
        printf(" %s", token_names[tokens.array[i].kind]);
        if (tokens.array[i].kind == IDENT)
        {
            printf("(\"%s\")", tokens.array[i].value);
        }
        puts("");
    }
    puts("");

    return tokens;
}

/*

    ops
    fn defs
    generics

    pub applies to:
        - mods
        - structs
        - traits
        - enums
        - unions
        - fields
        - imps
        - static variables
        - functions

    inl applies to:
        - static variables
        - functions  

    macro applies to:
        - functions
        
    (pub) mod:
        - name: string
        - body: arr<expr>

    use:
        - name: string

    (pub) (generic) struct:
        - name: string
        - body: arr<field>

    (pub) field:
        - name: string
        - type: string?

    (pub) (generic) enum:
        - name: string
        - body: arr<option>

    (pub) (generic) option:
        - name: string
        - body: arr<field>

    (pub) (generic) union:
        - name: string
        - body: arr<field>

    (pub) (generic) trait:
        - name: string
        - body: arr<function>

    (pub) (generic) imp:
        - trait: string?
        - target: string?

    (pub) (generic) fn_def:
        - name: string
        - args: arr<arg>
        - type: string?
        - body: arr<expr>

    arg:
        name: string
        type: string?

    self

    rest:

    const:

    true:

    false:

    null:

    if:

    elif:

    else:

    do:

    while:

    for:

    in:

    match:

    fall:

    break:

    next:

    return:

    defer:

    goto:

    as:

*/

typedef enum
{
    A
} ExprKind;

typedef union
{
    char _;
} ExprUnion;

typedef struct
{
    ExprKind kind;
    ExprUnion u;
} Expr;

VECTOR(ExprVector, expr_vector, Expr);

ExprVector parse(TokenVector tokens)
{
}

ExprVector compile(char *path, int length)
{
    DIR *dir = opendir(path);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        switch (ent->d_namlen)
        {
        case 1:
            if (ent->d_name[0] == '.')
            {
                break;
            }
        case 2:
            if (ent->d_name[0] == '.' && ent->d_name[1] == '.')
            {
                break;
            }
        default:
        {
            int new_length = ent->d_namlen;
            char *new_path = malloc(length + new_length + 1);
            memcpy(new_path, path, length);
            strcpy(new_path + length, ent->d_name);
            switch (ent->d_type)
            {
            case DT_DIR:
                compile(new_path, length + new_length);
                break;
            case DT_REG:
            {
                FILE *input;
                fopen_s(&input, new_path, "rb");
                fseek(input, 0L, SEEK_END);
                String data = string_new(ftell(input));
                rewind(input);
                fread(data.array, data.size = data.capacity, 1, input);
                fclose(input);
                parse(lex(data));
                break;
            }
            }
            free(new_path);
            break;
        }
        }
    }
    closedir(dir);
}

void gen(FILE *output, ExprVector exprs)
{
}

int main()
{
    ExprVector ast = compile("./src/", 6);

    FILE *output;
    fopen_s(&output, "./bin/main.c", "w");
    gen(output, ast);
    fclose(output);
}

//////////////////////////////

// multiple file support
// non-linear parsing
// single output file
// generics support
// generic functions
// generic structs
// generic enums
// generic traits
// generic variables
// match on enums (exhaistive)
// if elif else
// do while do-while
// break return next
// all ops
// mod support

//////////////////////////////

// lex - DONE
// parse
// gen
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define VECTOR(name, lower, type) \
    VECTOR_STRUCT(name, type)     \
    VECTOR_FUNC(name, lower, type)

#define VECTOR_STRUCT(name, type) \
    typedef struct                \
    {                             \
        type *array;              \
        int size;                 \
        int capacity;             \
    } name;

#define VECTOR_FUNC(name, lower, type)                      \
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

#define THROW(...)                \
    fprintf(stderr, __VA_ARGS__); \
    exit(1);

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
    IMPL,
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

void print_token_vector(TokenVector tokens)
{
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
            [IMPL] = "IMPL",
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
        printf("  %s", token_names[tokens.array[i].kind]);
        if (tokens.array[i].kind == IDENT)
        {
            printf("(\"%s\")", tokens.array[i].value);
        }
        puts("");
    }
    puts("");
}

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
                kind = BIT_NOT;
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
                else if (strcmp(value.array, "impl") == 0)
                {
                    value_kind = IMPL;
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

    print_token_vector(tokens);

    return tokens;
}

/*

    value = op + fn_call + var_use + literal
    global = use + mod + struct + enum + union + trait + impl + fn_def + var_def
    local = for + do + if + elif + else + while + match + fall + break + next + goto + return + deref + local_var_def + value
    literal = string_lit + arr_lit + num_lit + bool_lit + null_lit + self_lit

    attr:
        - name: string
        - data: string

    type:
        - const: bool
        - name: string
        - child: arr<type>

    constr_type:
        - name: string
        - constr: arr<type>

    (attr) (pub) mod:
        - attr: arr<attr>
        - pub: bool 
        - name: string
        - body: arr<global>

    (attr) use:
        - attr: arr<attr>
        - name: string

    (attr) (pub) (generic) struct:
        - attr: arr<attr>
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>

    (pub) field:
        - pub: bool 
        - name: string
        - type: type

    (attr) (pub) (generic) enum:
        - attr: arr<attr>
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<option>

    (pub) option:
        - pub: bool 
        - name: string
        - body: arr<field>

    (attr) (pub) (generic) union:
        - attr: arr<attr>
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>

    (attr) (pub) (generic) trait:
        - attr: arr<attr>
        - pub: bool
        - name: string
        - gen: arr<constr_type>
        - body: arr<fn_def>

    (attr) (pub) (generic) impl:
        - attr: arr<attr>
        - pub: bool
        - gen: arr<constr_type>
        - trait: type
        - target: type
        - body: arr<fn_def>

    (attr) (pub) (inl) (macro) (generic) fn_def:
        - attr: arr<attr>
        - pub: bool
        - inl: bool
        - macro: bool
        - name: string
        - gen: arr<constr_type>
        - args: arr<arg>
        - ret: type
        - body: arr<local>

    (rest) arg:
        rest: bool
        name: string
        type: type

    (attr) (pub) (inl) (combo) var_def:
        - attr: arr<attr>
        - pub: bool
        - inl: bool
        - name: string
        - type: type 

    (combo) local_var_def:
        - name: string
        - type: type
        - value: value

    fn_call:
        - type: type
        - args: arr<value>

    var_use:
        - name: string

    struct_inst:
        - type: type
        - fields: arr<field_val>

    field_val:
        - name: string
        - value: value

    do:
    block:
        - body: arr<local>

    string_lit:
    arr_lit:
        - value: arr<value>

    num_lit:
        - value: string

    bool_lit:
        - value: bool

    null_lit:
    self_lit:

    if:
    elif:
    else:
    while:
        - con: value
        - body: arr<local>

    for:
        - loop_var: string
        - range: value

    match:
        - value: value
        - body: arr<case>

    case:
        - value: value
        - body: arr<local>

    fall:

    break:
    next:
    goto:
        - label: string

    return:
        - value: value

    defer:
        - body: arr<local>

    cond_op:
        - con: value
        - lhs: value
        - rhs: value

    range_op:
    equal_op:
    not_equal_op:
    and_op:
    or_op:
    less_op:
    less_equal_op:
    greater_op:
    greater_equal_op:
    add_op:
    sub_op:
    mul_op:
    div_op:
    mod_op:
    bit_and:
    bit_or:
    bit_xor:
    left_shift:
    right_shift:
        - lhs: value
        - rhs: value

    not_op:
    bit_not:
        - rhs: value

    assign_op:
    add_assign_op:
    sub_assign_op:
    mul_assign_op:
    div_assign_op:
    mod_assign_op:
    bit_and_assign:
    bit_or_assign:
    bit_xor_assign:
    left_shift_assign:
    right_shift_assign:
        - lhs: var_use
        - rhs: value

*/

typedef struct Expr Expr;

VECTOR_STRUCT(ExprVector, Expr);

typedef enum
{
    E_ATTR,
    E_TYPE,
    E_CONSTR_TYPE,
    E_MOD,
    E_USE,
    E_STRUCT,
    E_FIELD,
    E_ENUM,
    E_OPTION,
    E_UNION,
    E_TRAIT,
    E_IMPL,
    E_FN_DEF,
    E_ARG,
    E_VAR_DEF,
    E_LOCAL_VAR_DEF,
    E_FN_CALL,
    E_VAR_USE,
    E_STRUCT_INST,
    E_FIELD_VAL,
    E_DO,
    E_BLOCK,
    E_STRING_LIT,
    E_ARR_LIT,
    E_NUM_LIT,
    E_BOOL_LIT,
    E_NULL_LIT,
    E_SELF_LIT,
    E_IF,
    E_ELIF,
    E_ELSE,
    E_WHILE,
    E_FOR,
    E_MATCH,
    E_CASE,
    E_FALL,
    E_BREAK,
    E_NEXT,
    E_GOTO,
    E_RETURN,
    E_DEFER,
    E_COND_OP,
    E_RANGE_OP,
    E_EQUAL_OP,
    E_NOT_EQUAL_OP,
    E_AND_OP,
    E_OR_OP,
    E_LESS_OP,
    E_LESS_EQUAL_OP,
    E_GREATER_OP,
    E_GREATER_EQUAL_OP,
    E_ADD_OP,
    E_SUB_OP,
    E_MUL_OP,
    E_DIV_OP,
    E_MOD_OP,
    E_BIT_AND,
    E_BIT_OR,
    E_BIT_XOR,
    E_LEFT_SHIFT,
    E_RIGHT_SHIFT,
    E_NOT_OP,
    E_BIT_NOT_OP,
    E_ASSIGN_OP,
    E_ADD_ASSIGN_OP,
    E_SUB_ASSIGN_OP,
    E_MUL_ASSIGN_OP,
    E_DIV_ASSIGN_OP,
    E_MOD_ASSIGN_OP,
    E_BIT_AND_ASSIGN,
    E_BIT_OR_ASSIGN,
    E_BIT_XOR_ASSIGN,
    E_LEFT_SHIFT_ASSIGN,
    E_RIGHT_SHIFT_ASSIGN
} ExprKind;

typedef struct
{
    char *name;
    char *data;
} Attr;

typedef struct
{
    char _const;
    char *name;
    ExprVector children;
} Type;

typedef struct
{
    char *name;
    ExprVector constr;
} ConstrType;

typedef struct
{
    ExprVector attr;
    char pub;
    char *name;
    ExprVector body;
} Mod;

typedef struct
{
    ExprVector attr;
    char *name;
} Use;

typedef struct
{
    ExprVector attr;
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Struct;

typedef struct
{
    char pub;
    char *name;
    Expr *type;
} Field;

typedef struct
{
    ExprVector attr;
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Enum;

typedef struct
{
    char pub;
    char *name;
    ExprVector body;
} Option;

typedef struct
{
    ExprVector attr;
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Union;

typedef struct
{
    ExprVector attr;
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Trait;

typedef struct
{
    ExprVector attr;
    char pub;
    ExprVector gen;
    Expr *trait;
    Expr *target;
    ExprVector body;
} Impl;

typedef struct
{
    ExprVector attr;
    char pub;
    char inl;
    char macro;
    char *name;
    ExprVector gen;
    ExprVector args;
    Expr *ret;
    ExprVector body;
} FnDef;

typedef struct
{
    char rest;
    char *name;
    Expr *type;
} Arg;

typedef struct
{
    ExprVector attr;
    char pub;
    char inl;
    char *name;
    Expr *type;
} VarDef;

typedef struct
{
    char *name;
    Expr *type;
    Expr *value;
} LocalVarDef;

typedef struct
{
    Expr *type;
    ExprVector args;
} FnCall;

typedef struct
{
    char *name;
} VarUse;

typedef struct
{
    Expr *type;
    ExprVector fields;
} StructInst;

typedef struct
{
    char *name;
    Expr *value;
} FieldVal;

typedef struct
{
    ExprVector body;
} Do;

typedef struct
{
    ExprVector body;
} Block;

typedef struct
{
    ExprVector value;
} StringLit;

typedef struct
{
    ExprVector value;
} ArrLit;

typedef struct
{
    char *value;
} NumLit;

typedef struct
{
    char value;
} BoolLit;

typedef struct
{
    Expr *con;
    ExprVector body;
} If;

typedef struct
{
    Expr *con;
    ExprVector body;
} Elif;

typedef struct
{
    Expr *con;
    ExprVector body;
} Else;

typedef struct
{
    Expr *con;
    ExprVector body;
} While;

typedef struct
{
    char *loop_var;
    Expr *range;
} For;

typedef struct
{
    Expr *value;
    ExprVector body;
} Match;

typedef struct
{
    Expr *value;
    ExprVector body;
} Case;

typedef struct
{
    char *label;
} Break;

typedef struct
{
    char *label;
} Next;

typedef struct
{
    char *label;
} Goto;

typedef struct
{
    Expr *value;
} Return;

typedef struct
{
    ExprVector body;
} Defer;

typedef struct
{
    Expr *cond;
    Expr *lhs;
    Expr *rhs;
} CondOp;

typedef struct
{
    Expr *lhs;
    Expr *rhs;
} BinaryOp;

typedef struct
{
    Expr *lhs;
    Expr *rhs;
} UnaryOp;

typedef union
{
    Attr attr;
    Type type;
    ConstrType constr_type;
    Mod mod;
    Use use;
    Struct _struct;
    Field field;
    Enum _enum;
    Option option;
    Union _union;
    Trait trait;
    Impl impl;
    FnDef fn_def;
    Arg arg;
    VarDef var_def;
    LocalVarDef local_var_def;
    FnCall fn_call;
    VarUse var_use;
    StructInst struct_inst;
    FieldVal field_val;
    Do _do;
    Block block;
    StringLit string_lit;
    ArrLit arr_lit;
    NumLit num_lit;
    BoolLit bool_lit;
    If _if;
    Elif elif;
    Else _else;
    While _while;
    For _for;
    Match match;
    Case _case;
    Break _break;
    Next next;
    Goto _goto;
    Return _return;
    Defer defer;
    CondOp cond_op;
    BinaryOp binary_op;
    UnaryOp unary_op;
} ExprUnion;

typedef struct Expr
{
    ExprKind kind;
    ExprUnion u;
} Expr;

VECTOR_FUNC(ExprVector, expr_vector, Expr);

void indent(int depth)
{
    for (int i = 0; i <= depth; ++i)
    {
        printf("  ");
    }
}

void print_expr_vector(ExprVector exprs, int depth);

void print_expr(Expr expr, int depth)
{
    switch (expr.kind)
    {
    case E_ATTR:
        printf("(name = %s", expr.u.attr.name);
        if (expr.u.attr.data)
        {
            printf(", data = %s", expr.u.attr.data);
        }
        putchar(')');
        break;
    case E_TYPE:
        printf("(const = %i, name = %s", expr.u.type._const, expr.u.type.name);
        if (expr.u.type.children.size)
        {
            printf(", children = [");
            print_expr_vector(expr.u.type.children, depth + 1);
            indent(depth);
            putchar(']');
        }
        putchar(')');
        break;
    case E_CONSTR_TYPE:
        printf("");
        break;
    case E_MOD:
        break;
    case E_USE:
        break;
    case E_STRUCT:
        break;
    case E_FIELD:
        break;
    case E_ENUM:
        break;
    case E_OPTION:
        break;
    case E_UNION:
        break;
    case E_TRAIT:
        break;
    case E_IMPL:
        break;
    case E_FN_DEF:
        break;
    case E_ARG:
        break;
    case E_VAR_DEF:
        break;
    case E_LOCAL_VAR_DEF:
        break;
    case E_FN_CALL:
        break;
    case E_VAR_USE:
        break;
    case E_STRUCT_INST:
        break;
    case E_FIELD_VAL:
        break;
    case E_DO:
        break;
    case E_BLOCK:
        break;
    case E_STRING_LIT:
        break;
    case E_ARR_LIT:
        break;
    case E_NUM_LIT:
        break;
    case E_BOOL_LIT:
        break;
    case E_NULL_LIT:
        break;
    case E_SELF_LIT:
        break;
    case E_IF:
        break;
    case E_ELIF:
        break;
    case E_ELSE:
        break;
    case E_WHILE:
        break;
    case E_FOR:
        break;
    case E_MATCH:
        break;
    case E_CASE:
        break;
    case E_FALL:
        break;
    case E_BREAK:
        break;
    case E_NEXT:
        break;
    case E_GOTO:
        break;
    case E_RETURN:
        break;
    case E_DEFER:
        break;
    case E_RANGE_OP:
    case E_EQUAL_OP:
    case E_NOT_EQUAL_OP:
    case E_AND_OP:
    case E_OR_OP:
    case E_LESS_OP:
    case E_LESS_EQUAL_OP:
    case E_GREATER_OP:
    case E_GREATER_EQUAL_OP:
    case E_ADD_OP:
    case E_SUB_OP:
    case E_MUL_OP:
    case E_DIV_OP:
    case E_MOD_OP:
    case E_BIT_AND:
    case E_BIT_OR:
    case E_BIT_XOR:
    case E_LEFT_SHIFT:
    case E_RIGHT_SHIFT:
    case E_ASSIGN_OP:
    case E_ADD_ASSIGN_OP:
    case E_SUB_ASSIGN_OP:
    case E_MUL_ASSIGN_OP:
    case E_DIV_ASSIGN_OP:
    case E_MOD_ASSIGN_OP:
    case E_BIT_AND_ASSIGN:
    case E_BIT_OR_ASSIGN:
    case E_BIT_XOR_ASSIGN:
    case E_LEFT_SHIFT_ASSIGN:
    case E_RIGHT_SHIFT_ASSIGN:
        break;
    case E_NOT_OP:
    case E_BIT_NOT_OP:
        break;
    }
}

void print_expr_vector(ExprVector exprs, int depth)
{
    const char *const expr_names[] =
        {
            [E_ATTR] = "ATTR",
            [E_TYPE] = "TYPE",
            [E_CONSTR_TYPE] = "CONSTR_TYPE",
            [E_MOD] = "MOD",
            [E_USE] = "USE",
            [E_STRUCT] = "STRUCT",
            [E_FIELD] = "FIELD",
            [E_ENUM] = "ENUM",
            [E_OPTION] = "OPTION",
            [E_UNION] = "UNION",
            [E_TRAIT] = "TRAIT",
            [E_IMPL] = "IMPL",
            [E_FN_DEF] = "FN_DEF",
            [E_ARG] = "ARG",
            [E_VAR_DEF] = "VAR_DEF",
            [E_LOCAL_VAR_DEF] = "LOCAL_VAR_DEF",
            [E_FN_CALL] = "FN_CALL",
            [E_VAR_USE] = "VAR_USE",
            [E_STRUCT_INST] = "STRUCT_INST",
            [E_FIELD_VAL] = "FIELD_VAL",
            [E_DO] = "DO",
            [E_BLOCK] = "BLOCK",
            [E_STRING_LIT] = "STRING_LIT",
            [E_ARR_LIT] = "ARR_LIT",
            [E_NUM_LIT] = "NUM_LIT",
            [E_BOOL_LIT] = "BOOL_LIT",
            [E_NULL_LIT] = "NULL_LIT",
            [E_SELF_LIT] = "SELF_LIT",
            [E_IF] = "IF",
            [E_ELIF] = "ELIF",
            [E_ELSE] = "ELSE",
            [E_WHILE] = "WHILE",
            [E_FOR] = "FOR",
            [E_MATCH] = "MATCH",
            [E_CASE] = "CASE",
            [E_FALL] = "FALL",
            [E_BREAK] = "BREAK",
            [E_NEXT] = "NEXT",
            [E_GOTO] = "GOTO",
            [E_RETURN] = "RETURN",
            [E_DEFER] = "DEFER",
            [E_COND_OP] = "COND_OP",
            [E_RANGE_OP] = "RANGE_OP",
            [E_EQUAL_OP] = "EQUAL_OP",
            [E_NOT_EQUAL_OP] = "NOT_EQUAL_OP",
            [E_AND_OP] = "AND_OP",
            [E_OR_OP] = "OR_OP",
            [E_LESS_OP] = "LESS_OP",
            [E_LESS_EQUAL_OP] = "LESS_EQUAL_OP",
            [E_GREATER_OP] = "GREATER_OP",
            [E_GREATER_EQUAL_OP] = "GREATER_EQUAL_OP",
            [E_ADD_OP] = "ADD_OP",
            [E_SUB_OP] = "SUB_OP",
            [E_MUL_OP] = "MUL_OP",
            [E_DIV_OP] = "DIV_OP",
            [E_MOD_OP] = "MOD_OP",
            [E_BIT_AND] = "BIT_AND",
            [E_BIT_OR] = "BIT_OR",
            [E_BIT_XOR] = "BIT_XOR",
            [E_LEFT_SHIFT] = "LEFT_SHIFT",
            [E_RIGHT_SHIFT] = "RIGHT_SHIFT",
            [E_NOT_OP] = "NOT_OP",
            [E_BIT_NOT_OP] = "BIT_NOT_OP",
            [E_ASSIGN_OP] = "ASSIGN_OP",
            [E_ADD_ASSIGN_OP] = "ADD_ASSIGN_OP",
            [E_SUB_ASSIGN_OP] = "SUB_ASSIGN_OP",
            [E_MUL_ASSIGN_OP] = "MUL_ASSIGN_OP",
            [E_DIV_ASSIGN_OP] = "DIV_ASSIGN_OP",
            [E_MOD_ASSIGN_OP] = "MOD_ASSIGN_OP",
            [E_BIT_AND_ASSIGN] = "BIT_AND_ASSIGN",
            [E_BIT_OR_ASSIGN] = "BIT_OR_ASSIGN",
            [E_BIT_XOR_ASSIGN] = "BIT_XOR_ASSIGN",
            [E_LEFT_SHIFT_ASSIGN] = "LEFT_SHIFT_ASSIGN",
            [E_RIGHT_SHIFT_ASSIGN] = "RIGHT_SHIFT_ASSIG",
        };

    if (depth == 0)
    {
        printf("Exprs:");
    }
    for (int i = 0; i < exprs.size; ++i)
    {
        putchar('\n');
        indent(depth);
        printf("%s", expr_names[exprs.array[i].kind]);
        print_expr(exprs.array[i], depth);
    }
    putchar('\n');
    if (depth == 0)
    {
        putchar('\n');
    }
}

ExprVector parse(TokenVector tokens)
{
    ExprVector exprs = expr_vector_new(10);

    Expr expr;
    for (int i = 0; i <= E_RIGHT_SHIFT_ASSIGN; ++i)
    {
        expr.kind = i;
        if (i == 1)
        {
            expr.u.type.children = expr_vector_new(1);
            Expr temp;
            temp.kind = E_TYPE;
            expr_vector_push(&expr.u.type.children, temp);
        }
        expr_vector_push(&exprs, expr);
    }

    print_expr_vector(exprs, 0);
}

ExprVector lex_parse_recursive(char *path, int length)
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
                lex_parse_recursive(new_path, length + new_length);
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
    ExprVector ast = lex_parse_recursive("./src/", 6);

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
// match on enums (exhaustive)
// if elif else
// do while do-while
// break return next
// all ops
// mod support

//////////////////////////////

// lex - DONE
// parse
// gen
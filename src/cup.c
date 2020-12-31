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
        v.array = malloc(c * sizeof(type));                 \
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
        memset(v->array + v->size, 0, sizeof(type));        \
    }

VECTOR(String, string, char);

typedef struct Location
{
    int line;
    int column;
} Location;

char *current_file_name;
String current_file;

Location get_location(int index)
{
    Location loc;
    loc.line = 1;
    loc.column = 1;
    for (int i = 0; i < index; ++i)
    {
        if (current_file.array[i] == '\n')
        {
            ++loc.line;
            loc.column = 1;
        }
        else
        {
            ++loc.column;
        }
    }
    return loc;
}

#ifdef _WIN32
#include <windows.h>
#define COLOR(c) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c)
#define RESET 7
#define MAGENTA 5
#define GREEN 10
#define RED 12
#else
#define COLOR(c) printf(c)
#define RESET "\033[0m"
#define MAGENTA "\033[35m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#endif

void print_snippet(Location location)
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
    for (int i = 0; i < current_file.size; ++i)
    {
        char c = current_file.array[i];
        if (c == '\n')
        {
            if (++line_index > location.line)
            {
                break;
            }
        }
        else if (line_index == location.line)
        {
            putchar(c);
        }
    }
    putchar('\n');
    for (int i = 0; i < length; ++i)
    {
        putchar(' ');
    }
    putchar('|');
    for (int i = 0; i < location.column; ++i)
    {
        putchar(' ');
    }
    COLOR(RED);
    putchar('^');
    COLOR(RESET);
    putchar('\n');
}

#define THROW(index, error, ...)                                       \
    {                                                                  \
        Location loc = get_location(index);                            \
        printf("%s:%i:%i: ", current_file_name, loc.line, loc.column); \
        COLOR(RED);                                                    \
        printf("error: ");                                             \
        COLOR(RESET);                                                  \
        printf(error "\n", __VA_ARGS__);                               \
        print_snippet(loc);                                            \
        exit(1);                                                       \
    }

typedef enum
{
    IDENT,
    STRING_LIT,
    CHAR_LIT,
    INT_LIT,
    FLOAT_LIT,
    TAG,
    MOD,
    USE,
    STRUCT,
    ENUM,
    UNION,
    PROP,
    DEF,
    SUB,
    THIS,
    TYPE,
    WHERE,
    AS,

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
    CASE,
    BACK,
    NEXT,
    DELAY,
    JUMP,

    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    ARROW,
    QUESTION_MARK,
    BACKTICK,
    HASH,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,

    DEREF,
    ADDRESS,
    RANGE,
    RANGE_INCL,
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
    SUBTRACT,
    SUBTRACT_ASSIGN,
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
    unsigned int index;
} Token;

VECTOR(TokenVector, token_vector, Token);

void print_token_vector(TokenVector tokens)
{
    const char *const token_names[] =
        {
            [IDENT] = "IDENT",
            [STRING_LIT] = "STRING_LIT",
            [CHAR_LIT] = "CHAR_LIT",
            [INT_LIT] = "INT_LIT",
            [FLOAT_LIT] = "FLOAT_LIT",
            [TAG] = "TAG",
            [MOD] = "MOD",
            [USE] = "USE",
            [STRUCT] = "STRUCT",
            [ENUM] = "ENUM",
            [UNION] = "UNION",
            [PROP] = "PROP",
            [DEF] = "DEF",
            [SUB] = "SUB",
            [THIS] = "THIS",
            [TYPE] = "TYPE",
            [WHERE] = "WHERE",
            [AS] = "AS",

            [_CONST] = "CONST",
            [_TRUE] = "TRUE",
            [_FALSE] = "FALSE",
            [_NULL] = "NULL",

            [IF] = "IF",
            [ELIF] = "ELIF",
            [ELSE] = "ELSE",
            [DO] = "DO",
            [WHILE] = "WHILE",
            [FOR] = "FOR",
            [_IN] = "IN",
            [MATCH] = "MATCH",
            [CASE] = "CASE",
            [BACK] = "BACK",
            [NEXT] = "NEXT",
            [DELAY] = "DELAY",
            [JUMP] = "JUMP",

            [SEMICOLON] = "SEMICOLON",
            [COLON] = "COLON",
            [COMMA] = "COMMA",
            [DOT] = "DOT",
            [ARROW] = "ARROW",
            [QUESTION_MARK] = "QUESTION_MARK",
            [BACKTICK] = "BACKTICK",
            [HASH] = "HASH",
            [LEFT_PAREN] = "LEFT_PAREN",
            [RIGHT_PAREN] = "RIGHT_PAREN",
            [LEFT_BRACE] = "LEFT_BRACE",
            [RIGHT_BRACE] = "RIGHT_BRACE",
            [LEFT_BRACKET] = "LEFT_BRACKET",
            [RIGHT_BRACKET] = "RIGHT_BRACKET",

            [DEREF] = "DEREF",
            [ADDRESS] = "ADDRESS",
            [RANGE] = "RANGE",
            [RANGE_INCL] = "RANGE_INCL",
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
            [SUBTRACT] = "SUBTRACT",
            [SUBTRACT_ASSIGN] = "SUBTRACT_ASSIGN",
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

    printf("Tokens:\n");
    for (int i = 0; i < tokens.size; ++i)
    {
        TokenKind kind = tokens.array[i].kind;
        COLOR(MAGENTA);
        printf("  %s", token_names[kind]);
        COLOR(RESET);
        if (kind == IDENT || kind == STRING_LIT || kind == CHAR_LIT || kind == INT_LIT || kind == FLOAT_LIT)
        {
            printf("(%s)", tokens.array[i].value);
        }
        putchar('\n');
    }
    putchar('\n');
}

const int const token_lengths[] =
    {
        [TAG] = 3,
        [MOD] = 3,
        [USE] = 3,
        [STRUCT] = 6,
        [ENUM] = 4,
        [UNION] = 5,
        [PROP] = 4,
        [DEF] = 3,
        [SUB] = 3,
        [THIS] = 4,
        [TYPE] = 4,
        [WHERE] = 5,
        [AS] = 2,

        [_CONST] = 5,
        [_TRUE] = 4,
        [_FALSE] = 5,
        [_NULL] = 4,

        [IF] = 2,
        [ELIF] = 4,
        [ELSE] = 4,
        [DO] = 2,
        [WHILE] = 5,
        [FOR] = 3,
        [_IN] = 2,
        [MATCH] = 5,
        [CASE] = 4,
        [BACK] = 4,
        [NEXT] = 4,
        [DELAY] = 5,
        [JUMP] = 4,

        [SEMICOLON] = 1,
        [COLON] = 1,
        [COMMA] = 1,
        [DOT] = 1,
        [ARROW] = 2,
        [QUESTION_MARK] = 1,
        [BACKTICK] = 1,
        [HASH] = 1,
        [LEFT_PAREN] = 1,
        [RIGHT_PAREN] = 1,
        [LEFT_BRACE] = 1,
        [RIGHT_BRACE] = 1,
        [LEFT_BRACKET] = 1,
        [RIGHT_BRACKET] = 1,

        [DEREF] = 1,
        [ADDRESS] = 1,
        [RANGE] = 2,
        [RANGE_INCL] = 3,
        [ASSIGN] = 1,
        [EQUAL] = 2,
        [NOT_EQUAL] = 2,
        [NOT] = 1,
        [AND] = 2,
        [OR] = 2,
        [LESS] = 1,
        [LESS_EQUAL] = 2,
        [GREATER] = 1,
        [GREATER_EQUAL] = 2,
        [ADD] = 1,
        [ADD_ASSIGN] = 2,
        [SUBTRACT] = 1,
        [SUBTRACT_ASSIGN] = 2,
        [MUL] = 1,
        [MUL_ASSIGN] = 2,
        [DIV] = 1,
        [DIV_ASSIGN] = 2,
        [MODULO] = 1,
        [MODULO_ASSIGN] = 2,
        [BIT_NOT] = 1,
        [BIT_AND] = 1,
        [BIT_AND_ASSIGN] = 2,
        [BIT_OR] = 1,
        [BIT_OR_ASSIGN] = 2,
        [BIT_XOR] = 1,
        [BIT_XOR_ASSIGN] = 2,
        [LEFT_SHIFT] = 2,
        [LEFT_SHIFT_ASSIGN] = 3,
        [RIGHT_SHIFT] = 2,
        [RIGHT_SHIFT_ASSIGN] = 3};

Token value_token(String value, int index)
{
    Token token;
    token.value = malloc(value.size + 1);
    memcpy(token.value, value.array, value.size);
    token.value[value.size] = '\0';
    token.index = index - value.size;
    return token;
}

TokenVector lex(String input)
{
    TokenVector tokens = token_vector_new(input.size / 4);
    char is_comment = -1;
    char is_literal = 0;
    String value = string_new(32);

    for (int i = 0; i <= input.size; ++i)
    {
        char c = input.array[i];

        if (is_comment != -1)
        {
            if (is_comment == 0)
            {
                if (c == '\n')
                {
                    is_comment = -1;
                }
            }
            else
            {
                if (c == '/' && i + 1 < input.size && input.array[i + 1] == '*')
                {
                    ++is_comment;
                    ++i;
                }
                else if (c == '*' && i + 1 < input.size && input.array[i + 1] == '/')
                {
                    if (--is_comment == 0)
                    {
                        is_comment = -1;
                    }
                    ++i;
                }
            }
            continue;
        }

        TokenKind kind = -1;

        if (is_literal == 3 || is_literal == 4)
        {
            is_literal -= 2;
        }

        if (is_literal == 1 || is_literal == 2)
        {
            if (c == '\0')
            {
                THROW(i - 1, "expected end of %s literal", is_literal == 1 ? "string" : "char");
            }
            else if (is_literal == 1 && c == '"')
            {
                goto end_string;
            }
            else if (is_literal == 2 && c == '\'')
            {
                goto end_char;
            }
            else
            {
                goto push;
            }
        }

        if (isspace(c) || c == '\0')
        {
            kind = 0;
        }
        else
        {
            switch (c)
            {
            case '"':
                kind = 0;
                if (is_literal == 1)
                {
                end_string:
                    kind = STRING_LIT;
                    is_literal = 0;
                }
                else
                {
                    is_literal = 3;
                }
                break;
            case '\'':
                kind = 0;
                if (is_literal == 2)
                {
                end_char:
                    kind = CHAR_LIT;
                    is_literal = 0;
                }
                else
                {
                    is_literal = 4;
                }
                break;
            case ';':
                kind = SEMICOLON;
                break;
            case ':':
                if (value.size && i + 1 < input.size && input.array[i + 1] == ':' && i + 2 < input.size && (input.array[i + 2] == '_' || isalpha(input.array[i + 2])))
                {
                    ++i;
                }
                else
                {
                    kind = COLON;
                }
                break;
            case ',':
                kind = COMMA;
                break;
            case '?':
                kind = QUESTION_MARK;
                break;
            case '`':
                kind = BACKTICK;
                break;
            case '#':
                kind = HASH;
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
                kind = LEFT_BRACKET;
                break;
            case ']':
                kind = RIGHT_BRACKET;
                break;
            case '.':
                if (i + 1 < input.size && input.array[i + 1] == '.')
                {
                    if (i + 2 < input.size && input.array[i + 2] == '.')
                    {
                        kind = RANGE_INCL;
                        i += 2;
                    }
                    else
                    {
                        kind = RANGE;
                        ++i;
                    }
                }
                else if (is_literal != 5 || (i + 1 < input.size && (input.array[i + 1] == '_' || isalpha(input.array[i + 1]))))
                {
                    kind = DOT;
                }
                break;
            case '=':
                if (i + 1 < input.size)
                {
                    if (input.array[i + 1] == '=')
                    {
                        kind = EQUAL;
                        ++i;
                    }
                    else
                    {
                        kind = ASSIGN;
                    }
                }
                else
                {
                    kind = ASSIGN;
                }
                break;
            case '@':
                kind = DEREF;
                break;
            case '$':
                kind = ADDRESS;
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
                        kind = ARROW;
                        ++i;
                        break;
                    case '=':
                        kind = SUBTRACT_ASSIGN;
                        ++i;
                        break;
                    default:
                        kind = SUBTRACT;
                    }
                }
                else
                {
                    kind = SUBTRACT;
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
                if (i + 1 < input.size)
                {
                    switch (input.array[i + 1])
                    {
                    case '=':
                        kind = DIV_ASSIGN;
                        ++i;
                        break;
                    case '/':
                        is_comment = 0;
                        kind = 0;
                        ++i;
                        break;
                    case '*':
                        is_comment = 1;
                        kind = 0;
                        ++i;
                        break;
                    default:
                        kind = DIV;
                        break;
                    }
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
                            ++i;
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
                                ++i;
                                ;
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
            if (c == '_' || c == '.' || c == ':' || isalnum(c))
            {
                if (value.size == 0 && isdigit(c))
                {
                    is_literal = 5;
                }
                else if (is_literal == 5 || is_literal == 6)
                {
                    if (c == '.' && is_literal == 5)
                    {
                        is_literal = 6;
                        goto check;
                    }
                    else if (c == '_' || isdigit(c))
                    {
                        goto check;
                    }

                    THROW(i - value.size, "invalid identifier name starting with a digit", 0);
                }

            check:
                if ((is_literal != 5 && is_literal != 6) || c != '_')
                {
                push:
                    if (is_literal == 2 && value.size == 4)
                    {
                        THROW(i, "too many characters in character literal", 0);
                    }
                    string_push(&value, c);
                }
            }
            else
            {
                THROW(i, "Unexpected symbol '%c'", c);
            }
        }
        else
        {
            if (value.size && kind != STRING_LIT && kind != CHAR_LIT)
            {
                TokenKind value_kind = -1;

                value.array[value.size] = '\0';

                if (is_literal == 5 || is_literal == 6)
                {
                }
                else if (strcmp(value.array, "tag") == 0)
                {
                    value_kind = TAG;
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
                else if (strcmp(value.array, "prop") == 0)
                {
                    value_kind = PROP;
                }
                else if (strcmp(value.array, "def") == 0)
                {
                    value_kind = DEF;
                }
                else if (strcmp(value.array, "sub") == 0)
                {
                    value_kind = SUB;
                }
                else if (strcmp(value.array, "this") == 0)
                {
                    value_kind = THIS;
                }
                else if (strcmp(value.array, "type") == 0)
                {
                    value_kind = TYPE;
                }
                else if (strcmp(value.array, "where") == 0)
                {
                    value_kind = WHERE;
                }
                else if (strcmp(value.array, "AS") == 0)
                {
                    value_kind = AS;
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
                else if (strcmp(value.array, "case") == 0)
                {
                    value_kind = CASE;
                }
                else if (strcmp(value.array, "back") == 0)
                {
                    value_kind = BACK;
                }
                else if (strcmp(value.array, "next") == 0)
                {
                    value_kind = NEXT;
                }
                else if (strcmp(value.array, "delay") == 0)
                {
                    value_kind = DELAY;
                }
                else if (strcmp(value.array, "jump") == 0)
                {
                    value_kind = JUMP;
                }

                if (value_kind == -1)
                {
                    Token token = value_token(value, i);
                    switch (is_literal)
                    {
                    case 5:
                        token.kind = INT_LIT;
                        break;
                    case 6:
                        token.kind = FLOAT_LIT;
                        if (value.array[value.size - 1] == '.')
                        {
                            THROW(i - 1, "expected a value after the decimal point", 0);
                        }
                        break;
                    default:
                        token.kind = IDENT;
                        break;
                    }
                    token_vector_push(&tokens, token);
                }
                else
                {
                    Token token;
                    token.kind = value_kind;
                    token.index = i - token_lengths[value_kind];
                    token_vector_push(&tokens, token);
                }

                value.size = 0;
                is_literal = 0;
            }

            if (kind)
            {
                Token token;
                if (kind == STRING_LIT || kind == CHAR_LIT)
                {
                    token = value_token(value, i);
                    value.size = 0;
                }
                else
                {
                    token.index = i - token_lengths[kind] + 1;
                }
                token.kind = kind;
                token_vector_push(&tokens, token);
            }
        }
    }

    free(value.array);

    return tokens;
}

/*

    literal = string_lit + arr_lit + num_lit + bool_lit + char_lit + null_lit + self_lit
    value = op + fn_call + var_use + literal
    local = for + do + if + elif + else + while + switch + fall + break + next + goto + return + deref + local_var_def + value
    global = use + mod + struct + enum + union + trait + impl + fn_def + var_def

    tag:
        - name: string
        - data: arr<literal>

    type:
        - const: bool
        - name: string
        - child: arr<type>

    constr_type:
        - name: string
        - constr: arr<type>

    (pub) mod:
        - pub: bool 
        - name: string
        - body: arr<global>

    use:
        - name: string

    (pub) (generic) struct:
        - pub: bool
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>
    
    (pub) field:
        - pub: bool 
        - name: string
        - type: type

    (pub) (generic) enum:
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<option>

    (pub) option:
        - pub: bool 
        - name: string
        - body: arr<field>

    (pub) (generic) union:
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>

    (pub) (generic) trait:
        - pub: bool
        - name: string
        - gen: arr<constr_type>
        - body: arr<fn_def>

    (pub) (generic) impl:
        - pub: bool
        - gen: arr<constr_type>
        - trait: type
        - target: type
        - body: arr<fn_def>

    (pub) (inl) (macro) (generic) fn_def:
        - pub: bool
        - inl: bool
        - macro: bool
        - name: string
        - gen: arr<constr_type>
        - args: arr<arg>
        - ret_type: type
        - body: arr<local>

    (rest) arg:
        rest: bool
        name: string
        type: type

    (pub) (inl) (combo) var_def:
        - pub: bool
        - inl: bool
        - name: string
        - type: type 
        - value: value

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

    char_lit:
        - value: char

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

    switch:
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

    deref_op:
    address_op:
    not_op:
    bit_not:
        - val: value

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
    E_TAG,
    E_TYPE,
    E_CONSTR_TYPE,
    E_TAG_DEF,
    E_MOD,
    E_USE,
    E_STRUCT,
    E_UNION,
    E_FIELD,
    E_ENUM,
    E_OPTION,
    E_OPTION_FIELD,
    E_PROP,
    E_DEF,
    E_SUB_DEF,
    E_ARG,
    E_VAR_DEF,
    E_LOCAL_VAR_DEF,
    E_SUB_CALL,
    E_VAR_USE,
    E_STRUCT_INST,
    E_FIELD_VAL,
    E_STRING,
    E_CHAR,
    E_ARR,
    E_INT,
    E_FLOAT,
    E_BOOL,
    E_NULL,
    E_THIS,
    E_TYPE,
    E_WHERE,
    E_DO,
    E_BLOCK,
    E_AS,
    E_IF,
    E_ELIF,
    E_ELSE,
    E_WHILE,
    E_FOR,
    E_MATCH,
    E_CASE,
    E_BACK,
    E_NEXT,
    E_DELAY,
    E_JUMP,
    E_COND_OP,
    E_RANGE_OP,
    E_RANGE_INCL_OP,
    E_EQUAL_OP,
    E_NOT_EQUAL_OP,
    E_AND_OP,
    E_OR_OP,
    E_LESS_OP,
    E_LESS_EQUAL_OP,
    E_GREATER_OP,
    E_GREATER_EQUAL_OP,
    E_ADD_OP,
    E_SUBTRACT_OP,
    E_MUL_OP,
    E_DIV_OP,
    E_MOD_OP,
    E_BIT_AND,
    E_BIT_OR,
    E_BIT_XOR,
    E_LEFT_SHIFT,
    E_RIGHT_SHIFT,
    E_DEREF_OP,
    E_ADDRESS_OP,
    E_NEGATION_OP,
    E_NOT_OP,
    E_BIT_NOT_OP,
    E_ASSIGN_OP,
    E_ADD_ASSIGN_OP,
    E_SUBTRACT_ASSIGN_OP,
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
    ExprVector args;
} Tag;

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
    char pub;
    char *name;
    ExprVector body;
} Mod;

typedef struct
{
    char *name;
} Use;

typedef struct
{
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
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Enum;

typedef struct
{
    char *name;
    ExprVector body;
} Option;

typedef struct
{
    char *name;
    Expr *type;
} OptionField;

typedef struct
{
    char pub;
    char *name;
    ExprVector gen;
    ExprVector body;
} Trait;

typedef struct
{
    char pub;
    ExprVector gen;
    Expr *trait;
    Expr *target;
    ExprVector body;
} Impl;

typedef struct
{
    char pub;
    char inl;
    char macro;
    char *name;
    ExprVector gen;
    ExprVector args;
    Expr *ret_type;
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
    char pub;
    char inl;
    char *name;
    Expr *type;
    Expr *value;
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
    char *value;
} StringLit;

typedef struct
{
    ExprVector value;
} ArrLit;

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
} Switch;

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
    Expr *con;
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
    Expr *val;
} UnaryOp;

typedef union
{
    Tag tag;
    Type type;
    ConstrType constr_type;
    Mod mod;
    Use use;
    Struct _struct;
    Struct _union;
    Field field;
    Enum _enum;
    Option option;
    OptionField option_field;
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
    StringLit char_lit;
    StringLit int_lit;
    StringLit float_lit;
    ArrLit arr_lit;
    BoolLit bool_lit;
    If _if;
    Elif elif;
    Else _else;
    While _while;
    For _for;
    Switch _switch;
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
    ExprVector tags;
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
#define PRINT_OPT_EXPR_VECTOR(vector, name, next) \
    if (vector.size)                              \
    {                                             \
        if (next)                                 \
        {                                         \
            printf(", ");                         \
        }                                         \
        printf(name " = [");                      \
        print_expr_vector(vector, depth + 1);     \
        indent(depth);                            \
        putchar(']');                             \
    }

    putchar('(');

    PRINT_OPT_EXPR_VECTOR(expr.tags, "tags", 0)
    if (expr.tags.size)
    {
        printf(", ");
    }

    switch (expr.kind)
    {
    case E_TAG:
        printf("name = %s", expr.u.tag.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.tag.args, "args", 1)
        break;
    case E_TYPE:
        printf("const = %i, name = %s", expr.u.type._const, expr.u.type.name);
        if (expr.u.type.children.size)
        {
            printf(", children = [");
            print_expr_vector(expr.u.type.children, depth + 1);
            indent(depth);
            putchar(']');
        }
        break;
    case E_CONSTR_TYPE:
        printf("name = %s, constr = [", expr.u.constr_type.name);
        print_expr_vector(expr.u.constr_type.constr, depth + 1);
        indent(depth);
        putchar(']');
        break;
    case E_MOD:
        printf("pub = %i, name = %s", expr.u.mod.pub, expr.u.mod.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.mod.body, "body", 1)
        break;
    case E_USE:
        printf("name = %s", expr.u.use.name);
        break;
    case E_STRUCT:
    case E_UNION:
        printf("pub = %i, name = %s", expr.u._struct.pub, expr.u._struct.name);
        PRINT_OPT_EXPR_VECTOR(expr.u._struct.gen, "gen", 1)
        PRINT_OPT_EXPR_VECTOR(expr.u._struct.body, "body", 1)
        break;
    case E_FIELD:
        printf("pub = %i, name = %s, type = ", expr.u.field.pub, expr.u.field.name);
        print_expr(*expr.u.field.type, depth);
        break;
    case E_ENUM:
        printf("pub = %i, name = %s", expr.u._enum.pub, expr.u._enum.name);
        PRINT_OPT_EXPR_VECTOR(expr.u._enum.gen, "gen", 1)
        PRINT_OPT_EXPR_VECTOR(expr.u._enum.body, "body", 1)
        break;
    case E_OPTION:
        printf("name = %s", expr.u.option.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.option.body, "body", 1)
        break;
    case E_OPTION_FIELD:
        printf("name = %s, type = ", expr.u.option_field.name);
        print_expr(*expr.u.option_field.type, depth);
        break;
    case E_TRAIT:
        printf("pub = %i, name = %s", expr.u.trait.pub, expr.u.trait.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.trait.gen, "gen", 1)
        PRINT_OPT_EXPR_VECTOR(expr.u.trait.body, "body", 1)
        break;
    case E_IMPL:
        printf("pub = %i", expr.u.impl.pub);
        PRINT_OPT_EXPR_VECTOR(expr.u.impl.gen, "gen", 1)
        printf(", type = ");
        print_expr(*expr.u.impl.trait, depth);
        printf(", target = ");
        print_expr(*expr.u.impl.target, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.impl.body, "body", 1)
        break;
    case E_FN_DEF:
        printf("pub = %i, inl = %i, macro = %i, name = %s", expr.u.fn_def.pub, expr.u.fn_def.inl, expr.u.fn_def.macro, expr.u.fn_def.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.fn_def.gen, "gen", 1)
        PRINT_OPT_EXPR_VECTOR(expr.u.fn_def.args, "args", 1)
        printf(", ret_type = ");
        print_expr(*expr.u.fn_def.ret_type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.fn_def.body, "body", 1)
        break;
    case E_ARG:
        printf("rest = %i, name = %s, type = ", expr.u.arg.rest, expr.u.arg.name);
        print_expr(*expr.u.arg.type, depth);
        break;
    case E_VAR_DEF:
        printf("pub = %i, inl = %i, name = %s", expr.u.var_def.pub, expr.u.var_def.inl, expr.u.var_def.name);
        if (expr.u.var_def.type)
        {
            printf(", type = ");
            print_expr(*expr.u.var_def.type, depth);
        }
        if (expr.u.var_def.value)
        {
            printf(", value = ");
            print_expr(*expr.u.var_def.value, depth);
        }
        break;
    case E_LOCAL_VAR_DEF:
        printf("name = %s", expr.u.local_var_def.name);
        if (expr.u.local_var_def.type)
        {
            printf(", type = ");
            print_expr(*expr.u.var_def.type, depth);
        }
        if (expr.u.local_var_def.value)
        {
            printf(", value = ");
            print_expr(*expr.u.local_var_def.value, depth);
        }
        break;
    case E_FN_CALL:
        printf("type = ");
        print_expr(*expr.u.fn_call.type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.fn_call.args, "args", 1)
        break;
    case E_VAR_USE:
        printf("name = %s", expr.u.var_use.name);
        break;
    case E_STRUCT_INST:
        printf("type = ");
        print_expr(*expr.u.struct_inst.type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.fn_call.args, "args", 1)
        break;
    case E_FIELD_VAL:
        printf("name = %s, value = ", expr.u.field_val.name);
        print_expr(*expr.u.field_val.value, depth);
        break;
    case E_DO:
    case E_BLOCK:
        PRINT_OPT_EXPR_VECTOR(expr.u._do.body, "body", expr.tags.size)
        break;
    case E_ARR_LIT:
        PRINT_OPT_EXPR_VECTOR(expr.u.arr_lit.value, "value", expr.tags.size)
        break;
    case E_STRING_LIT:
    case E_CHAR_LIT:
    case E_INT_LIT:
    case E_FLOAT_LIT:
        printf("value = %s", expr.u.string_lit.value);
        break;
    case E_BOOL_LIT:
        printf("value = %i", expr.u.bool_lit.value);
        break;
    case E_IF:
    case E_ELIF:
    case E_ELSE:
    case E_WHILE:
        printf("con = ");
        print_expr(*expr.u._if.con, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u._if.body, "body", 1)
        break;
    case E_FOR:
        if (expr.u._for.loop_var)
        {
            printf("loop_var = %s, ", expr.u._for.loop_var);
        }
        printf("range = ");
        print_expr(*expr.u._for.range, depth);
        break;
    case E_SWITCH:
        printf("value = ");
        print_expr(*expr.u._switch.value, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u._switch.body, "body", 1)
        break;
    case E_CASE:
        printf("value = ");
        print_expr(*expr.u._case.value, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u._case.body, "body", 1)
        break;
    case E_BREAK:
    case E_NEXT:
    case E_GOTO:
        if (expr.u._break.label)
        {
            printf("label = %s", expr.u._break.label);
        }
        break;
    case E_RETURN:
        if (expr.u._return.value)
        {
            printf("value = ");
            print_expr(*expr.u._return.value, depth);
        }
        break;
    case E_DEFER:
        PRINT_OPT_EXPR_VECTOR(expr.u.defer.body, "body", expr.tags.size)
        break;
    case E_COND_OP:
        printf("con = ");
        print_expr(*expr.u.cond_op.con, depth);
        printf(", lhs = ");
        print_expr(*expr.u.cond_op.lhs, depth);
        if (expr.u.cond_op.rhs)
        {
            printf(", rhs = ");
            print_expr(*expr.u.cond_op.rhs, depth);
        }
        break;
    case E_RANGE_OP:
    case E_RANGE_INCL_OP:
    case E_EQUAL_OP:
    case E_NOT_EQUAL_OP:
    case E_AND_OP:
    case E_OR_OP:
    case E_LESS_OP:
    case E_LESS_EQUAL_OP:
    case E_GREATER_OP:
    case E_GREATER_EQUAL_OP:
    case E_ADD_OP:
    case E_SUBTRACT_OP:
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
    case E_SUBTRACT_ASSIGN_OP:
    case E_MUL_ASSIGN_OP:
    case E_DIV_ASSIGN_OP:
    case E_MOD_ASSIGN_OP:
    case E_BIT_AND_ASSIGN:
    case E_BIT_OR_ASSIGN:
    case E_BIT_XOR_ASSIGN:
    case E_LEFT_SHIFT_ASSIGN:
    case E_RIGHT_SHIFT_ASSIGN:
        printf("lhs = ");
        print_expr(*expr.u.binary_op.lhs, depth);
        printf(", rhs = ");
        print_expr(*expr.u.binary_op.rhs, depth);
        break;
    case E_DEREF_OP:
    case E_ADDRESS_OP:
    case E_NEGATION_OP:
    case E_NOT_OP:
    case E_BIT_NOT_OP:
        printf("val = ");
        print_expr(*expr.u.unary_op.val, depth);
        break;
    }
    putchar(')');
}

void print_expr_vector(ExprVector exprs, int depth)
{
    const char *const expr_names[] =
        {
            [E_TAG] = "TAG",
            [E_TYPE] = "TYPE",
            [E_CONSTR_TYPE] = "CONSTR_TYPE",
            [E_MOD] = "MOD",
            [E_USE] = "USE",
            [E_STRUCT] = "STRUCT",
            [E_UNION] = "UNION",
            [E_FIELD] = "FIELD",
            [E_ENUM] = "ENUM",
            [E_OPTION] = "OPTION",
            [E_OPTION_FIELD] = "OPTION_FIELD",
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
            [E_CHAR_LIT] = "CHAR_LIT",
            [E_ARR_LIT] = "ARR_LIT",
            [E_INT_LIT] = "INT_LIT",
            [E_FLOAT_LIT] = "FLOAT_LIT",
            [E_BOOL_LIT] = "BOOL_LIT",
            [E_CHAR_LIT] = "CHAR_LIT",
            [E_NULL_LIT] = "NULL_LIT",
            [E_SELF_LIT] = "SELF_LIT",
            [E_IF] = "IF",
            [E_ELIF] = "ELIF",
            [E_ELSE] = "ELSE",
            [E_WHILE] = "WHILE",
            [E_FOR] = "FOR",
            [E_SWITCH] = "SWITCH",
            [E_CASE] = "CASE",
            [E_FALL] = "FALL",
            [E_BREAK] = "BREAK",
            [E_NEXT] = "NEXT",
            [E_GOTO] = "GOTO",
            [E_RETURN] = "RETURN",
            [E_DEFER] = "DEFER",
            [E_COND_OP] = "COND_OP",
            [E_RANGE_OP] = "RANGE_OP",
            [E_RANGE_INCL_OP] = "RANGE_INCL_OP",
            [E_EQUAL_OP] = "EQUAL_OP",
            [E_NOT_EQUAL_OP] = "NOT_EQUAL_OP",
            [E_AND_OP] = "AND_OP",
            [E_OR_OP] = "OR_OP",
            [E_LESS_OP] = "LESS_OP",
            [E_LESS_EQUAL_OP] = "LESS_EQUAL_OP",
            [E_GREATER_OP] = "GREATER_OP",
            [E_GREATER_EQUAL_OP] = "GREATER_EQUAL_OP",
            [E_ADD_OP] = "ADD_OP",
            [E_SUBTRACT_OP] = "SUBTRACT_OP",
            [E_MUL_OP] = "MUL_OP",
            [E_DIV_OP] = "DIV_OP",
            [E_MOD_OP] = "MOD_OP",
            [E_BIT_AND] = "BIT_AND",
            [E_BIT_OR] = "BIT_OR",
            [E_BIT_XOR] = "BIT_XOR",
            [E_LEFT_SHIFT] = "LEFT_SHIFT",
            [E_RIGHT_SHIFT] = "RIGHT_SHIFT",
            [E_DEREF_OP] = "DEREF_OP",
            [E_ADDRESS_OP] = "ADDRESS_OP",
            [E_NEGATION_OP] = "NEGATION_OP",
            [E_NOT_OP] = "NOT_OP",
            [E_BIT_NOT_OP] = "BIT_NOT_OP",
            [E_ASSIGN_OP] = "ASSIGN_OP",
            [E_ADD_ASSIGN_OP] = "ADD_ASSIGN_OP",
            [E_SUBTRACT_ASSIGN_OP] = "SUBTRACT_ASSIGN_OP",
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
        COLOR(GREEN);
        printf("%s", expr_names[exprs.array[i].kind]);
        COLOR(RESET);
        print_expr(exprs.array[i], depth);
    }
    putchar('\n');
    if (depth == 0)
    {
        putchar('\n');
    }
}

Expr parse_local()
{
    Expr expr;

    return expr;
}

ExprVector parse_local_block()
{
    ExprVector exprs = expr_vector_new(8);

    return exprs;
}

ExprVector parse_tags(TokenVector tokens, int *index)
{
    ExprVector tags = expr_vector_new(2);

main:
    while (tokens.array[*index].kind == HASH)
    {
        Token token = tokens.array[++*index];
        if (token.kind == IDENT)
        {
            Expr tag;
            tag.kind = E_TAG;
            tag.tags.size = 0;
            tag.u.tag.name = token.value;
            tag.u.tag.args = expr_vector_new(2);

            token = tokens.array[++*index];
            if (token.kind == LEFT_PAREN)
            {
                while (1)
                {
                    token = tokens.array[++*index];
                    if (token.kind == RIGHT_PAREN)
                    {
                        break;
                    }

                    Expr lit;
                    lit.kind = -1;
                    lit.tags.size = 0;

                    switch (token.kind)
                    {
                    case STRING_LIT:
                        lit.kind = E_STRING_LIT;
                        lit.u.string_lit.value = token.value;
                        break;
                    case CHAR_LIT:
                        lit.kind = E_CHAR_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case INT_LIT:
                        lit.kind = E_INT_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case FLOAT_LIT:
                        lit.kind = E_FLOAT_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case LEFT_BRACKET:
                        lit.kind = E_ARR_LIT;
                        THROW(token.index, "unexpected array literal", 0);
                        break;
                    case _TRUE:
                    case _FALSE:
                        lit.kind = E_BOOL_LIT;
                        lit.u.bool_lit.value = token.kind == _TRUE;
                        break;
                    }

                    if (lit.kind != -1)
                    {
                        expr_vector_push(&tag.u.tag.args, lit);

                        token = tokens.array[++*index];
                        if (token.kind == COMMA)
                        {
                            continue;
                        }
                        else if (token.kind == RIGHT_PAREN)
                        {
                            break;
                        }
                    }
                    THROW(token.index, "unexpected smth in 'tag' declaration", 0);
                }
                expr_vector_push(&tags, tag);
                ++*index;
            }
            else
            {
                expr_vector_push(&tags, tag);
                goto main;
            }
        }
        else
        {
            THROW(tokens.array[*index - 1].index, "expected identifier after tag declaration", 0);
        }
    }

    return tags;
}

/// use
// USE
// IDENT
// SEMICOLON

/// mod
// MOD
// IDENT
// LEFT_BRACE
// [
//   GLOBAL
// ]
// RIGHT_BRACE

/// struct
// STRUCT
// IDENT
// LEFT_BRACE
// [
//   RIGHT_BRACE? -> end
//   IDENT
//   COLON
//   TYPE
//   COMMA?
// ]

/// enum
// ENUM
// IDENT
// LEFT_BRACE
// [
//   RIGHT_BRACE? -> end
//   IDENT
//   ???
//     LEFT_PAREN
//       [
//         RIGHT_PAREN? -> end
//         IDENT
//         COLON
//         TYPE
//         COMMA?
//       ]
//     RIGHT_PAREN
//   ???
//   COMMA?
// ]
// RIGHT_BRACE

Expr *parse_type(TokenVector tokens, int *index)
{
    Expr *expr = malloc(sizeof(Expr));
    expr->kind = E_TYPE;
    expr->tags = parse_tags(tokens, index);

    // actually parse the type
    expr->u.type._const = 0;
    expr->u.type.name = tokens.array[*index].value;
    expr->u.type.children = expr_vector_new(1);
    *index += 1;

    return expr;
}

#define NEXT_TOKEN token = tokens.array[++*index]

#define EXPECT_TOKEN(_kind, error, code) \
    if (token.kind != _kind)             \
    {                                    \
        THROW(token.index, error, 0);    \
    }                                    \
    code;                                \
    NEXT_TOKEN

#define OPTIONAL_TOKEN(_kind, true_code, false_code) \
    if (token.kind == _kind)                         \
    {                                                \
        NEXT_TOKEN;                                  \
        true_code;                                   \
    }                                                \
    else                                             \
    {                                                \
        false_code;                                  \
    }

ExprVector parse_global_block(TokenVector tokens, int *index);

Expr parse_global(TokenVector tokens, int *index)
{
    Expr expr;
    expr.tags = parse_tags(tokens, index);

    Token token = tokens.array[*index];

    OPTIONAL_TOKEN(PUB, expr.u.mod.pub = 1, expr.u.mod.pub = 0);

    switch (token.kind)
    {
    case USE:
        expr.kind = E_USE;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'use' keyword", expr.u.use.name = token.value);
        EXPECT_TOKEN(SEMICOLON, "expected ';' after 'use' path", {});
        break;
    case MOD:
        expr.kind = E_MOD;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'mod' keyword", expr.u.mod.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        expr.u.mod.body = parse_global_block(tokens, index);
        break;
    case STRUCT:
    case UNION:
        expr.kind = token.kind == STRUCT ? E_STRUCT : E_UNION;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'struct' keyword", expr.u._struct.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        expr.u._struct.body = expr_vector_new(4);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_BRACE, goto end_struct, {});
            Expr field;
            field.kind = E_FIELD;
            field.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            OPTIONAL_TOKEN(PUB, field.u.field.pub = 1, field.u.field.pub = 0);
            EXPECT_TOKEN(IDENT, "expected field name in 'struct' body", field.u.field.name = token.value);
            EXPECT_TOKEN(COLON, "expected ':' after field name", {});
            field.u.field.type = parse_type(tokens, index);
            token = tokens.array[*index];
            expr_vector_push(&expr.u._struct.body, field);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last field", {});
    end_struct:
        break;
    case ENUM:
        expr.kind = E_ENUM;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'enum' keyword", expr.u._enum.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'enum' name", {});
        expr.u._enum.body = expr_vector_new(4);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_BRACE, goto end_enum, {});
            Expr opt;
            opt.kind = E_OPTION;
            opt.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            EXPECT_TOKEN(IDENT, "expected option name in 'struct' body", opt.u.option.name = token.value);
            OPTIONAL_TOKEN(
                LEFT_PAREN,
                opt.u.option.body = expr_vector_new(2);
                while (1) {
                    OPTIONAL_TOKEN(RIGHT_PAREN, goto end_option, {});
                    Expr field;
                    field.kind = E_OPTION_FIELD;
                    field.tags = parse_tags(tokens, index);
                    token = tokens.array[*index];
                    EXPECT_TOKEN(IDENT, "expected field name in 'option' body", field.u.option_field.name = token.value);
                    EXPECT_TOKEN(COLON, "expected ':' after field name", {});
                    field.u.option_field.type = parse_type(tokens, index);
                    token = tokens.array[*index];
                    expr_vector_push(&opt.u.option.body, field);
                    OPTIONAL_TOKEN(COMMA, {}, break);
                };
                EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last field", {});
                end_option:
                    , opt.u.option.body.size = 0);
            expr_vector_push(&expr.u._enum.body, opt);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last option", {});
    end_enum:
        break;
    case IMPL:
        expr.kind = E_IMPL;
        NEXT_TOKEN;
        // EXPECT_TOKEN(IDENT, "expected identifier after 'impl' keyword", expr.u.impl.trait = token.value);
        // OPTIONAL_TOKEN(FOR, {}, {});
        break;
    default:
        THROW(token.index, "expected item in global scope", 0);
    }

    return expr;
}

ExprVector parse_global_block(TokenVector tokens, int *index)
{
    ExprVector exprs = expr_vector_new(32);

    char is_global = *index == 0;

    while (*index < tokens.size)
    {
        if (tokens.array[*index].kind == RIGHT_BRACE)
        {
            if (!is_global)
            {
                *index += 1;
                return exprs;
            }
            else
            {
                THROW(tokens.array[*index].index, "unexpected item in global scope", 0);
            }
        }

        expr_vector_push(&exprs, parse_global(tokens, index));
    }

    if (!is_global)
    {
        THROW(current_file.size, "expected a closing brace '}'", 0);
    }

    return exprs;
}

ExprVector lex_parse_recursive(char *path, int length)
{
    DIR *dir = opendir(path);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        int new_length = strlen(ent->d_name);
        switch (new_length)
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
            char *new_path = malloc(length + 1 + new_length + 1);
            memcpy(new_path, path, length);
            new_path[length] = '/';
            memcpy(new_path + length + 1, ent->d_name, new_length);
            new_path[length + new_length + 1] = '\0';
            switch (ent->d_type)
            {
            case DT_DIR:
                lex_parse_recursive(new_path, length + 1 + new_length);
                break;
            case DT_REG:
            {
                current_file_name = new_path;
                FILE *input;
                input = fopen(new_path, "rb");
                fseek(input, 0L, SEEK_END);
                current_file = string_new(ftell(input) + 1);
                rewind(input);
                current_file.size = current_file.capacity - 1;
                fread(current_file.array, current_file.size, 1, input);
                current_file.array[current_file.size] = '\0';
                fclose(input);
                TokenVector tokens = lex(current_file);
                print_token_vector(tokens);
                int index = 0;
                ExprVector exprs = parse_global_block(tokens, &index);
                print_expr_vector(exprs, 0);
                free(tokens.array);
                free(exprs.array);
                free(current_file.array);
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
    ExprVector ast = lex_parse_recursive("./src", 5);

    FILE *output;
    output = fopen("./bin/main.c", "w");
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
// switch on enums (exhaustive)
// if elif else
// do while
// break return next
// all ops
// mod support

//////////////////////////////

// lex - DONE
// parse
// gen
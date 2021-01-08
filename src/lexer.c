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
    VAR,
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
            [VAR] = "VAR",
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
        [VAR] = 3,
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
                else if (strcmp(value.array, "var") == 0)
                {
                    value_kind = VAR;
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
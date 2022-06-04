typedef enum TokenKind {
    NONE, // fake optional
    EMPTY, IDENT, TEXT, NUM,
    PAREN_L, PAREN_R,
    MEMBER, INDEX,
    M_REF, U_REF, OPT, FUN, ERR,
    S_CAST, U_CAST,
    ADD, SUB, MUL, DIV, REM,
    EQUAL, REF_EQUAL, AND, OR, LESS, LESS_EQUAL,
    ARG, TYPE,
    OBJ, ASSIGN, OBJ_ASSIGN, REF_ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, REM_ASSIGN,
    LABEL, NEW_LINE,
} TokenKind;

typedef struct Token {
    TokenKind kind;
    Str value;
    int index;
} Token;

Token token_of_kind (TokenKind kind, int index) {
    return (Token){ .kind = kind, .value = { .len = 0 }, .index = index };
}

Token token_of_kind_with_value (TokenKind kind, Str value, int index) {
    return (Token){ .kind = kind, .value = value, .index = index };
}

const char *const TOKEN_NAMES[] = {
    [EMPTY] = "EMPTY", [IDENT] = "IDENT", [TEXT] = "TEXT", [NUM] = "NUM",
    [PAREN_L] = "(", [PAREN_R] = ")",
    [MEMBER] = ":", [INDEX] = ".",
    [M_REF] = "@", [U_REF] = "#", [OPT] = "?", [FUN] = "|", [ERR] = "!",
    [S_CAST] = "[", [U_CAST] = "{",
    [ADD] = "+", [SUB] = "-", [MUL] = "*", [DIV] = "/", [REM] = "%",
    [EQUAL] = "=", [REF_EQUAL] = "@=", [AND] = "&", [OR] = "^", [LESS] = "<", [LESS_EQUAL] = "<=",
    [TYPE] = ",",
    [OBJ] = "$", [ASSIGN] = "~", [OBJ_ASSIGN] = "$~", [REF_ASSIGN] = "@~", [ADD_ASSIGN] = "+~", [SUB_ASSIGN] = "-~", [MUL_ASSIGN] = "*~", [DIV_ASSIGN] = "/~", [REM_ASSIGN] = "%~",
    [LABEL] = ";", [NEW_LINE] = "NEW_LINE",
};

const int TOKEN_LENGTHS[] = {
    [EMPTY] = 5, [IDENT] = 5, [TEXT] = 4, [NUM] = 3,
    [PAREN_L] = 1, [PAREN_R] = 1,
    [MEMBER] = 1, [INDEX] = 1,
    [M_REF] = 1, [U_REF] = 1, [OPT] = 1, [FUN] = 1, [ERR] = 1,
    [S_CAST] = 1, [U_CAST] = 1,
    [ADD] = 1, [SUB] = 1, [MUL] = 1, [DIV] = 1, [REM] = 1,
    [EQUAL] = 1, [REF_EQUAL] = 2, [AND] = 1, [OR] = 1, [LESS] = 1, [LESS_EQUAL] = 2,
    [TYPE] = 1,
    [OBJ] = 1, [ASSIGN] = 1, [OBJ_ASSIGN] = 2, [REF_ASSIGN] = 2, [ADD_ASSIGN] = 2, [SUB_ASSIGN] = 2, [MUL_ASSIGN] = 2, [DIV_ASSIGN] = 2, [REM_ASSIGN] = 2,
    [LABEL] = 1, [NEW_LINE] = 1,
};

typedef struct TokenArr {
    Token* buf;
    int len;
    int cap;
} TokenArr;

TokenArr new_token_arr (int cap) {
    return (TokenArr){
        .buf = malloc(cap * sizeof(Token)),
        .len = 0,
        .cap = cap
    };
}

void push_token_arr (TokenArr* arr, Token token) {
    arr->buf[arr->len++] = token;
    if (arr->len == arr->cap) {
        arr->cap *= 2;
        arr->buf = realloc(arr->buf, sizeof(Token) * arr->cap);
    }
}

void print_token_arr (TokenArr tokens) {
    for (int i = 0; i < tokens.len; ++i) {
        TokenKind kind = tokens.buf[i].kind;
        if (kind == IDENT) {
            if (i != 0 && tokens.buf[i - 1].kind == IDENT) {
                putchar(' ');
            }
            printf(tokens.buf[i].value.buf);
        } else if (kind == TEXT) {
            putchar(' ');
            COLOR(MAGENTA);
            putchar('"');
            COLOR(RESET);
            printf(tokens.buf[i].value.buf);
            COLOR(MAGENTA);
            putchar('"');
            COLOR(RESET);
        } else if (kind == NUM) {
            putchar(' ');
            COLOR(MAGENTA);
            putchar('\'');
            COLOR(RESET);
            printf(tokens.buf[i].value.buf);
            COLOR(MAGENTA);
            putchar('\'');
            COLOR(RESET);
        } else if (kind == NEW_LINE) {
            putchar('\n');
        } else {
            COLOR(MAGENTA);
            printf("%s", TOKEN_NAMES[kind]);
            COLOR(RESET);
        }
    }
    putchar('\n');
}

typedef enum LexerState {
    LS_NONE,
    LS_COMMENT,
    LS_TEXT,
    LS_NUM,
} LexerState;

#define is_space isspace
#define is_alpha isalpha
#define is_alpha_num isalnum

TokenArr lex (File file) {
    TokenArr tokens = new_token_arr(8);
    LexerState state = LS_NONE;
    GStr value = new_gstr_with_cap(8);

    for (int i = 0; i <= file.data.len; ++i) {
        char c = file.data.buf[i];
        
        if (c == '>') {
            state = LS_COMMENT;
            continue;
        }

        if (state == LS_COMMENT) {
            if (c == '\n') {
                state = LS_NONE;
            }
            continue;
        }

        TokenKind tok_kind = NONE;

        if (state == LS_TEXT) {
            if (c == '\0') {
                THROW(file, i, "expected end of text literal", 0);
            } else if (c == '"') {
                push_token_arr(&tokens, token_of_kind_with_value(
                    TEXT,
                    copy_gstr_to_str(value),
                    i - value.len
                ));
                value.len = 0;
                state = LS_NONE;
                continue;
            } else {
                if (c == '\\') {
                    c = file.data.buf[i += 1];
                    if (c == '\\') {}
                    else if (c == '\'') {
                        c = '\'';
                    } else {
                        THROW(file, i, "unrecognized character escape sequence", 0);
                    }
                }
                push_gstr(&value, c);
            }
            continue;
        }

        if (state == LS_NUM) {
            if (c == '\0') {
                THROW(file, i, "expected end of number literal", 0);
            } else if (c == '\'') {
                push_token_arr(&tokens, token_of_kind_with_value(
                    NUM,
                    copy_gstr_to_str(value),
                    i - value.len
                ));
                value.len = 0;
                state = LS_NONE;
            } else {
                push_gstr(&value, c);
            }
            continue;
        }

        // Operators and symbols
        if (c == '\0' || (c != '\n' && is_space(c))) {
            tok_kind = EMPTY;
        } else {
            if (c == '"') {
                tok_kind = EMPTY;
                state = LS_TEXT;
            } else if (c == '\'') {
                tok_kind = EMPTY;
                state = LS_NUM;
            } else if (c == '(') {
                tok_kind = PAREN_L;
            } else if (c == ')') {
                tok_kind = PAREN_R;
            } else if (c == ':') {
                tok_kind = MEMBER;
            } else if (c == '.') {
                tok_kind = INDEX;
            } else if (c == '@') {
                if (file.data.buf[i + 1] == '=') {
                    tok_kind = REF_EQUAL;
                    i += 1;
                } else if (file.data.buf[i + 1] == '~') {
                    tok_kind = REF_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = M_REF;
                }
            } else if (c == '#') {
                tok_kind = U_REF;
            } else if (c == '?') {
                tok_kind = OPT;
            } else if (c == '|') {
                tok_kind = FUN;
            } else if (c == '!') {
                tok_kind = ERR;
            } else if (c == '[') {
                tok_kind = S_CAST;
            } else if (c == '{') {
                tok_kind = U_CAST;
            } else if (c == '+') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = ADD_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = ADD;
                }
            } else if (c == '-') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = SUB_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = SUB;
                }
            } else if (c == '*') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = MUL_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = MUL;
                }
            } else if (c == '/') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = DIV_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = DIV;
                }
            } else if (c == '%') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = REM_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = REM;
                }
            } else if (c == '=') {
                tok_kind = EQUAL;
            } else if (c == '&') {
                tok_kind = AND;
            } else if (c == '^') {
                tok_kind = OR;
            } else if (c == '<') {
                if (file.data.buf[i + 1] == '=') {
                    tok_kind = LESS_EQUAL;
                    i += 1;
                } else {
                    tok_kind = LESS;
                }
            } else if (c == ',') {
                tok_kind = TYPE;
            } else if (c == '$') {
                if (file.data.buf[i + 1] == '~') {
                    tok_kind = OBJ_ASSIGN;
                    i += 1;
                } else {
                    tok_kind = OBJ;
                }
            } else if (c == '~') {
                tok_kind = ASSIGN;
            } else if (c == ';') {
                tok_kind = LABEL;
            } else if (c == '\n') {
                tok_kind = NEW_LINE;
            }
        }

        if (tok_kind == NONE) {
            if (is_alpha_num(c)) {
                push_gstr(&value, c);
            } else {
                THROW(file, i, "unexpected symbol '%c'", c);
            }
        } else {
            if (value.len > 0) {
                push_token_arr(&tokens, token_of_kind_with_value(
                    IDENT,
                    copy_gstr_to_str(value),
                    i - value.len
                ));

                value.len = 0;
            }

            if (tok_kind != EMPTY) {
                push_token_arr(&tokens, token_of_kind(
                    tok_kind,
                    i - TOKEN_LENGTHS[tok_kind] + 1
                ));
            }
        }
    }

    push_token_arr(&tokens, token_of_kind(EMPTY, file.data.len));

    free(value.buf);

    return tokens;
}
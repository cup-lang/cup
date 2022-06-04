typedef enum ExprKind {
    EX_EMPTY,
    EX_IDENT,
    EX_TEXT,
    EX_NUM,
    EX_OP,
} ExprKind;

typedef struct Expr Expr;

typedef struct ExprArr {
    Expr* buf;
    int len;
    int cap;
} ExprArr;

typedef struct Ident {
    Str value;
} Ident;

typedef struct Text {
    Str value;
} Text;

typedef struct Num {
    Str value;
} Num;

typedef struct Op {
    TokenKind kind;
    ExprArr exprs;
} Op;

typedef union ExprUnion {
    Ident ident;
    Text text;
    Num num;
    Op op;
} ExprUnion;

typedef struct Expr {
    ExprKind kind;
    ExprUnion u;
} Expr;

ExprArr new_expr_arr (int cap) {
    return (ExprArr){
        .buf = malloc(cap * sizeof(Expr)),
        .len = 0,
        .cap = cap
    };
}

void push_expr_arr (ExprArr* arr, Expr expr) {
    arr->buf[arr->len++] = expr;
    if (arr->len == arr->cap) {
        arr->cap *= 2;
        arr->buf = realloc(arr->buf, sizeof(Expr) * arr->cap);
    }
}

void do_indent (int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
}

void print_expr (Expr* expr, int indent) {
    switch (expr->kind) {
        case EX_EMPTY:
            COLOR(GREEN);
            printf("()");
            COLOR(RESET);
            break;
        case EX_IDENT:
            printf("%s", expr->u.ident.value);
            break;
        case EX_TEXT:
            COLOR(GREEN);
            putchar('"');
            COLOR(MAGENTA);
            printf("%s", expr->u.text.value);
            COLOR(GREEN);
            putchar('"');
            COLOR(RESET);
            break;
        case EX_NUM:
            COLOR(GREEN);
            putchar('\'');
            COLOR(MAGENTA);
            printf("%s", expr->u.num.value);
            COLOR(GREEN);
            putchar('\'');
            COLOR(RESET);
            break;
        case EX_OP:
            ExprArr exprs = expr->u.op.exprs;
            char is_type = expr->u.op.kind == TYPE;
            if (is_type) {
                indent += 1;
                putchar('\n');
            }
            for (int i = 0; i < exprs.len; ++i) {
                print_expr(exprs.buf + i, indent);
                if (i != exprs.len - 1) {
                    COLOR(GREEN);
                    if (is_type) {
                        putchar('\n');
                        do_indent(indent);
                    } else if (expr->u.op.kind == ARG) {
                        printf(" ");
                    } else {
                        printf("%s", TOKEN_NAMES[expr->u.op.kind]);
                    }
                    COLOR(RESET);
                }
            }
            if (is_type) {
                indent -= 1;
            }
            break;
    }
}

void expect_token (File file, TokenArr tokens, int* index, TokenKind kind, char* error) {
    Token token = tokens.buf[*index];
    if (token.kind != kind) {
        THROW(file, token.index, error, 0);
    }
    *index += 1;
}

Expr* parse_expr (File file, TokenArr tokens, int* index, int op_level) {
    Expr* expr = malloc(sizeof(Expr));

    char was_paren = FALSE;
    Token token = tokens.buf[*index];
    if (token.kind == PAREN_L) {
        *index += 1;
        while (tokens.buf[*index].kind == NEW_LINE) {
            *index += 1;
        }
        was_paren = TRUE;
        if (tokens.buf[*index].kind == PAREN_R) {
            expr->kind = EX_EMPTY;
            *index += 1;
        } else {
            expr = parse_expr(file, tokens, index, 0);
            expect_token(file, tokens, index, PAREN_R, "expected ')' after '('");
        }
    } else if (token.kind == IDENT) {
        expr->kind = EX_IDENT;
        expr->u.ident.value = token.value;
        *index += 1;
    } else if (token.kind == TEXT) {
        expr->kind = EX_TEXT;
        expr->u.text.value = token.value;
        *index += 1;
    } else if (token.kind == NUM) {
        expr->kind = EX_NUM;
        expr->u.num.value = token.value;
        *index += 1;
    } else if (token.kind == M_REF || token.kind == U_REF || token.kind == OPT) {
    } else if (token.kind == PAREN_R || token.kind == EMPTY) {
        return NULL;
    } else {
        THROW(file, token.index, "expected an expression", 0);
    }

    while (TRUE) {
        int new_op_level = 0;
        token = tokens.buf[*index];
        TokenKind op_kind;

        if (token.kind == LABEL) {
            new_op_level = 8;
            op_kind = token.kind;
        } else if (token.kind == NEW_LINE) {
            // Skip multiple new lines
            while (tokens.buf[*index + 1].kind == NEW_LINE) {
                *index += 1;
            }
            new_op_level = 8;
            op_kind = TYPE;
        } else if (
            token.kind == OBJ ||
            token.kind == ASSIGN ||
            token.kind == OBJ_ASSIGN ||
            token.kind == REF_ASSIGN ||
            token.kind == ADD_ASSIGN ||
            token.kind == SUB_ASSIGN ||
            token.kind == MUL_ASSIGN ||
            token.kind == DIV_ASSIGN ||
            token.kind == REM_ASSIGN
        ) {
            new_op_level = 7;
            op_kind = token.kind;
        } else if (
            token.kind == IDENT ||
            token.kind == TEXT ||
            token.kind == NUM ||
            token.kind == PAREN_L
        ) {
            new_op_level = 6;
            op_kind = ARG;
        } else if (token.kind == TYPE) {
            new_op_level = 6;
            op_kind = token.kind;
        } else if (
            token.kind == EQUAL ||
            token.kind == REF_EQUAL ||
            token.kind == AND ||
            token.kind == OR ||
            token.kind == LESS ||
            token.kind == LESS_EQUAL
        ) {
            new_op_level = 5;
            op_kind = token.kind;
        } else if (
            token.kind == ADD ||
            token.kind == SUB ||
            token.kind == MUL ||
            token.kind == DIV ||
            token.kind == REM
        ) {
            new_op_level = 4;
            op_kind = token.kind;
        } else if (
            token.kind == S_CAST ||
            token.kind == U_CAST
        ) {
            new_op_level = 3;
            op_kind = token.kind;
        } else if (
            token.kind == M_REF ||
            token.kind == U_REF ||
            token.kind == OPT ||
            token.kind == FUN ||
            token.kind == ERR
        ) {
            new_op_level = 2;
            op_kind = token.kind;
        } else if (token.kind == MEMBER || token.kind == INDEX) {
            new_op_level = 1;
            op_kind = token.kind;
        }

        if (token.kind == M_REF ||token.kind == U_REF || token.kind == OPT) {
            *index += 1;
            expr->kind = EX_OP;
            expr->u.op.kind = op_kind;
            expr->u.op.exprs = new_expr_arr(1);
            push_expr_arr(&expr->u.op.exprs, *parse_expr(file, tokens, index, new_op_level));
            continue;
        }

        if (new_op_level == 0 || (op_level != 0 && new_op_level >= op_level)) {
            break;
        }

        if (op_kind != ARG) {
            *index += 1;
        }

        Expr* other = parse_expr(file, tokens, index, new_op_level);
        if (other == NULL) {
            break;
        }

        // Chaining
        if (!was_paren && (new_op_level == 5 || (new_op_level == 7 && op_kind != OBJ) || op_kind == MEMBER || op_kind == TYPE) && expr->kind == EX_OP && expr->u.op.kind == op_kind) {
            push_expr_arr(&expr->u.op.exprs, *other);
        }
        // Binary
        else {
            Expr old_expr = *expr;
            expr = malloc(sizeof(Expr));
            expr->kind = EX_OP;
            expr->u.op.kind = op_kind;
            expr->u.op.exprs = new_expr_arr(2);
            push_expr_arr(&expr->u.op.exprs, old_expr);
            push_expr_arr(&expr->u.op.exprs, *other);
        }
    }

    return expr;
}

Expr* parse (File file, TokenArr tokens) {
    if (tokens.len == 1 && tokens.buf[0].kind == EMPTY) {
        Expr* expr = malloc(sizeof(Expr));
        expr->kind = EX_EMPTY;
        return expr;
    }
    int* index = malloc(sizeof(int));
    *index = 0;
    return parse_expr(file, tokens, index, 0);
}
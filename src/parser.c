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
		printf("    ");
	}
}

void print_expr (Expr* expr, int indent) {
	do_indent(indent);
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
			char is_block = expr->u.op.kind == BLOCK;
			putchar('(');
			if (is_block) {
				indent += 1;
				putchar('\n');
			}
			for (int i = 0; i < exprs.len; ++i) {
				print_expr(exprs.buf + i, is_block ? indent : 0);
				if (i != exprs.len - 1) {
					COLOR(GREEN);
					if (is_block) {
						putchar('\n');
					} else {
						printf(" %s ", TOKEN_NAMES[expr->u.op.kind]);
					}
					COLOR(RESET);
				}
			}
			if (is_block) {
				indent -= 1;
				putchar('\n');
				do_indent(indent);
			}
			putchar(')');
			break;
		default:
			COLOR(RED);
			printf("ERROR");
			COLOR(RESET);
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

int count_indents (TokenArr tokens, int* index) {
	while (tokens.buf[*index].kind == NEW_LINE) {
		*index += 1;
	}
	int indent = 0;
	while (tokens.buf[*index].kind == INDENT) {
		indent += 1;
		*index += 1;
	}
	return indent;
}

Expr* parse_expr (File file, TokenArr tokens, int* index, int last_indent, int op_level) {
	Expr* expr = malloc(sizeof(Expr));
	char was_paren = FALSE;

	Token token = tokens.buf[*index];
	if (token.kind == NEW_LINE) {
		// Count indents
		int indent = count_indents(tokens, index);

		// End block
		// if (indent < last_indent) {
		// 	*index -= indent;
		// 	return NULL;
		// }

		// New block
		if (indent > last_indent) {
			last_indent = indent;
			return parse_expr(file, tokens, index, last_indent, 0);
		}
	}
	token = tokens.buf[*index];

	if (token.kind == PAREN_L) {
		*index += 1;
		was_paren = TRUE;
		if (tokens.buf[*index].kind == PAREN_R) {
			expr->kind = EX_EMPTY;
			*index += 1;
		} else {
			int indent = count_indents(tokens, index);
			expr = parse_expr(file, tokens, index, indent, 0);
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
	} else if (token.kind == PAREN_R) {
		expr->kind = EX_EMPTY;
		return expr;
	} else if (token.kind == EMPTY) {
		return NULL;
	} else {
		THROW(file, token.index, "expected an expression, got '%s'", TOKEN_NAMES[token.kind]);
	}

	char was_block = FALSE;
	while (TRUE) {
		int new_op_level = 0;
		token = tokens.buf[*index];
		TokenKind op_kind = EMPTY;
		Expr* other;

		if (
			token.kind == LABEL ||
			token.kind == ARG
		) {
			new_op_level = 7;
			op_kind = token.kind;
		} else if (
			token.kind == PAREN_L ||
			token.kind == IDENT ||
			token.kind == TEXT ||
			token.kind == NUM
		) {
			new_op_level = 6;
			op_kind = ARG;
		} else if (
			token.kind == ASSIGN ||
			token.kind == OBJ_ASSIGN ||
			token.kind == REF_ASSIGN ||
			token.kind == ADD_ASSIGN ||
			token.kind == SUB_ASSIGN ||
			token.kind == MUL_ASSIGN ||
			token.kind == DIV_ASSIGN ||
			token.kind == REM_ASSIGN
		) {
			new_op_level = 6;
			op_kind = token.kind;
		} else if (token.kind == BLOCK) {
			new_op_level = 6;
			op_kind = BLOCK;
			was_block = TRUE;
		} else if (token.kind == NEW_LINE) {
			int indent = count_indents(tokens, index);

			if (indent > last_indent) {
				other = parse_expr(file, tokens, index, indent, 0);
				goto parsed_other;
			} else if (indent == last_indent) {
				new_op_level = 7;
				op_kind = BLOCK;
				was_block = TRUE;
				*index -= 1;
			}
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
			token.kind == U_CAST ||
			token.kind == OBJ
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

		// Unary operators
		if (token.kind == M_REF ||token.kind == U_REF || token.kind == OPT) {
			*index += 1;
			expr->kind = EX_OP;
			expr->u.op.kind = op_kind;
			expr->u.op.exprs = new_expr_arr(1);
			push_expr_arr(&expr->u.op.exprs, *parse_expr(file, tokens, index, last_indent, new_op_level));
			continue;
		}

		// No operator was found
		if (new_op_level == 0) {
			break;
		}

		op_rhs:
		printf("~ was %i got %i (at %i)\n", op_level, new_op_level, *index);
		char is_ltr = op_level != 6 || was_block;
		// Operator has lower precedence
		if (
			op_level != 0 &&
			// Left to right
			(is_ltr && op_level <= new_op_level) ||
			// Right to left
			(!is_ltr && op_level < new_op_level)
		) {
			printf("< lower was %i got %i (at %i)\n", op_level, new_op_level, *index);
			if (new_op_level == 7 && op_kind == BLOCK) {
				*index -= last_indent;
			}
			break;
		}

		// Every operator except "argumentation with space"
		if (!(op_kind == ARG && new_op_level == 6)) {
			*index += 1;
		}

		other = parse_expr(file, tokens, index, last_indent, new_op_level);
		parsed_other:
		if (other == NULL) {
			printf("! was NULL (at %i)\n", *index);
			break;
		}

		// Chaining operator
		if (
			!was_paren &&
			expr->kind == EX_OP &&
			expr->u.op.kind == op_kind && (
				// Logic operators
				new_op_level == 5 ||
				// Assign operators
				(new_op_level == 6 && op_kind != ARG) ||
				op_kind == MEMBER ||
				op_kind == BLOCK
			)
		) {
			push_expr_arr(&expr->u.op.exprs, *other);
		}
		// Binary operator
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

	int indent = count_indents(tokens, index);
	return parse_expr(file, tokens, index, indent, 0);
}
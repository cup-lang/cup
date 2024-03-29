typedef enum ExprKind {
	EX_EMPTY,
	EX_IDENT,
	EX_TEXT,
	EX_NUM,
	EX_OP,
} ExprKind;

typedef struct Expr Expr;

ARRAY_STRUCT(Expr)

typedef struct Ident {
	Str value;
} ExprIdent;

typedef struct Text {
	Str value;
} ExprText;

typedef struct Num {
	Str value;
} ExprNum;

typedef struct Op {
	TokenKind kind;
	ExprArr exprs;
} ExprOp;

typedef union ExprUnion {
	ExprIdent ident;
	ExprText text;
	ExprNum num;
	ExprOp op;
} ExprUnion;

typedef struct Expr {
	ExprKind kind;
	ExprUnion u;
	File file;
	int index;
} Expr;

ARRAY_FUNCS(Expr, expr)

void do_indent (int indent) {
	for (int i = 0; i < indent; ++i) {
		printf("    ");
	}
}

void print_expr (Expr* expr, int indent, char should_indent) {
	if (should_indent) {
		do_indent(indent);
	}
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
			COLOR(MAGENTA);
			putchar('"');
			COLOR(RESET);
			printf("%s", expr->u.text.value);
			COLOR(MAGENTA);
			putchar('"');
			COLOR(RESET);
			break;
		case EX_NUM:
			COLOR(MAGENTA);
			putchar('\'');
			COLOR(RESET);
			printf("%s", expr->u.num.value);
			COLOR(MAGENTA);
			putchar('\'');
			COLOR(RESET);
			break;
		case EX_OP:
			ExprArr exprs = expr->u.op.exprs;
			char is_newline = expr->u.op.kind == NEW_LINE;
			if (exprs.len != 1) {
				COLOR(GREEN);
				putchar('(');
				COLOR(RESET);
			}
			if (is_newline) {
				indent += 1;
				putchar('\n');
			}
			for (int i = 0; i < exprs.len; ++i) {
				// Unary operator
				if (exprs.len == 1) {
					COLOR(MAGENTA);
					printf("%s", TOKEN_NAMES[expr->u.op.kind]);
					COLOR(RESET);
				}
				print_expr(exprs.buf + i, indent, is_newline);
				if (i != exprs.len - 1) {
					COLOR(MAGENTA);
					if (is_newline) {
						putchar('\n');
					} else {
						printf(" %s ", TOKEN_NAMES[expr->u.op.kind]);
					}
					COLOR(RESET);
				}
			}
			if (is_newline) {
				indent -= 1;
				putchar('\n');
				do_indent(indent);
			}
			if (exprs.len != 1) {
				COLOR(GREEN);
				putchar(')');
				COLOR(RESET);
			}
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

char has_lower_precedence (int op_level, int new_op_level, char is_ltr) {
	return op_level != 0 &&
		// When left to right
		(is_ltr && op_level <= new_op_level) ||
		// When right to left
		(!is_ltr && op_level < new_op_level);
}

Expr* parse_expr (File file, TokenArr tokens, int* index, int last_indent, int op_level) {
	// Prevents chaining
	char was_paren = FALSE;

	Token token = tokens.buf[*index];
	while (token.kind == NEW_LINE || token.kind == INDENT) {
		int indent = count_indents(tokens, index);

		// New block
		if (indent > last_indent) {
			return parse_expr(file, tokens, index, indent, 0);
		}
		// End block
		else if (indent < last_indent) {
			*index -= indent;
			return NULL;
		}

		last_indent = indent;
		token = tokens.buf[*index];
	}

	Expr* expr = malloc(sizeof(Expr));
	expr->file = file;
	expr->index = token.index;

	if (token.kind == PAREN_L) {
		*index += 1;
		was_paren = TRUE;
		Expr* new_expr = parse_expr(file, tokens, index, 0, 0);
		if (new_expr == NULL) {
			expr->kind = EX_EMPTY;
		} else {
			expr = new_expr;
		}
		count_indents(tokens, index);
		expect_token(file, tokens, index, PAREN_R, "expected ')' after '('");
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
	}
	// Unary, do nothing yet
	else if (token.kind == NEG || token.kind == REF || token.kind == OPT) {}
	else {
		return NULL;
	}

	while (TRUE) {
		int new_op_level = 0;
		token = tokens.buf[*index];
		TokenKind op_kind = EMPTY;
		Expr* other;

		
		if (token.kind == NEW_LINE) {
			new_op_level = 9;
			op_kind = NEW_LINE;
			
			int old_index = *index;
			int indent = count_indents(tokens, index);

			// New block
			if (indent > last_indent) {
				if (has_lower_precedence(op_level, new_op_level, TRUE)) {
					*index = old_index;
					break;
				}
				other = parse_expr(file, tokens, index, indent, 0);
				goto parsed_other;
			}
			// Same block
			else if (indent == last_indent) {
				*index -= 1;
			}
			// End block
			else {
				*index = old_index;
				break;
			}
		} else if (
			token.kind == LABEL ||
			token.kind == ARG
		) {
			new_op_level = 8;
			op_kind = token.kind;
		} else if (
			token.kind == ASSIGN ||
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
			token.kind == EQUAL ||
			token.kind == REF_EQUAL ||
			token.kind == AND ||
			token.kind == OR ||
			token.kind == XOR
		) {
			new_op_level = 6;
			op_kind = token.kind;
		} else if (
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
			token.kind == IDENT ||
			token.kind == TEXT ||
			token.kind == NUM ||
			token.kind == PAREN_L
		) {
			new_op_level = 3;
			op_kind = ARG;
		} else if (
			token.kind == OBJ ||
			token.kind == BLOCK
		) {
			new_op_level = 3;
			op_kind = token.kind;
		} else if (
			token.kind == NEG ||
			token.kind == REF ||
			token.kind == OPT ||
			token.kind == ERR
		) {
			new_op_level = 2;
			op_kind = token.kind;
		} else if (token.kind == MEMBER) {
			new_op_level = 1;
			op_kind = token.kind;
		} else if (token.kind == PAREN_R || token.kind == EMPTY) {
			break;
		}

		// Unary operators
		if (token.kind == NEG || token.kind == REF || token.kind == OPT) {
			*index += 1;
			Expr* new_expr = parse_expr(file, tokens, index, last_indent, new_op_level);
			if (new_expr == NULL) {
				Token token = tokens.buf[*index];
				THROW(file, token.index, "expected an expression, got '%s'", TOKEN_NAMES[token.kind]);
			}
			expr->kind = EX_OP;
			expr->u.op.kind = op_kind;
			expr->u.op.exprs = new_expr_arr(1);
			push_expr_arr(&expr->u.op.exprs, *new_expr);
			continue;
		}

		// No operator was found
		if (new_op_level == 0) {
			THROW(file, token.index, "expected a symbol got '%s'", TOKEN_NAMES[token.kind]);
		}

		op_rhs:
		// Operator has lower precedence
		if (has_lower_precedence(op_level, new_op_level, op_level != 3 && op_level != 7)) {
			if (op_kind == NEW_LINE) {
				*index -= last_indent;
			}
			break;
		}

		int op_index = token.index;
		// Consume the operator
		if (op_kind != ARG || new_op_level != 3) {
			*index += 1;
		}

		other = parse_expr(file, tokens, index, last_indent, new_op_level);
		parsed_other:
		if (other == NULL) {
			break;
		}

		// Chaining operator
		if (
			!was_paren &&
			expr->kind == EX_OP &&
			expr->u.op.kind == op_kind &&
			(
				op_kind == MEMBER ||
				op_kind == BLOCK ||
				// Logic operators
				new_op_level == 5 || new_op_level == 6 ||
				// Assign operators
				new_op_level == 7 ||
				op_kind == NEW_LINE
			)
		) {
			push_expr_arr(&expr->u.op.exprs, *other);
		}
		// Chaining right-to-left block
		else if (
			!was_paren &&
			other->kind == EX_OP &&
			other->u.op.kind == op_kind &&
			op_kind == BLOCK
		) {
			push_front_expr_arr(&other->u.op.exprs, *expr);
			expr = other;
		}
		// Binary operator
		else {
			Expr old_expr = *expr;
			expr = malloc(sizeof(Expr));
			expr->kind = EX_OP;
			expr->file = file;
			expr->index = op_index;
			expr->u.op.kind = op_kind;
			expr->u.op.exprs = new_expr_arr(2);
			push_expr_arr(&expr->u.op.exprs, old_expr);
			push_expr_arr(&expr->u.op.exprs, *other);
		}
	}

	return expr;
}

Expr* parse (File file, TokenArr tokens) {
	int* index = malloc(sizeof(int));
	*index = 0;

	Expr* expr = parse_expr(file, tokens, index, 0, 0);
	if (*index < tokens.len - 1) {
		Token token = tokens.buf[*index];
		THROW(file, token.index, "expected an expression, got '%s'", TOKEN_NAMES[token.kind]);
	}
	if (expr == NULL) {
		expr = malloc(sizeof(Expr));
		expr->kind = EX_EMPTY;
		expr->file = file;
		expr->index = 0;
	}
	return expr;
}
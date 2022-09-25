typedef enum TokenKind {
	NONE, // fake optional
	EMPTY, INDENT, IDENT, TEXT, NUM,
	PAREN_L, PAREN_R,
	MEMBER,
	NEG, REF, OPT, ERR,
	OBJ, BLOCK,
	ADD, SUB, MUL, DIV, REM,
	LESS, LESS_EQUAL,
	EQUAL, REF_EQUAL, AND, OR, XOR,
	ASSIGN, REF_ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, REM_ASSIGN,
	LABEL, ARG, NEW_LINE,
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

const char* const TOKEN_NAMES[] = {
	[EMPTY] = "EMPTY", [INDENT] = "--->", [IDENT] = "IDENT", [TEXT] = "TEXT", [NUM] = "NUM",
	[PAREN_L] = "(", [PAREN_R] = ")",
	[MEMBER] = ".",
	[NEG] = "!", [REF] = "@", [OPT] = "?", [ERR] = "$",
	[OBJ] = ":", [BLOCK] = ",",
	[ADD] = "+", [SUB] = "-", [MUL] = "*", [DIV] = "/", [REM] = "%",
	[LESS] = "<", [LESS_EQUAL] = "<=",
	[EQUAL] = "=", [REF_EQUAL] = "@=", [AND] = "&", [OR] = "|", [XOR] = "^",
	[ASSIGN] = "~", [REF_ASSIGN] = "@~", [ADD_ASSIGN] = "+~", [SUB_ASSIGN] = "-~", [MUL_ASSIGN] = "*~", [DIV_ASSIGN] = "/~", [REM_ASSIGN] = "%~",
	[LABEL] = ";", [ARG] = "#", [NEW_LINE] = "NEW LINE",
};

const int TOKEN_LENGTHS[] = {
	[EMPTY] = 5, [INDENT] = 1, [IDENT] = 5, [TEXT] = 4, [NUM] = 3,
	[PAREN_L] = 1, [PAREN_R] = 1,
	[MEMBER] = 1,
	[NEG] = 1, [REF] = 1, [OPT] = 1, [ERR] = 1,
	[OBJ] = 1, [BLOCK] = 1,
	[ADD] = 1, [SUB] = 1, [MUL] = 1, [DIV] = 1, [REM] = 1,
	[LESS] = 1, [LESS_EQUAL] = 2,
	[EQUAL] = 1, [REF_EQUAL] = 2, [AND] = 1, [OR] = 1, [XOR] = 1,
	[ASSIGN] = 1, [REF_ASSIGN] = 2, [ADD_ASSIGN] = 2, [SUB_ASSIGN] = 2, [MUL_ASSIGN] = 2, [DIV_ASSIGN] = 2, [REM_ASSIGN] = 2,
	[LABEL] = 1, [ARG] = 1, [NEW_LINE] = 1,
};

ARRAY(Token, token)

void print_token_arr (TokenArr tokens) {
	for (int i = 0; i < tokens.len; ++i) {
		TokenKind kind = tokens.buf[i].kind;
		if (kind == IDENT) {
			if (i != 0 && tokens.buf[i - 1].kind == IDENT) {
				putchar(' ');
			}
			printf("%s", tokens.buf[i].value.buf);
		} else if (kind == TEXT) {
			putchar(' ');
			COLOR(GREEN);
			putchar('"');
			COLOR(RESET);
			printf("%s", tokens.buf[i].value.buf);
			COLOR(GREEN);
			putchar('"');
			COLOR(RESET);
		} else if (kind == NUM) {
			putchar(' ');
			COLOR(GREEN);
			putchar('\'');
			COLOR(RESET);
			printf("%s", tokens.buf[i].value.buf);
			COLOR(GREEN);
			putchar('\'');
			COLOR(RESET);
		} else if (kind == NEW_LINE) {
			putchar('\n');
		} else {
			COLOR(GREEN);
			printf("%s", TOKEN_NAMES[kind]);
			COLOR(RESET);
		}
	}
	putchar('\n');
}

void try_make_ident (TokenArr* tokens, GStr* value, int index) {
	if (value->len > 0) {
		push_token_arr(tokens, token_of_kind_with_value(
			IDENT,
			copy_gstr_to_str(*value),
			index - value->len
		));

		value->len = 0;
	}
}

typedef enum LexerState {
	LS_NONE,
	LS_COMMENT,
	LS_TEXT,
	LS_NUM,
} LexerState;

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
				try_make_ident(&tokens, &value, i);
				push_token_arr(&tokens, token_of_kind(NEW_LINE, i));
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
		if (c == '\0' || c == ' ') {
			tok_kind = EMPTY;
		} else {
			#define IF_CHAR(CHAR, TOKEN) if (c == CHAR) { tok_kind = TOKEN; }
			#define IF_CHAR_THEN(CHAR, TOKEN, THEN) if (c == CHAR) { tok_kind = TOKEN; THEN; }
			#define IF_NEXT_CHAR(CHAR, TOKEN) if (file.data.buf[i + 1] == CHAR) { tok_kind = TOKEN; i += 1; }

			IF_CHAR('\t', INDENT)
			else IF_CHAR_THEN('"', EMPTY, state = LS_TEXT)
			else IF_CHAR_THEN('\'', EMPTY, state = LS_NUM)
			else IF_CHAR('(', PAREN_L)
			else IF_CHAR(')', PAREN_R)
			else IF_CHAR('.', MEMBER)
			else IF_CHAR('!', NEG)
			else IF_CHAR_THEN('@', REF,
				IF_NEXT_CHAR('=', REF_EQUAL)
				else IF_NEXT_CHAR('~', REF_ASSIGN)
			)
			else IF_CHAR('?', OPT)
			else IF_CHAR('$', ERR)
			else IF_CHAR(':', OBJ)
			else IF_CHAR(',', BLOCK)
			else IF_CHAR_THEN('+', ADD, IF_NEXT_CHAR('~', ADD_ASSIGN))
			else IF_CHAR_THEN('-', SUB, IF_NEXT_CHAR('~', SUB_ASSIGN))
			else IF_CHAR_THEN('*', MUL, IF_NEXT_CHAR('~', MUL_ASSIGN))
			else IF_CHAR_THEN('/', DIV, IF_NEXT_CHAR('~', DIV_ASSIGN))
			else IF_CHAR_THEN('%', REM, IF_NEXT_CHAR('~', REM_ASSIGN))
			else IF_CHAR_THEN('<', LESS, IF_NEXT_CHAR('=', LESS_EQUAL))
			else IF_CHAR('=', EQUAL)
			else IF_CHAR('&', AND)
			else IF_CHAR('|', OR)
			else IF_CHAR('^', XOR)
			else IF_CHAR('~', ASSIGN)
			else IF_CHAR(';', LABEL)
			else IF_CHAR('#', ARG)
			else IF_CHAR('\n', NEW_LINE)
		}

		if (tok_kind == NONE) {
			if (is_alpha_num(c) || c == '_') {
				push_gstr(&value, c);
			} else {
				THROW(file, i, "unexpected symbol '%c'", c);
			}
		} else {
			try_make_ident(&tokens, &value, i);

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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>

#define TRUE 1
#define FALSE 0

typedef struct Str {
	char* buf;
	int len;
} Str;

Str new_str_from_ptr (char* buf) {
	return (Str){ .buf = buf, .len = strlen(buf) };
}

Str new_str_with_len (int len) {
	Str str = { .buf = malloc((len + 1) * sizeof(char)), len = len };
	str.buf[str.len] = '\0';
	return str;
}

typedef struct GStr {
	char* buf;
	int len;
	int cap;
} GStr;

GStr new_gstr_from_ptr (char* buf) {
	int len = strlen(buf);
	return (GStr){ .buf = buf, .len = len, .cap = len };
}

GStr new_gstr_with_cap (int cap) {
	GStr str = {
		.buf = malloc(cap * sizeof(char)),
		.len = 0,
		.cap = cap
	};
	str.buf[str.len] = '\0';
	return str;
}

void push_gstr (GStr* str, char c) {
	str->buf[str->len++] = c;
	if (str->len == str->cap) {
		str->cap *= 2;
		str->buf = realloc(str->buf, sizeof(char) * str->cap);
	}
	str->buf[str->len] = '\0';
}

Str copy_gstr_to_str (GStr str) {
	Str new_str = new_str_with_len(str.len);
	memcpy(new_str.buf, str.buf, str.len);
	return new_str;
}

#define COLOR(c) printf(c)
#define RESET "\033[0m"
#define MAGENTA "\033[35m"
#define GREEN "\033[32m"
#define RED "\033[31m"

typedef struct File {
	Str path;
	Str data;
} File;

#include "throw.c"
#include "lexer.c"
#include "parser.c"

Str read_file (char* path) {
	FILE *input;
	input = fopen(path, "rb");
	fseek(input, 0L, SEEK_END);
	Str file = new_str_with_len(ftell(input));
	rewind(input);
	fread(file.buf, file.len, 1, input);
	fclose(input);
	return file;
}

Expr* lex_parse (Str path) {
	printf("Compiling: %s\n", path);
	File file = { .path = path, .data = read_file(path.buf) };
	TokenArr tokens = lex(file);
	// print_token_arr(tokens);
	Expr* expr = parse(file, tokens);
	print_expr(expr, 0, FALSE);
	return expr;
}

Str join_paths (Str a, Str b) {
	Str str = new_str_with_len(a.len + 1 + b.len);
	memcpy(str.buf, a.buf, a.len);
	str.buf[a.len] = '/';
	memcpy(str.buf + a.len + 1, b.buf, b.len);
	return str;
}

Expr* lex_parse_rec (Str path) {
	DIR *dir = opendir(path.buf);
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		Str file = new_str_from_ptr(ent->d_name);

		if (strcmp(file.buf, ".") == 0 || strcmp(file.buf, "..") == 0) {
			continue;
		}

		Str new_path = join_paths(path, file);
		
		switch (ent->d_type) {
			case DT_DIR:
				lex_parse_rec(new_path);
				break;
			case DT_REG: {
				lex_parse(new_path);
				break;
			}
		}

		free(new_path.buf);
	}
	closedir(dir);
}

int main () {
	lex_parse(new_str_from_ptr("src/cup/main.cup"));
	// lex_parse_rec(new_str_from_ptr("src/cup"));
	// analyze(expr);
	return 0;
}
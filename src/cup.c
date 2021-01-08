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

#include "lexer.c"
#include "parser.c"

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
#include <stdlib.h>
#include <stdio.h>

#define VECTOR(name, lower, type) typedef struct{type* array;int size;int capacity;}name;name lower##_new(int c){name v;v.array=calloc(c, sizeof(type));v.size=0;v.capacity = c;return v;}void lower##_push(name* v, type e){v->array[v->size++]=e;if(v->size==v->capacity){int c=v->capacity*=2;v->array=realloc(v->array, sizeof(type) * c);}}
VECTOR(String, string, char);

typedef enum
{
    A
} TokenKind;

typedef struct
{
    TokenKind kind;
} Token;

VECTOR(TokenVector, token_vector, Token);

TokenVector lex(char* input)
{

}

void parse()
{
}

void gen(FILE *output)
{
}

int main()
{
    FILE *input;
    fopen_s(&input, "src/main.cp", "rb");
    fseek(input, 0L, SEEK_END);
    long input_size = ftell(input);
    rewind(input);
    char* data = malloc(input_size);
    fread(data, input_size, 1, input);
    fclose(input);
    lex(data);
    
    parse();

    FILE* output;
    fopen_s(&output, "bin/main.c", "w");
    gen(output);
}

//////////////////////////////

// multiple file support
// non-linear parsing
// single output file
// generics support
// generic functions
// generic structs
// generic enums
// generic variables
// match on enums
// if elif else
// do while do-while
// break return next
// all ops
// mod support

//////////////////////////////

// lex
// parse
// gen
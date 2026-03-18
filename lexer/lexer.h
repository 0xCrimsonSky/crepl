#ifndef LEXER
#define LEXER

#include "../global.h"

typedef enum {
    NUM,
    FNUM,
    IDENTIFIER,
    OPERATOR,
    LITERAL
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

DynArray* tokenize(char*);

void tokenizeFree(DynArray*);

#endif
#ifndef PARSER
#define PARSER

#include "../global.h"

typedef enum {
    INTEGER,
    FLOAT,
    BINARYOP,
    UNARYOP,
    ASSIGN,
    RPARAMOP,
    RPARAMCL,
    UNKNOWN,
    START,
    END
} NodeType;

typedef enum {
    ADD,
    SUB,
    MUL,
    DIV
} BinOps;

struct Statement {
    NodeType type;
    void* value;
    struct Statement* next;
};

typedef struct {
    NumType type;
    union {
        int i;
        float f;
        double d;
    } value;
} NumberStatement;

typedef struct {
    NumType type;
    union {
        int i;
        float f;
        double d;
    } value;
    BinOps op;
} ExpressionStatement;

typedef struct {
    ExpressionStatement* multiplyQueue;
    ExpressionStatement* addQueue;
} EvaluationQueue;

typedef struct {
    NumType type;
    union {
        int i;
        float f;
        double d;
    } value;
    union {
        int i;
        float f;
        double d;
    } prev_operand_value;
} ExpressionResult;

typedef struct Statement Statement;

Statement* generateAST(char *);

void parse(Statement*);

void ASTFree(Statement*);

#endif
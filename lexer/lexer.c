#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "lexer.h"

bool isNum(char c) {
    return c >= '0' && c <= '9';
}

bool isDot(char c) {
    return c == '.';
}

char* generateNumber(char* input, int* i) {
    size_t size = 0, capacity = 8;
    char* value = malloc(capacity);

    while(input[*i] != '\0' && (isNum(input[*i]) || isDot(input[*i]))){
        size++;
        if(size+1 == capacity) {
            capacity *= 2;
            value = realloc(value, capacity);
        }
        value[size-1] = input[*i];
        *i = *i+1;
    }
    value[size] = '\0';
    return value;
}

DynArray* tokenize(char* input) {
    int capacity = 1;
    Token* tokens = malloc(sizeof(Token) * capacity);
    int tokens_len = 0;

    for(int i=0; input[i] != '\0'; i++) {
        if(tokens_len == capacity) {
            capacity *= 2;
            Token* temp = realloc(tokens, capacity * (sizeof *temp));
            if(temp == NULL){
                printf("Failed to allocate memory");
                exit(1);
            }
            tokens = temp;
        }

        char ch = input[i];
        switch(ch){
            case ' ':
            case '\n':
                continue;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                char* value = generateNumber(input, &i);

                tokens[tokens_len].type = NUM;
                tokens[tokens_len].value = malloc(strlen(value)+1);
                strcpy(tokens[tokens_len].value, value);
                free(value);

                i--;
                tokens_len++;

                break;
            case '+':
            case '-':
            case '*':
            case '/':
                tokens[tokens_len].type = OPERATOR;
                tokens[tokens_len].value = malloc(sizeof(char)*2);
                tokens[tokens_len].value[0] = ch;
                tokens[tokens_len].value[1] = '\0';

                tokens_len++;
                break;
            case '.':
                if(input[i+1] != '\0' && isNum(input[i+1])) {
                    char* value = generateNumber(input, &i);
                    size_t value_length = strlen(value);

                    tokens[tokens_len].type = NUM;
                    tokens[tokens_len].value = malloc(value_length+1);
                    strcpy(tokens[tokens_len].value, value);
                    free(value);

                    i--;
                    tokens_len++;

                    break;
                }
                // fall through
            default:
                tokens[tokens_len].type = LITERAL;
                tokens[tokens_len].value = malloc(sizeof(char)*2);
                tokens[tokens_len].value[0] = ch;
                tokens[tokens_len].value[1] = '\0';

                tokens_len++;
        }
    }

    DynArray* arr = malloc(sizeof(DynArray));
    arr->data = tokens;
    arr->size = tokens_len;

    return arr;
}

void tokenizeFree(DynArray* arr) {
    Token* items = (Token*)arr->data;

    for(size_t i=0; i < arr->size; i++) {
        free(items[i].value);
    }
    
    free(items);
    free(arr);
}
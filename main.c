#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include "parser/parser.h"

#define VERSION "0.1.0"

int isExit(char* input) {
    if(
        input[0] != '\0' && (input[0] == 'e' || input[0] == 'E') &&
        input[1] != '\0' && (input[1] == 'x' || input[1] == 'X') &&
        input[2] != '\0' && (input[2] == 'i' || input[2] == 'I') &&
        input[3] != '\0' && (input[3] == 't' || input[3] == 'T') &&
        (input[4] == '\0' || input[4] == '\n')
    ) {
        return 1;
    }
    return 0;
}

int main(){
    char input[256];
    Statement* program = NULL;

    printf("CREPL v%s\n> ", VERSION);
    fgets(input, sizeof(input), stdin);

    while(isExit(input) == 0) {
        program = generateAST(input);
        if(program != NULL) {
            parse(program);
        }
        ASTFree(program);
        program = NULL;
        
        printf("> ");
        fgets(input, sizeof(input), stdin);
    }
    printf("Exiting...\n");
}
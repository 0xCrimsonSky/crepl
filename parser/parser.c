#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "parser.h"
#include "../lexer/lexer.c"

void addPendingResult(ExpressionResult**, ExpressionStatement*);

void printResult(ExpressionResult* result){
    if(result->type == NUM_FLOAT) {
        printf("%f\n", result->value.f);
    }
    else {
        printf("%d\n", result->value.i);
    }
}

char getOperatorChar(BinOps op){
    if(op == 0) return '+';
    if(op == 1) return '-';
    if(op == 2) return '*';
    if(op == 3) return '/';
    return '+';
}

Statement* generateStatement(Token* items, int index) {
    TokenType type = items[index].type;
    Statement* stmt = malloc(sizeof(Statement));

    switch(type){
        case NUM: {
            float integer = 0, fraction = 0.1;
            bool is_fraction = false;
            char* str_num = items[index].value;
            for(int i=0; str_num[i] != '\0'; i++) {
                if(str_num[i] == '.') {
                    if(is_fraction == true) {
                        printf("Incorrect number %s\n", str_num);
                        free(stmt);
                        return NULL;
                    }
                    is_fraction = true;
                    continue;
                }

                if(is_fraction == true){
                    float temp = fraction;
                    fraction *= ((float)(str_num[i]-48));
                    integer += fraction;
                    fraction = temp * 0.1;
                }
                else {
                    integer *= 10;
                    integer += ((float)(str_num[i]-48));
                }
            }
            
            if(is_fraction) {
                stmt->type = FLOAT;
                stmt->value = malloc(sizeof(float));
                *(float*)stmt->value = integer;
            } else {
                stmt->type = INTEGER;
                stmt->value = malloc(sizeof(int));
                *(int*)stmt->value = (int)integer;
            }
            break;
        }
        case OPERATOR:
            stmt->value = malloc(sizeof(BinOps));
            stmt->type = BINARYOP;

            char literal = items[index].value[0];
            if(literal == '+') {
                *(BinOps*)stmt->value = ADD;
            } else if(literal == '-') {
                *(BinOps*)stmt->value = SUB;
            } else if(literal == '*') {
                *(BinOps*)stmt->value = MUL;
            } else if(literal == '/') {
                *(BinOps*)stmt->value = DIV;
            } else {
                free(stmt->value);
                stmt->value = malloc(sizeof(char)+1);
                stmt->type = UNKNOWN;
                ((char*)stmt->value)[0] = items[index].value[0];
                ((char*)stmt->value)[1] = '\0';
            }
            break;
        case LITERAL: {
            stmt->value = malloc(sizeof(char)+1);

            char literal = items[index].value[0];
            if(literal == '('){
                stmt->type = RPARAMOP;
            } else if(literal == ')') {
                stmt->type = RPARAMCL;
            } else {
                stmt->type = UNKNOWN;
            }
            ((char*)stmt->value)[0] = literal;
            ((char*)stmt->value)[1] = '\0';
            break;
        }
        default: {
            stmt->type = UNKNOWN;
            stmt->value = malloc(sizeof(char)+1);
            ((char*)stmt->value)[0] = items[index].value[0];
            ((char*)stmt->value)[1] = '\0';
        }
    }

    return stmt;
}

Statement* generateAST(char* input){
    DynArray* tokenArr = tokenize(input);
    Token* items = (Token*)tokenArr->data;
    Statement* curr_stmt = malloc(sizeof(Statement));
    curr_stmt->type = START;
    curr_stmt->value = NULL;

    Statement* program = curr_stmt;

    for(size_t i=0; i<tokenArr->size; i++){
        curr_stmt->next = generateStatement(items, i);
        if(curr_stmt->next == NULL) {
            ASTFree(program);
            tokenizeFree(tokenArr);
            return NULL;
        }
        curr_stmt = curr_stmt->next;
    }
    Statement* end = malloc(sizeof(Statement));
    end->type = END;
    end->value = NULL;
    end->next = NULL;

    curr_stmt->next = end;
    
    tokenizeFree(tokenArr);

    return program;
}

void ASTFree(Statement* stmt) {
    Statement* prev;
    while(stmt != NULL) {
        prev = stmt;
        stmt = stmt->next;
        free(prev->value);
        free(prev);
    }
}

void execMathOp(ExpressionResult* result, Statement* right_operand, BinOps operator) {
    if(right_operand->type == FLOAT) {
        if(result->type == NUM_FLOAT) {
            result->prev_operand_value.f = *(float*)right_operand->value;
        } else {
            result->type = NUM_FLOAT;
            result->value.f = (float)result->value.i;
            result->prev_operand_value.f = (int)(*(float*)right_operand->value);
        }
    } else {
        if(result->type == NUM_FLOAT) {
            result->prev_operand_value.f = (float)(*(int*)right_operand->value);
        }else {
            result->prev_operand_value.i = *(int*)right_operand->value;
        }
    }

    if(operator == ADD) {
        if(result->type == NUM_FLOAT) {
            result->value.f = (result->type == NUM_INT ? result->value.i : result->value.f) + (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        } else {
            result->value.i = (result->type == NUM_INT ? result->value.i : result->value.f) + (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        }
    } else if(operator == SUB) {
        if(result->type == NUM_FLOAT) {
            result->value.f = (result->type == NUM_INT ? result->value.i : result->value.f) - (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        } else { 
            result->value.i = (result->type == NUM_INT ? result->value.i : result->value.f) - (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        }
    } else if(operator == MUL) {
        if(result->type == NUM_FLOAT) {
            result->value.f = (result->type == NUM_INT ? result->value.i : result->value.f) * (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        } else { 
            result->value.i = (result->type == NUM_INT ? result->value.i : result->value.f) * (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        }
    } else if(operator == DIV) {
        if(result->type == NUM_FLOAT) {
            result->value.f = (result->type == NUM_INT ? result->value.i : result->value.f) / (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        } else { 
            result->value.i =( result->type == NUM_INT ? result->value.i : result->value.f) / (right_operand->type == INTEGER ? *(int*)right_operand->value : *(float*)right_operand->value);
        }
    }

    return;
}

ExpressionResult* parseExpression(Statement** stmt, bool is_sub_expr) {
    ExpressionResult* result = malloc(sizeof(ExpressionResult));
    result->value.i = 0;
    result->type = -1;
    ExpressionStatement add_sub_result = { .type=-1  };

    Statement* temp = *stmt;

    BinOps curr_operator = -1;
    BinOps prev_operator = -1;
    int itr_count = 0, prev_op_count = -2;

    while(*stmt != NULL && (*stmt)->type != END && (!is_sub_expr || (is_sub_expr && (*stmt)->type != RPARAMCL))) {
        NodeType curr_type = (*stmt)->type;
        void* curr_value = (*stmt)->value;
        bool iterate_next = true;
        switch(curr_type){
            case RPARAMOP:
                *stmt = (*stmt)->next;
                ExpressionResult* sub_expr_result = parseExpression(stmt, true);
                if(sub_expr_result == NULL) {
                    free(result);
                    return NULL;
                }

                if(sub_expr_result->type == NUM_FLOAT){
                    (*stmt)->type = FLOAT;
                    free((*stmt)->value);
                    (*stmt)->value = malloc(sizeof(float));
                    *(float*)(*stmt)->value = sub_expr_result->value.f;
                } else {
                    (*stmt)->type = INTEGER;
                    free((*stmt)->value);
                    (*stmt)->value = malloc(sizeof(int));
                    *(int*)(*stmt)->value = sub_expr_result->value.i;
                }
                iterate_next = false;
                free(sub_expr_result);
                break;
            case INTEGER:
            case FLOAT:
                if(curr_operator == (BinOps)-1){
                    if(result->type != (NumType)-1) {
                        if(curr_type == INTEGER){
                            printf("Unexpected number %d\n", *(int*)curr_value);
                        } else {
                            printf("Unexpected number %f\n", *(float*)curr_value);
                        }
                        free(result);
                        return NULL;
                    }

                    if(curr_type == FLOAT){
                        result->type = NUM_FLOAT;
                        result->value.f = *(float*)curr_value;
                    } else {
                        result->type = NUM_INT;
                        result->value.i = *(int*)curr_value;
                    }
                } else {
                    execMathOp(result, *stmt, curr_operator);
                }
                break;
            case BINARYOP:
                BinOps op = *(BinOps*)curr_value;
                if(prev_op_count == itr_count-1) {
                    printf("Unexpected literal %c\n", getOperatorChar(op));
                    free(result);
                    return NULL;
                }
                prev_op_count = itr_count;
                prev_operator = curr_operator;
                curr_operator = op;

                if(result->type == (NumType)-1){
                    if(op == ADD || op == SUB){
                        result->type = NUM_INT;
                        result->value.i = 0;
                    } else {
                        printf("Unexpected literal %c\n", getOperatorChar(op));
                        free(result);
                        return NULL;
                    }
                }
                
                if((curr_operator == MUL || curr_operator == DIV) && (prev_operator == ADD || prev_operator == SUB)){
                    if(result->type == NUM_FLOAT) {
                        if(prev_operator == SUB) {
                            add_sub_result.value.f = result->value.f + result->prev_operand_value.f;
                        } else {
                            add_sub_result.value.f = result->value.f - result->prev_operand_value.f;
                        }
                        result->value.f = result->prev_operand_value.f;
                    } else {
                        if(prev_operator == SUB) {
                            add_sub_result.value.i = result->value.i + result->prev_operand_value.i;
                        } else {
                            add_sub_result.value.i = result->value.i - result->prev_operand_value.i;
                        }
                        result->value.i = result->prev_operand_value.i;
                    }
                    add_sub_result.type = result->type;
                    add_sub_result.op = prev_operator;
                } 
                else if((curr_operator == ADD || curr_operator == SUB) && (prev_operator == MUL || prev_operator == DIV)) {
                    addPendingResult(&result, &add_sub_result);
                }
                
                if(op == DIV && result->type != NUM_FLOAT){
                    result->type = NUM_FLOAT;
                    result->value.f = (float)(result->value.i);
                }
                break;
            default:
                printf("Unexpected literal %c\n", *(char*)curr_value);
                free(result);
                return NULL;
        }
        if(iterate_next == true) {
            *stmt = (*stmt)->next;
        }
        itr_count++;
    }

    if(is_sub_expr && (*stmt)->type != RPARAMCL) {
        printf("Missing )\n");
        free(result);
        return NULL;
    }

    if(add_sub_result.type != (NumType)-1) {
        addPendingResult(&result, &add_sub_result);
    }

    return result;
}

void addPendingResult(ExpressionResult** result, ExpressionStatement* add_sub_result){
    if((*result)->type == NUM_FLOAT) {
        if(add_sub_result->type == NUM_FLOAT) {
            if(add_sub_result->op == SUB) {
                (*result)->value.f = add_sub_result->value.f - (*result)->value.f;
            } else {
                (*result)->value.f += add_sub_result->value.f;
            }
        } else {
            if(add_sub_result->op == SUB) {
                (*result)->value.f = (float)add_sub_result->value.i - (*result)->value.f;
            } else {
                (*result)->value.f += (float)add_sub_result->value.i;
            }
        }
    } else {
        if(add_sub_result->type == NUM_FLOAT) {
            if(add_sub_result->op == SUB) {
                (*result)->value.i = (int)add_sub_result->value.f - (*result)->value.i;
            } else {
                (*result)->value.i += (int)add_sub_result->value.f;
            }
        } else {
            if(add_sub_result->op == SUB) {
                (*result)->value.i = add_sub_result->value.i - (*result)->value.i;
            } else {
                (*result)->value.i += add_sub_result->value.i;
            }
        }
    }
    add_sub_result->type = -1;
    add_sub_result->op = -1;
    add_sub_result->value.i=0;
    add_sub_result->value.f=0;
    add_sub_result->value.d=0;
}

void parse(Statement* program){
    Statement* stmt = program->next;
    
    while(stmt != NULL && stmt->type != END) {
        switch(stmt->type){
            case INTEGER:
            case FLOAT:
            case RPARAMOP:
                ExpressionResult* result = parseExpression(&stmt, false);

                if(result == NULL) {
                    return;
                }
                printResult(result);

                free(result);
                break;
            case BINARYOP:
                BinOps op = *(BinOps*)stmt->value;
                if((op == ADD || op == SUB) && stmt->next != NULL && (stmt->next->type == INTEGER || stmt->next->type == FLOAT)) {
                    ExpressionResult* result = parseExpression(&stmt, false);

                    if(result == NULL) {
                        return;
                    }
                    printResult(result);
                    
                    free(result);
                    break;
                }
                //fall through
            default:
                if(stmt->type == BINARYOP){
                    printf("Unexpected literal %c\n", getOperatorChar(*(BinOps*)stmt->value));
                } else {
                    printf("Unknown literal %c\n", *(char*)stmt->value);
                }
                return;
        }
        if(stmt != NULL){
            stmt = stmt->next;
        }
    }
}
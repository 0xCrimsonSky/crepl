#ifndef GLOBAL
#define GLOBAL

#include<stddef.h>

typedef enum {
    NUM_INT,
    NUM_FLOAT,
    NUM_DOUBLE
} NumType;

typedef struct {
    void* data;
    size_t size;
} DynArray;

#endif
#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_UNDEFINED,
} DataType;

typedef struct symbol {
    char* name;
    DataType type;
    int isArray;

    union {
        int intValue;
        float floatValue;
        char charValue;
        char* stringValue;

        int* intArray;
        float* floatArray;
        char* charArray;
        char** stringArray;
    } data;

    int* dimensions;
    int numDimensions;
    struct symbol* next;
} symbol;


extern symbol* s;

symbol* slook(char*);
symbol* sUpdate(char*, DataType, int, int*);
void sVal(char*, int*, float, char*);
int getIndex(int*, int*, int);
void printSym();
void freeSymbolTable();

#endif

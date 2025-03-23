#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NODE_INT,
    NODE_FLOAT,
    NODE_CHAR,
    NODE_STRING,
    NODE_BOOL,
    NODE_VAR,
    NODE_ASSIGN,
    NODE_OP,
    NODE_ARRAY_ACCESS,
    NODE_WRITE,
    NODE_FOR,
    NODE_WHILE,
    NODE_IF,
    NODE_IF_ELSE,
    NODE_START,
    NODE_STMT_LIST,
    NODE_DECL,
    NODE_BREAK,
    NODE_INT_DEC,
    NODE_FLOAT_DEC,
    NODE_STR_DEC,
    NODE_STR_VAR,
    NODE_FUNC_CALL,
    NODE_FUNC_DEF,
    NODE_RETURN,
    NODE_CONST
} NodeType;


typedef struct node 
{
    NodeType type;
    union {
        int intValue;
        float floatValue;
        char* stringValue;
        char charValue;
    } data;
    
    char* varName;
    struct node* child;
    struct node* sibling;
    int childCount;
    
} node;

typedef struct eval
{
    NodeType type;
    union{
        int intValue;
        float floatValue;
        char* stringValue;
        char charValue;
    } data;

} eval;


node* createNode(char*, char*, node*, node*, float, char*);
void assignStmt(node*);
void forStmt(node*);
int tableUpdate(node*);
void print_tree(node*);
void evaluate_tree(node*);

#endif

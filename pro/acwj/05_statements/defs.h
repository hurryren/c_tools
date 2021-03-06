#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

// structure and enum definitions

#define TEXTLEN  512 // length of symbols in input

// Tokens
enum {
    T_EOF,T_PLUS, T_MINUS, T_STAR,T_SLASH,T_INTLIT, T_SEMI, T_PRINT
};

// Token structure
struct token{
    int token;
    int intvalue;
};

// AST node types
enum {
    A_ADD,A_SUBTRACT,A_MULTIPLY,A_DIVIDA,A_INTLIT
};

// Abstract syntax Tree structure
struct ASTnode{
    int op;                 // "operation" to be performed on this tree
    struct ASTnode *left;   // Left and right child trees
    struct ASTnode *right;
    int intvalue;           // For A_INTLIT, the integer value
};




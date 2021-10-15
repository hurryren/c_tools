#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define TEXTLEN 512  // length of symbols in input
#define NSYMBOLS  1024 // number of symbol table entries

// Token types
enum{
    T_EOF,
    T_PLUS, T_MINUS,
    T_STAR,T_SLASH,
    T_EQ,T_NE,
    T_LT,T_GT,T_LE,T_GE,
    T_INTLIT,T_SEMI,T_ASSIGN,T_IDENT,
    T_PRINT,T_INT
};


struct token{
    int token; // token type from the enum list above
    int intvalue; // for T_INTLIT, the integer value
};

// ast node types, the first few line up with the related tokens
enum{
    A_ADD=1,A_SUBTRACT,A_MULTIPLY,A_DIVIDE,
    A_EQ,A_NE,A_LT,A_GT,A_LE,A_GE,
    A_INTLIT,
    A_IDENTM,A_LVIDENTM, A_ASSIGN
};


// abstruct syntax tree structure
struct ASTnode{
    int op;
    struct ASTnode *left;
    struct ASTnode *right;
    union{
        int intvalue;
        int id;
    }v;
};

struct symtable{
    char *name; // name of symbol
};


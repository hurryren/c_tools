#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

// structure and enum definitions

#define TEXTLEN  512 // length of symbols in input
#define NSYMBOLS    1024  // number os symbol table entries

// Tokens
enum {
    T_EOF,T_PLUS, T_MINUS, T_STAR,T_SLASH,T_INTLIT, T_SEMI,T_EQUALS,
    T_IDENT,
    // keywords
    T_PRINT,T_INT
};

// Token structure
struct token{
    int token;
    int intvalue;
};


// AST node types
enum {
    A_ADD,A_SUBTRACT,A_MULTIPLY,A_DIVIDA,A_INTLIT,
    A_IDENT,A_LVIDENT,A_ASSIGN
};

// Abstract syntax Tree structure
struct ASTnode{
    int op;                 // "operation" to be performed on this tree
    struct ASTnode *left;   // Left and right child trees
    struct ASTnode *right;
    union {
        int intvalue;           // For A_INTLIT, the integer value
        int id;     // for A_IDENT, the symbol slot number
    }v;
};

// symbol table structure
struct symtable {
    char *name;     // name of a symbol
};

#include "defs.h"
#include "data.h"
#include "decl.h"

// AST tree function

// build and return a generic AST node
struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right,int intvalue){
    struct ASTnode *n;
    // malloc a new ASTnode
    n = (struct ASTnode *) malloc(sizeof(struct ASTnode));
    if(n == NULL){
        fatal("unable to malloc in mkastnode()\n");
    }

    // copy in the field values and return it
    n->op = op;
    n->left = left;
    n->right = right;
    n->v.intvalue = intvalue;
    return n;
}

// make an AST leaf node
struct ASTnode *mkastleaf(int op,int intvalue){
    return mkastnode(op, NULL, NULL, intvalue);
}

// make a unary AST node: only one child
struct ASTnode *mkastunary(int op, struct ASTnode*left, int intvalue){
    return mkastnode(op, left, NULL,intvalue);
}

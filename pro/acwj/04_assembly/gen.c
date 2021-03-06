#include "defs.h"
#include "data.h"
#include "decl.h"

// given an AST, generate
// assemble code recursively
static int genAST(struct ASTnode *n)
{
    int leftreg, rightreg;

    // get the left and right sub-tree value
    if (n->left)
        leftreg = genAST(n->left);
    if (n->right)
        rightreg = genAST(n->right);

    switch (n->op)
    {
    case A_ADD:
        return cgadd(leftreg, rightreg);
    case A_SUBTRACT:
        return cgsub(leftreg, rightreg);
    case A_MULTIPLY:
        return cgmul(leftreg, rightreg);
    case A_DIVIDA:
        return cgdiv(leftreg, rightreg);
    case A_INTLIT:
        return cgload(n->intvalue);
    default:
        fprintf(stderr, "Unknown AST operator %d\n", n->op);
        exit(1);
    }
}

void generatecode(struct ASTnode *n){
    int reg;
    cgpreamble();
    reg = genAST(n);
    cgprintint(reg);
    cgpostamble();
}
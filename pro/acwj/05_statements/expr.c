#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse a primary factor and return an
// Ast node representing it
static struct ASTnode *primary(void)
{
    struct ASTnode *n;

    // for an INTLIT token, make a leaf AST node for it
    // scan in the next token. other wise, a syntax error
    // for any other token type
    switch (Token.token)
    {
    case T_INTLIT:
        n = mkastleaf(A_INTLIT, Token.intvalue);
        scan(&Token);
        return n;

    default:
        fprintf(stderr, "syntax error on line %d, token %d\n", Line, Token.token);
        exit(1);
    }
}

// convert a binary operator token into an AST operation.
static int arithop(int tokentype)
{
    switch (tokentype)
    {
    case T_PLUS:
        return (A_ADD);
    case T_MINUS:
        return (A_SUBTRACT);
    case T_STAR:
        return (A_MULTIPLY);
    case T_SLASH:
        return (A_DIVIDA);
    default:
        fprintf(stderr, "syntax error on line %d, token %d\n", Line, tokentype);
        exit(1);
    }
}

// opeartor precedence for each token
static int OpPrec[] = {0, 10, 10, 20, 20, 0};

// check that we have a binary operator and return its precedence
static int op_precedence(int tokentype)
{
    int prec = OpPrec[tokentype];
    if (prec == 0)
    {
        fprintf(stderr, "syntax error on line %d, token %d\n", Line, tokentype);
        exit(1);
    }
    return prec;
}

// return an ST tree whose root is a binary operator.
// Parameter ptp is the previous token's precedence
struct ASTnode *binexpr(int ptp){
    struct ASTnode *left, *right;
    int tokentype;

    // get the integer lliteral on the left
    // fetch the next token at the same time
    left = primary();

    // if we hit a semicolon, return just the left node
    tokentype = Token.token;
    if(tokentype == T_SEMI)
        return left;

    // while the precedence of this token is
    // more than that of the previous token precedence
    while  (op_precedence(tokentype) > ptp){
        // fetch in the next integer literal
        scan(&Token);

        // recursively call binexpr() with the
        // precedence of our token to build a sub-tree
        right = binexpr(OpPrec[tokentype]);

        // join that sub-tree with ours. convert the token
        // into an AST operation at the same time
        left = mkastnode(arithop(tokentype), left, right,0);

        // update the details of the current token
        // if we hit a semicolon, return just the left node
        tokentype = Token.token;
        if(tokentype == T_SEMI)
            return left;
    }

    // return the tree we have when the precedence
    // is the same or lower
    return left;
}
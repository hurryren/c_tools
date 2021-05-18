#include "defs.h"
#include "data.h"
#include "decl.h"

// Parsing of expressions with full recursive descent

// parse a primary factor and return an
// ast node representing it
static struct ASTnode *primary(void){
    struct ASTnode *n;

    // for an intlit token, make a leaf AST node for it
    // and scan in the next token. Otherwise, a syntax error
    // for any other token type
    switch (Token.token)
    {
        case T_INTLIT:
            n = mkastleaf(A_INTLIT, Token.intvalue);
            scan(&Token);
            return n;
    default:
        fprintf(stderr,"syntax error on line %d, token %d\n",Line, Token.token);
        exit(1);
    }
}

// convert a binary operator token into an AST operation
static int arithop(int tok){
    switch(tok){
        case T_PLUS:
            return A_ADD;
        case T_MINUS:
            return A_SUBTRACT;
        case T_STAR:
            return A_MULTIPLY;
        case T_SLASH:
            return A_DIVIDA;
        default:
            fprintf(stderr, "syntax error on line %d, token %d\n",Line,tok);
            exit(0);
    }
}

struct ASTnode * additive_expr(void);

// return an AST tree whose root is a '*' or '/' binary operator
struct ASTnode *multiplicative_expr(void){
    struct ASTnode *left, *right;
    int tokentype;

    // Get the integer literal on the left
    // fetch the next token at the same time
    left = primary();

    // if no tokens left, return just the left node
    tokentype = Token.token;
    if(tokentype == T_EOF)
        return left;

    // while the token is a '*' or '/'
    while((tokentype == T_STAR) || (tokentype == T_SLASH)){
        // fetch in the next integer literal
        scan(&Token);
        right = primary();

        // join that with the left integer literal
        left = mkastnode(arithop(tokentype), left, right,0);

        // update the details of the current token
        // if no token leftm return just the left node
        tokentype = Token.token;
        if(tokentype == T_EOF)
            break;
    }
    // return whatever tree we have created
    return left;
}

// return an AST tree whose root is a '+' or '-' binary operator
struct ASTnode *additive_expr(void){
    struct ASTnode *left, *right;
    int tokentype;

    // get the left sub-tree at a higher precedence than us
    left = multiplicative_expr();

    // if no tokens left, return  just the lft node
    tokentype = Token.token;
    if(tokentype == T_EOF)
        return left;

    // cache the '+' or '-' token type

    // loop working on token at our level of precedende
    while(1){
        // fetch in the next integer literal
        scan(&Token);

        // get the right sub-tree at a highter precedence than us
        right = multiplicative_expr();

        // join the right sub-tree at a higher precedence than us
        left = mkastnode(arithop(tokentype),left, right,0);

        // and get the next token at our precedence
        tokentype = Token.token;
        if(tokentype == T_EOF)
            break;
    }
    // return whatever tree we have created
    return left;
}

struct ASTnode *binexpr(int n){
    return additive_expr();
}
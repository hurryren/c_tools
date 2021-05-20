#include "defs.h"
#include "data.h"
#include "decl.h"

// parsing of statments

// statements: statement
//      | statement statements
//      ;
//
//  statement: 'print' expression ';'
//      ;

// parse one or more statements
void statements(void){
    struct ASTnode *tree;
    int reg;

    while(1){
        // match a 'print' as the first token
        match(T_PRINT, "print");

        // parse the following expresion and
        // generate the assemble code
        tree = binexpr(0);
        reg = genAST(tree);
        genprintint(reg);
        genfreeregs();

        // match the following semicolon
        // and stop if we are at EOF
        semi();
        if(Token.token == T_EOF)
            return;
    }
}
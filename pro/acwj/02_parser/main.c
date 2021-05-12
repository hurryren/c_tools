#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"
#include <errno.h>


// compiler setup and top-level execution

// initialise global variables
static void init(){
    Line = 1;
    Putback = '\n';
}

// print out a usage if started incorrectly
static void usage(char *prog){
    fprintf(stderr,"Usage: %s infile\n",prog);
    exit(1);
}

// list of printable tokens
char *tokstr[] = {"+","-","*","/","intlit"};

// main program: check arguments and print a usage
// if we don't have an argument. Open up the input
// file and call scanfile() to scan the tokens in it
void main(int argc, char *argv[]){
    struct ASTnode *n;

    if(argc != 2)
        usage(argv[0]);

    init();

    if((Infile = fopen(argv[1],"r")) == NULL){
        fprintf(stderr, "Unable to open %s: %s\n", argv[1],strerror(errno));
        exit(1);
    }
    scan(&Token);   // get the first token from the input
    n = binexpr();  // Parse the expression in the file
    printf("%d\n",interpretAST(n));     // Calculate the final result
    exit(0);
}
#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"
#include <errno.h>

static void init()
{
    Line = 1;
    Putback = '\n';
}

static void usage(char *prog)
{
    fprintf(stderr, "usage:%s infile\n", prog);
    exit(1);
}

void main(int argc, char *argv[])
{
    if (argc != 2)
        usage(argv[0]);

    init();

    // open up the input file
    if ((Infile = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "unable to open %s: %s\n",argv[1],strerror(errno));
        exit(1);
    }

    // create output file
    if((Outfile = fopen("out.s", "w")) == NULL){
        fprintf(stderr," Unable to create out.s: %s",strerror(errno));
        exit(1);
    }

    scan(&Token); // get the first token from the input
    genpreamble();  // output the preamble
    statements();  //parse the statements in the input
    genpostamble(); // output the postamble
    fclose(Outfile); //close the output file and exit
    exit(0);
}
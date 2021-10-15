#include "defs.h"
#include "data.h"
#include "decl.h"

static int chrpos(char *s, int c)
{
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

// get the next character from the input file
static int next(void)
{
    int c;

    if (Putback)
    {
        c = Putback;
        Putback = 0;
        return c;
    }

    c = fgetc(Infile);
    if ('\n' == c)
        Line++;
    return c;
}

static void putback(int c){
    Putback=c;
}

static int skip(void){
    int c;
    c = next();
    while(' ' == c || '\t' == c || '\r' == c || '\f'==c){
        c = next();
    }
    return c;
}

// scan and return an integer literal value from the input file,
// store the value as a string in text
static int scanint(int c){
    int k, val=0;

    // convert each character into an int value
    while((k=chrpos("0123456789",c)) >= 0){
        val = val*10 + k;
        c = next();
    }

    // we hit a non-integer character, put it back
    putback(c);
    return val;
}


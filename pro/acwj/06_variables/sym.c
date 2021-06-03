#include "defs.h"
#include "data.h"
#include "decl.h"

static int Globs = 0;   // Postion of next free global symbol slot

int findglob(char *s){
    int i;
    for(i = 0;i < Globs;i++){
        if(*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
            return i;
    }
    return -1;
}


// get the postion of a new globale symbol slot, or die
// if we've run out of positions
static int newglob(void){
    int p;
    if((p = Globs ++ ) >= NSYMBOLS)
        fatal("Too many global symbols");
    return p;
}

// add a global symbol to the symbol table.
// return the slot number in the symbol table
int addglob(char *name){
    int y;

    // if this is already in the symbol table, return the existing slot
    if((y = findglob(name)) != -1)
        return y;

    // otherwise get a new slot, fill it in and
    // return the slot number
    y = newglob();
    Gsym[y].name = strdup(name);
    return y;
}
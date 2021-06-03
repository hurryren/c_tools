#ifndef extern_
    #define extern_ extern;
#endif
// Global variables

extern_ int     Line;
extern_ int     Putback;
extern_ FILE    *Infile;
extern_ FILE    *Outfile;
extern_ struct token Token;
extern_ char Text[TEXTLEN +1]; // last identifier scanned
extern_ struct symtable Gsym[NSYMBOLS];  // global symbol table
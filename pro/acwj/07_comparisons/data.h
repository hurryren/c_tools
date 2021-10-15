#ifndef extern_
#define extern_ extern
#endif

extern_ int Line;       // current line number
extern_ int Putback;    // character put back by scanner
extern_ FILE *Infile;   // input and output files
extern_ FILE *Outfile;
extern_ struct token Token;  // last token scanned
extern_ char Text[TEXTLEN+1];  // last identifier scanned
extern_ struct symtable Gsym[NSYMBOLS]; // global symbol table
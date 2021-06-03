#include "defs.h"
#include "data.h"
#include "decl.h"

// list of avaliable registers
// and their names
static int freereg[4];
static char *reglist[4] = {"%r8", "%r9", "%r10", "%r11"};

// set all registers as available
void freeall_registers(void)
{
    freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}

// allocate a free register. Return the number of
// register. Die if no available register
static int alloc_register(void)
{
    for (int i = 0; i < 4; i++)
    {
        if (freereg[i])
        {
            freereg[i] = 0;
            return i;
        }
    }
    fprintf(stderr, "out of registers!\n");
    exit(1);
}

// return a register to the list of available registers.
// check to see if it's not already there
static void free_register(int reg)
{
    if(freereg[reg] != 0){
        fprintf(stderr, "Error trying to free register %d\n",reg);
        exit(1);
    }
    freereg[reg] = 1;
}

// print out the assembly preamble
void cgpreamble(){
    freeall_registers();
    fputs(
        "\t.text\n"
        ".LC0:\n"
        "\t.string\t\"%d\\n\"\n"
        "printint:\n"
        "\tpushq\t%rbp\n"
        "\tmovq\t%rsp, %rbp\n"
        "\tsubq\t$16, %rsp\n"
        "\tmovl\t%edi, -4(%rbp)\n"
        "\tmovl\t-4(%rbp), %eax\n"
        "\tmovl\t%eax, %esi\n"
        "\tleaq\t.LC0(%rip), %rdi\n"
        "\tmovl\t$0, %eax\n"
        "\tcall\tprintf@PLT\n"
        "\tnop\n"
        "\tleave\n"
        "\tret\n"
        "\n"
        "\t.globl\tmain\n"
        "\t.type\tmain, @function\n"
        "main:\n"
        "\tpushq\t%rbp\n"
        "\tmovq\t%rsp, %rbp\n",
        Outfile);
}

// print out the assembly postamble
void cgpostamble(){
    fputs(
        "\tmovl\t$0, %eax\n"
        "\tpopq\t%rbp\n"
        "\tret\n",
        Outfile);
}

// load an integer literal value into a register
// return the number of the register
int cgloadint(int value){

    // get a new register
    int r = alloc_register();

    // print out the coto to initialize it
    fprintf(Outfile,"\tmovq\t$%d, %s\n",value, reglist[r]);
    return r;
}

// load a value from a variable into a register
// return the number of the register
int cgloadglob(char *identifier){

    // get a new register
    int r = alloc_register();

    // print out the coto to initialize it
    fprintf(Outfile,"\tmovq\t%s(\%%rip), %s\n",identifier, reglist[r]);
    return r;
}

// add toew registers together and return
// the number of the register with the result
int cgadd(int r1, int r2){
    fprintf(Outfile,"\taddq\t%s, %s\n",reglist[r1],reglist[r2]);
    free_register(r1);
    return r2;
}

// subtract the second register from the first and
// return the number os the register with the result
// subq S, D      D = D - S
int cgsub(int r1, int r2){
    fprintf(Outfile, "\tsubq\t%s, %s\n",reglist[r2],reglist[r1]);
    free_register(r2);
    return r1;
}

// multiply tow registers together and return
// the number of the register with the result
int cgmul(int r1, int r2){
    fprintf(Outfile,"\timulq\t%s, %s\n",reglist[r1],reglist[r2]);
    free_register(r1);
    return r2;
}

// divide the first register by the second and
// return the number of the register with the result
int cgdiv(int r1, int r2){
    fprintf(Outfile, "\tmovq\t%s, %%rax\n",reglist[r1]);
    fprintf(Outfile,"\tcqo\n");
    fprintf(Outfile,"\tidivq\t%s\n",reglist[r2]);
    fprintf(Outfile,"\tmovq\t%%rax, %s\n",reglist[r1]);
    free_register(r2);
    return r1;
}

// call printint() with the given register
void cgprintint(int r){
    fprintf(Outfile,"\tmovq\t%s, %%rdi\n", reglist[r]);
    fprintf(Outfile,"\tcall\tprintint\n");
    free_register(r);
}

// store a register's value into a variable
int cgstorglob(int r, char *identifier){
  fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], identifier);
    return r;
}

// generate a global symbol
void cgglobsym(char *sym){
    fprintf(Outfile, "\t.comm\t%s,8,8\n",sym);
}
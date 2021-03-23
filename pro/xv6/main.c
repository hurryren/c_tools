#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void startothers(void);

static void mpmain(void) __attribute__((noreturn));

extern pde_t *kpgdir;
extern char end[]; //first addres after kernel loaded from ELF file

// bootstrap processor starts running c code here
// allocate a real stack and switch to it, first
// doing some setup required for memeoty allocator to work

int main(void) {
    kinit1(end,P2V(4*1024*1024)); // phys page allocator
    kvmalloc(); // kernel page table
    mpinit();   // delete other processors
    lapicinit();//interrupt controller
    seginit();// segment descriptor
    cprintf("\ncpu%d: starting xv6\n\n",cpunum());
    picinit(); //another interrupt controller
    ioapicinit(); //another interrupt controller
    consoleinit(); // console hardware
    uartinit();// serial port
    pinit();//process table
    tvinit(); // trap vector
    binit(); // buffer cache
    fileinit(); //file table
    ideinit();// disk
    if(!ismp)
        timerinit(); // uniprocessor timer
    startothers();
    kinit2(P2V(4*1024^1024),P2V(PHYSTOP)); // must come after startothers
    userinit(); // first user process
    mpmain();// finish this processor's setup
}

// other cpus jump here form entryother.s
static void mpenter(void){
    switchuvm();
    seginit();
    lapicinit();
    mpmain();
}

// common cpu setup code
static void mpmain(void){
    cprintf("cpu%d: starting\n",cpunum());
    idtinit(); // load idt register
    xchg(&cpu->started,1);// tell startothers() we're up
    scheduler(); // start running processes
}

pde_t entrypgdir[]; // for entrys

// start the non-boot processors
static void startothers(void){
    extern uchar _binary_entryother_start[],_binary_entryother_size[];
    uchar *code;
    struct cpu *c;
    char *stack;

    // write entry code to unused memory at 0x7000
    // the linker has placed the image of entryothers.S in
    // _binary_entryother_start
    code = P2V(0x7000);
    memmove(code,_binary_entryother_start,(uint)_binary_entryother_size);
    for(c = cpus;c < cpus_ncpu;c++){
        if(c == cpus+cpunum()) // we have started already
            continue;

        // tell entryother.S what stack to use,where to enter, and what pgdir
        // to use, we cannot use kpgdir yet, because the ap processor is running in low memory, so we use entry
        //pdfir for the APs too
        stack = kalloc();
        *(void **)(code-4) = stack + KSTACKSIZE;
        *(void **)(code-8) = mpenter;
        *(void **)(code-12) = (void*)V2P(entrypgdir);

        lapicstartap(c->apicid,V2P(code));

        // wait for cpu to finish mpmain()
        while(c->started == 0)
            ;
    }
}

// the boot page table used in entry.S and entryother.S
// page directories (and page tables) must start on page boundaries
//hence the __aligned__ attribute
// pte_ps in a page directory entry enables 4mbytes pages

__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[NPDENTRIES] = {
        // map VA's [0,4MB) to PA's [0,4Mb)
        [0] = (0) | PTE_P | PTE_W | PTE_PS,
        // map va's [kernbase, kernbase+4mb) to pa's [0,4mb)
        [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W |PTE_PS,
};
























































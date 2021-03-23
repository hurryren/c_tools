#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "proc.h"
#include "elf.h"

extern char data[]; //define by kernel.ld
pde_t *kpgdir;  // for use in scheduler()

// set up cpu's kernel segment descriptors
// run once on entry on each cpu
void seginit(void){
    struct cpu *c;
    // map logical addresses to virtual addresses using identity map
    // cannot share a code descriptor for both kernel and user because
    // it would have to have dpl_usr, but the cpu forbids an interrupt
    // from cpl=0 to dpl=3
    c = &cpus[cpunum()];
    c->gdt[SEG_KCODE] = SEG(STA_X|STA_R,0,0xffffffff,0);
    c->gdt[SEG_KDATA] = SEG(STA_W,0,0xffffffff,0);
    c->gdt[SEG_UCODE] = SEG(STA_X|STA_R,0,0xffffffff,DPL_USER);
    c->gdt[SEG_UDATA] = SEG(STA_W,0,0xffffffff,DPL_USER);

    // map  cpu and proc == these are provate per cpu
    c->gdt[SEG_KCPU] = SEG(STA_W,&c->cpu,8,0);

    lgdt(c->gdt,sizeof(c->gdt));
    loadgs(SEG_KCPU << 3);

    // initialize cpu-local storage
    cpu = c;
    proc = 0;

}

// return the  address of the PTE in page trable pgdir
// that corresponds to virtual address va. if alloc != 0
// create any required page table pages
static pte_t * walkpgdir(pde_t *pgdir, const void *va, int alloc){
    pde_t *pde;
    pte_t *pgtab;

    pde = &pgdir[PDX(va)];
    if(*pde & PTE_P){
        pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    }else{
        if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
            return 0;
        // makesure all those pte_p bits are zero
        memset(pgtab,0,PGSIZE);
        // the permission here are overly generous, but they can be
        // further restricted by the permissions in the page table entries, i
        // if necessary
        *pde = V2P(pgtab) | PTE_P |　PTE_Ｗ |PTE_U;
    }
    return &pgtab[PTX(va)];
}

// create PTes for virtual addresses starting at va that refer to physical
// address starting at pa. va and size might not be page-aligned
static int mappages(pde_t *pgdir, void * va, uint size, uint pa, int perm){
    char *a, *last;
    pte_t *pte;

    a = (char*)PGROUNDDOWN((uint)va);
    last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
    for(;;){
        if((pte = walkpgdir(pgdir,a,1)) == 0)
            return -1;
        if(*pte & PTE_P)
            panic("remap");
        *pte = pa | perm | PTE_P;
        if(a == last)
            break;
        a += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

// there is
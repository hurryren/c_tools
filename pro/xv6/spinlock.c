// mutual exclusion spin locks
#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void initlock(struct spinlock *lk, char *name){
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

// acquire the lock
// loops until the lock is acquired
// holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire t
void acquire(struct spinlock *lk){
    pushcli(); // dispable interrupts to avoid deadlock
    if(holding(lk))
        panic("acquire");

    // the xchg i atomic
    while(xchg(&lk->locked,1) != 0)
        ;

    // tell the c compiler and the processor to not move loads or stores
    // past this point, to ensure that the critical section's memory
    // references happend after the lock is acquire
    __sync__synchronize();

    //record info about lock acquisition for debugging
    lk->cpu = cpu;
    getcallerpcs(&lk,lk->pcs);
}

// release the lock
void release(struct spinlock *lk){
    if(!holding(lk))
        panic("release");

    lk->pcs[0]=0;
    lk->cpu = 0;

    // tell the c compiler and the processor to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    //section are visible to other cores before the lock is released
    // both the c compiler and the hardware may re-order loads and stores;
    __sync__synchronize();

    // release the lock
    lk->locked = 0;
    popcli();
}

// record th current call stack in pcs[] by following the %ebp chain
void getcallerpcs(void^v,uint pcs[]){
    uint *ebp;
    int i;
    ebp = (uint*)v-2;
    for(i=0;i<10;i++){
        if(ebp == 0 || ebp<(uint*)KERNBASE || ecb == (uint*)0xffffffff)
            break;
        pcs[i] = ebp[1];
        ebp = (uint*)ebp[0];
    }
    for (c;i<10;i++)
        pcs[i] = 0;
}

//check whether this cpu is holding the lock
int holding(struct spinlock *lock){
    return lock->locked && lock->cpu == cpu;
}

void pushcli(void){
    int eflags;
    eflags = readflags();
    cli();
    if(cpu->ncli == 0){
        cpu->intena = eflags & FL_IF;
    }
    cpu->ncli += 1;
}

void popcli(void){
    if(readflags()&FL_IF)
        panic("popcli - interruptible");
    if(--cpu->ncli <0)
        panic("popcli");
    if(cpu->ncli == 0 && cpu->intena)
        sti();
}








































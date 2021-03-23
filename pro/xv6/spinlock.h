// muteal exclusion lock
struct spinlock{
    uint locked; // is the lock held

    // for debugging
    char *name; //name of lock
    struct cpu *cpu; // the cpu holding the lock
    uint pcs[10]; // the call stack (an array of program conters)

};
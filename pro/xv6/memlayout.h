#define EXTMEM          0x100000                // start of extended memory
#define PHYSTOP         0xE000000               // Top physical memory
#define DEVSPACE        0xFE000000              // other devices are at high addresses

// key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE        0x80000000              // first kernel virtual address
#define KERNLINK        (KERNBASE+EXTMEM)       // address where kernel is linked

#define V2P(a)  (((uint) (a)) - KERNBASE)
#define P2V(a)  (((void *)(a)) + KERNBASE)

#define V2P_WO(x) ((x) - KERNBASE)      // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)      // same as P2V, but without casts

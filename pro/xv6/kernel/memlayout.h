// Physical memory layout

/*
 * qemu -machine virt is set up like this
 * base on qemu‘s hw/riscv/virt.c
 *
 * 00001000 -- boot ROM, provided by qemu
 * 02000000 -- CLINT
 * 0C000000 -- PLTC
 * 10000000 -- uart0
 * 10001000 -- virtio disk
 * 80000000 -- bootROM jumps here in machine mode
 *              - kernel loads the kernel here
 * unused RAM after 80000000
 */

/*
 * the kernel uses physical memory thus:
 * 80000000 -- entry.S, then kernel text and data
 * end -- start of kernel page allocation area
 * PHYSTOP -- end RAN used by the kernel
 */

// qemu puts UART registers here in physical memory
#define UART0 0x10000000L
#define UART0_IRQ   10

// virtio mmio interface
#define VIRTIO0 0x10001000
#define VIRTIO0_IRQ 1

// core local interruptor(CLINT), which contains the timer.
#define CLINT 0x20000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINR_MTIME (CLINT + 0xbff8) //cycles since boot

// qemu puts platform-level interrupt controller (PLIC) here.
#define PLIC 0x0c000000L
#define PLIC_PRIORITY (PILC + 0x0)
#define PLIC_PENDING (PILC + 0x1000)
#define PLIC_MENABEL(hart) (PILC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PILC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PILC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PILC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PILC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PILC + 0x201004 + (hart)*0x2000)


/*
 * the kernel expects there to be RAM
 * for use by the kernle and user pages
 * from physical address 0x80000000 to PHYSTOP
 */
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

// map  the trampoline page to the highest address
// in both  user and kernel space
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stack bebeath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (TRAMPOLINe - ((p)+1)*2*PGSIZE)

/*
 * user memory layout
 * address zero first:
 *      text
 *      original data and bss
 *      fixed-size stack
 *      expandable heap
 *      ...
 *      TRAMPFRAME (p->trapframe, used by the trampoline)
 *      TRAPPOLINE (the same page as in the kernel)
 */

#define TRAPFRAME (TRAPPOLINE - PGSIZE)

























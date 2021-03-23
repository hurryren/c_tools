// this file contains definitions for the
// x86 memeory management uint (mmu)

// eflags register
#define FL_CF           0x00000001      // carry flag
#define FL_PF           0x00000004      // parity flag
#define FL_AF           0x00000010      // auxiliary carry flag
#define FL_ZF           0x00000040      // zero flag
#define FL_SF           0x00000080      // sign flag
#define FL_TF           0x00000100      // trap flag
#define FL_IF           0x00000200      // interrupt flag
#define FL_DF           0x00000400      // direction flag
#define FL_OF           0x00000800      // overflow flag
#define FL_IOPL_MASK    0x00003000      // I/O privilege level bit mask flag
#define FL_IOPL_0       0x00000000      // IOPL == 0
#define FL_IOPL_1       0x00001000      // IOPL == 1
#define FL_IOPL_2       0x00002000      // IOPL == 2
#define FL_IOPL_3       0x00003000      // IOPL == 3
#define FL_NT           0x00004000      // nested task
#define FL_RF           0x00010000      // resume flag
#define FL_VM           0x00020000      // virtual 8086 mode
#define FL_AC           0x00040000      // alignment check
#define FL_VIF          0x00080000      // virtual interrupt flag
#define FL_VIP          0x00100000      // virtual interrupt pending
#define FL_ID           0x00200000      // ID flag

// control register flags
#define CR0_PF          0x00000001      // protection enable
#define CR0_MP          0x00000002      // monitor coprocessor
#define CR0_EM          0x00000004      // emulation
#define CR0_TS          0x00000008      // task switched
#define CR0_ET          0x00000010      // extension type
#define CR0_NE          0x00000020      // numeric error
#define CR0_WP          0x00010000      // write protect
#define CR0_AM          0x00040000      // alignment mask
#define CR0_NW          0x20000000      // not writethrough
#define CR0_CD          0x40000000      // chche disable
#define CR0_PG          0x80000000      // paging

#define CR4_pse         0x00000010      // page size extension

// various segment selectors.
#define SEG_KCODE   1 // kernel code
#define SEG_KDATA   2 // kernel data + stack
#define SEG_KCPU    3 // kernel per-cpu data
#define SEG_UCODE   4 // user code
#define SEG_UDATD   5 // user data+stack
#define SEG_TSS     6 // this process's task state

// cpu->gdt[NSEGS] holds the above segments
#define NSEGS       7


#ifndef __ASSEMBLER__
// segment descriptor
struct segdesc{
    uint lim_15_0: 16; // low bits of segment limit
    uint base_15_0: 16; // low bits of segment base address
    uint base_23_16: 8; // middle bits of segment base address
    uint type:4;        // segment type (see sts_ constant)
    uint s:1;           // 0 = system, 1 = application
    uint dpl:2;         // descriptor privilege level
    uint p:1;           // present
    uint lim_19_16 : 4; // high bits of segment limit
    uint avl :1 ;       // present
    uint rsvl:1;        // reserved
    uint db: 1;         // 0 = 16-bit segment, 1 = 32-bit segment
    uint g:1;           // granularity:limit caled by 4k when set
    uint base_31_24:8: // high bits of segment base address
};

// normal segment
#define SEG(type, base, lim,dpl)(struct segdesc) \
    {((lim)>>12)&0xffff,(uint)(base)&0xffff,\
    ((uint )(base)>>16)&0xff, type,1,dpl,1,      \
    (uint)(lim)>>28,0,0,1,1,(uint)(base)>24}

#define SEG16(type, base, lim,dpl)(struct segdesc) \
    {(lim) &0xffff,(uint)(base)&0xffff,\
    ((uint )(base)>>16)&0xff, type,1,dpl,1,      \
    (uint)(lim)>>16,0,0,1,1,(uint)(base)>24}
#endif

#define DPL_USER    0x3     // user dpl

// aplication segment type bits
#define STA_X   0x8     // executable segment
#define STA_E   0x4     // expand down (non-executable segments)
#define STA_C   0x4     // conforming code segment (executable only)
#define STA_W   0x2     // writeable (non-executable segments)
#define STA_R   0x2     // readable executable segment
#define STA_A   0x1     // accessed

//system segment type bits
#define STA_T16A    0x1     // available 16-bit tss
#define STA_LDT     0x2     // local descriptor table
#define STA_T16B    0x3     // busy 16-bit tss
#define STA_CG16    0x4     // 16-bit call gate
#define STA_TG      0x5     // task gate / coum transmitions
#define STA_IG16    0x6     // 16-bit interrupt gate
#define STA_TG16    0x7     // 16-bit trap gate
#define STA_T32A    0x9     // avaliable 32-bit tss
#define STA_T32B    0xb     // busy 32bit tss
#define STA_CG32    0xc     // 32bit call gate
#define STA_IG32    0xE     // 32bit interrupt gate
#define STA_TG32    0xF     // 32-bit trap gate


// A virtual address ’la’ has a three−part structure as follows:
//
// +−−−−−−−−10−−−−−−+−−−−−−−10−−−−−−−+−−−−−−−−−12−−−−−−−−−−+
// | Page Directory | Page Table | Offset within Page |
// | Index | Index | |
// +−−−−−−−−−−−−−−−−+−−−−−−−−−−−−−−−−+−−−−−−−−−−−−−−−−−−−−−+
// \−−− PDX(va) −−/ \−−− PTX(va) −−/

// page directory index
#define PDX(va) (((uint)(va) >> (PDXSHIFT) & 0x3ff))

// page table index
#define PTX(va) (((uint)(va)>>PTXSHIFT) &0x3ff)

// construct virtual address from indexes and offset
#define PGADDR(d,t,o) ((uint) ((d)<<PDXSHIFT | (t) <<PTXSHIFT | (o)))

// page directory and page table constans
#define NPDENTRIES      1024        // directory entries per page directory
#define NPTENTRIES      1024        // PTEs per page table
#define PGSIZE          4096        // bytes mapped by a page

#define PGSHIFT         12          // log2(pgsize)
#define PTXSHIFT        12          // offset of PTX in a linear address
#define PDXSHIFT        22          // offset of PDX in a linear address


#define PGROUNDUP(sz) (((sz)+PGSIZE-1) & (PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

// page table/directory entry flags
#define PTE_P       0x001   // present
#define PTE_W       0x002   // writeabel
#define PTE_U       0x004   // user
#define PTE_PWT     0x008   // write-through
#define PTE_PCD     0x010   // cached-disable
#define PTE_A       0x020   // accessed
#define PTE_D       0x040   // dirty
#define PTE_PS      0x080   // page size
#define PTE_MBZ     0x180   // bits must be sero


// address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint)(pte) & ~0xfff)
#define PTE_FLAGS(pte)   ((uint)(pte) & ~0xfff)

#ifndef __ASSEMBLER__
typedef uint pte_t;

// task state segment format
struct taskstate{
    uint link;
    uint esp0;
    ushort ss0;
    ushort padding1;
    uint *esp1;
    ushort ss1;
    ushort padding2;
    uint *esp2;
    ushort ss2;
    ushort padding3;
    uint *esp3;
    ushort ss3;
    void *cr3;
    uint *eip;
    uint eflags;
    uint eax;
    uint ebx;
    uint ecx;
    uint edx;
    uint *esp;
    uint *ebp;
    uint esi;
    uint edi;
    ushort es;
    ushort padding4;
    ushort cs;
    ushort padding5;
    ushort ss;
    ushort padding6;
    ushort ds;
    ushort padding7;
    ushort fs;
    ushort padding8;
    ushort gs;
    ushort padding9;
    ushort ldt;
    ushort padding10;
    ushort t;
    ushort iomb;

};


// gate decriptors for interrupts and traps
struct gatedesc{
    uint off_15_0:16; // low 16 bits of offset in segment
    uint cs:16;// code segment selector
    uint args:5;// #args, 0 for interrupt/trap gates
    uint rsv1:3;// reserved(should be sero I guess)
    uint type:4;//type(STS_{TG,IG32,TG32})
    uint s:1;//must be 0
    uint dpl:2;// descriptor(meaning new) privilege level
    uint p:1;//present
    uint off_31_16:16;// high bits of offset in segment
};

// setup a normal interrupt/trap gate descriptor
/*  - istrap: 1 for a trap(=exception)gate, 0 for an interrupt gate
 *      interrupt gate clears FL_IF, trap gate leaves FL_IF alone
 *  - sel: code segment selector for interrupt/trap handler
 *  - off: offset in code segment for interrupt/trap handler
 *  - dpl: descriptor privilege level -
 *          the privilege level required for software to invoke
 *          this interrupt/trap gate explicity using an int instruction
 *
 */
#define SETGATE(gate, istrap, sel, off, d) \
{ \
    (gate).off_15_0 = (uint)(off) & 0xffff; \
    (gate).cs = (sel); \
    (gate).args = 0; \
    (gate).rsv1 = 0; \
    (gate).type = (istrap) ? STS_TG32 : STS_IG32; \
    (gate).s = 0; \
    (gate).dpl = (d); \
    (gate).p = 1; \
    (gate).off_31_16 = (uint)(off) >> 16; \
}

#endif




























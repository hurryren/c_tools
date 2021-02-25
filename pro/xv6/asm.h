// assembler macros to create x86 segments

#define SEG_NULLASM \
        .word 0,0;   \
        .byte 0,0,0,0

// the 0xC0 means the limit is in 4096-byte uints
// and (for executeable segments) 32-bit mode

#define SEG_ASM(type,base,lim)                         \
        .word (((lim)>>12) & 0xffff), ((base) & 0xffff); \
        .byte (((base) >> 16) &0xff), (0x90 | (type)), \
            (0xC0 | (((lim)>>28) & 0xf)), (((base) >> 24) & 0xff)


#define STA_X   0x8     // executable segment
#define STA_E   0x4     // expand down (non-executable segments)
#define STA_C   0x4     // conforming code segment (executable only)
#define STA_W   0x2     // writeable (non-executable segments)
#define STA_R   0x2     // readable executable segment
#define STA_A   0x1     // accessed

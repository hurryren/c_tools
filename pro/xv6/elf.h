// format of an elf executable file
#define ELF_MAGIC   0x464c457fu // "\x7felf" in little endian

// file header
struct elfhdr{
    uint magic;// must equal ELF_MAGIC
    uchar elf[12];
    ushort  type;
    ushort machine;
    uint version;
    uint entry;
    uint phoff;
    uint shoff;
    uint flags;
    ushort ehsize;
    ushort phentsize;
    ushort phnum;
    ushort shentsize;
    ushort shenum;
    ushort shstrndx;
};

// program section header
struct proghdr{
    uint type;
    uint off;
    uint vaddr;
    uint paddr;
    uint filesz;
    uint memsz;
    uint flags;
    uint align;
};
// value for prohdr type
#define ELF_PROG_LOAD       1

// flag bits for proghdr flags
#define ELF_PROG_FLAG_EXEC  1
#define ELF_PROG_FLAG_WRITE 2
#define ELF_PROG_FLAG_READ  4

































# the xv6 kernel starts executing in this file. this file is linked with
#　the kernel c code，so it can refer to kernel symbols such as main().
# the boot block (bootasm.S and bootmain.c) jumps to entry below
#
# using CRUB 2,  you can boot xv6 from a file stored in a linux file system by
# copying kernel or kernelmemfs to /boot and then adding this menu entry
#
# menuentry "xv6"{
#   insmod ext2
#   set root='{hd0,msdos1)'
#   set root='/boot/kernel'
#   echo "Loading ${kernel}..."
#   multiboot ${kernel} ${kernel}
#   boot
#}


#include "asm.h"
#include "memlayout.h"
#include "mmu.h"
#include "param.h"

# Multiboot header. Data to direct multiboot loader
.p2align 2
.text
.globl multiboot_header

multiboot_header:
    #define magic 0x1badb002
    #define flags 0
    .long magic
    .long flags
    .long (-magic-flags)

# by convention, the symbol specifes the ELF entry point.
# since we haven't set up virtual memory yes, our entry point is
# the physical address  of 'entry'

.globl _start
_start = V2P_WO(entry)

# entering xv6 on boot processor with paging off
.globl entry
entry:
    #turn on page size extension for 4Mbyte pages
    movl    %cr4, %eax
    orl     $(CR4_PSE),%eax
    movl    %eax,%cr4

    #set page directory
    movl $(V2P_WO(entrypgdur)),%eax
    movl %eax,%cr3
    #turn on paging
    movl %cr0,%eax
    orl $(CR0_PG|CR0_WP),%eax
    movl %eax,%cr0

    #set up the stack pointer
    movl $$(stack+KSTACKSIZE),%esp

    # jump to main() and switch to executing at
    # high address. The indirect call is needed because
    # the assembler produces a PC-relative instruction
    # for a direct jump
    mov $main,%eax
    jmp *eax

.comm stack, KSTACKSIZE

























































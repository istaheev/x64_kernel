#define ASM_FILE 1

#include <multiboot.h>
#include <kernel.h>

#define MULTIBOOT_HEADER_FLAGS  0x00010003

        .text

        .globl entry, multiboot_entry

        .code32

entry:
        jmp multiboot_entry

        /* Align 32 bits boundary. */
        .align  4
 
        /* Multiboot header. */
multiboot_header:
        /* magic */
        .long   MULTIBOOT_HEADER_MAGIC
        /* flags */
        .long   MULTIBOOT_HEADER_FLAGS
        /* checksum */
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
        /* header_addr */
        .long   multiboot_header
        /* load_addr */
        .long   multiboot_entry
        /* load_end_addr */
        .long   __link_load_end
        /* bss_end_addr */
        .long   __link_bss_end
        /* entry_addr */
        .long   multiboot_entry

multiboot_entry:

/* We dont trust segment descriptors from GRUB and set our ones */

        lgdt    gdt_32_ptr
        ljmp    $0x08, $gdt_32_ready

gdt_32_ready:

        movl    $0x10, %eax
        mov     %ax, %ds
        mov     %ax, %es
        mov     %ax, %ss

/* Setup page tables: identity map for first 1Gb 
   and map 1Gb from 0xFFFFFFFF80000000 to the first 1 Gb */
        
        movl    $page_dir_ptr, %eax
        orl     $3, %eax
        movl    %eax, page_ml4              /* page_ml4[0] = page_dir_ptr */
        movl    %eax, page_ml4 + 0xff8      /* page_ml4[511] = page_dir_ptr */

        movl    $page_dir, %eax
        orl     $3, %eax
        movl    %eax, page_dir_ptr          /* page_dir_ptr[0] = page_dir */
        movl    %eax, page_dir_ptr + 0xff0  /* page_dir_ptr[510] = page_dir */

        movl    $512, %ecx
        movl    $page_dir, %esi
        movl    $0x000083, %eax             /* PAGE_PRESENT | PAGE_WRITE | PAGE_1MB */
fill_page_dir_loop:
        movl    %eax, (%esi)
        addl    $0x200000, %eax
        addl    $8, %esi
        loop    fill_page_dir_loop

page_tables_ready:

/* Set address of our PML4 in cr3 */
        
        movl    $page_ml4, %eax
        movl    %eax, %cr3

/* Enable PAE */
        
        movl    %cr4, %eax
        orl     $(1<<5), %eax
        movl    %eax, %cr4

/* Enable long mode */

        movl    $0xC0000080, %ecx
        rdmsr
        orl     $(1<<8), %eax
        wrmsr

/* Enable paging */

        movl    %cr0, %eax
        orl     $(1<<31), %eax
        movl    %eax, %cr0

/* Now we are in 64-bit compatibility mode, to enter "normal" long mode 
   proper gdt should be loaded. Since we are not in 64-bit memory yet,
   physical address of gdt_64 is used. */

        lgdt    gdt_64_ptr_compatibility_mode
        ljmp    $0x08, $code_64

/* 64-bit code area */

        .code64

code_64:
        // Load same GDT but using address in higher-half memory 
        // so lower memory can be unmapped later
        lgdt    gdt_64_ptr

        movl    $0x10, %eax
        movl    %eax, %ds
        movl    %eax, %es
        movl    %eax, %ss
        movl    %eax, %fs
        movl    %eax, %gs

/* Initialize the kernel stack */

        movq    $(kernel_stack + KERNEL_STACK_SIZE), %rsp

/* Call kernel entry point located in the higher half memory */

        call    kmain

halt:   hlt
        jmp halt

/* Global descriptor tables */

        .align 8

gdt_32: /* GDT used in 32-bit protected mode immediately after multibooting */
        .quad   0x0000000000000000
        .quad   0x00CF9A000000FFFF
        .quad   0x00CF92000000FFFF

gdt_32_ptr:
        .word   23
        .long   gdt_32

gdt_64_ptr_compatibility_mode:
        .word   23
        .long   PHYS_ADDR(gdt_64)

        .align 4096

/* Paging structures used to establish initial memory mapping before the kernel is started */

page_ml4:
        .fill   0x1000, 8, 0
page_dir_ptr:
        .fill   0x1000, 8, 0
page_dir:
        .fill   0x1000, 8, 0

        .data

        .align 8

gdt_64:
        .quad   0x0000000000000000
        .quad   0x00A09A0000000000
        .quad   0x00A0920000000000

gdt_64_ptr:
        .word   23
        .quad   gdt_64

#define ASM_FILE 1

#include <multiboot.h>

#define MULTIBOOT_HEADER_FLAGS	0x00010003

#define STACK_SIZE	0x4000

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
        .long   _load_end
        /* bss_end_addr */
        .long   _bss_end
        /* entry_addr */
        .long   multiboot_entry

multiboot_entry:
		/* Initialize the stack pointer. */
        movl     $(stack + STACK_SIZE), %esp

halt:	hlt
		jmp halt

		.comm   stack, STACK_SIZE

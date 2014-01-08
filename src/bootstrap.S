#define ASM_FILE 1

#include <multiboot.h>

#define MULTIBOOT_HEADER_FLAGS	0x00000003

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
 #ifndef __ELF__
         /* header_addr */
         .long   multiboot_header
         /* load_addr */
         .long   entry
         /* load_end_addr */
         .long   0
         /* bss_end_addr */
         .long   _end_bss
         /* entry_addr */
         .long   multiboot_entry
 #endif /* ! __ELF__ */

multiboot_entry:
		/* Initialize the stack pointer. */
        movl     $(stack + STACK_SIZE), %esp

        //call kmain

halt:	hlt
		jmp halt

		.comm   stack, STACK_SIZE

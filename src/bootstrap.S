#define STACK_SIZE	0x4000

		.text

		.globl entry

entry:
		/* Initialize the stack pointer. */
        mov     $(stack + STACK_SIZE), %rsp

        call kmain
        
halt:	hlt
		jmp halt

		.comm   stack, STACK_SIZE

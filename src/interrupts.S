#define ASM_FILE 1

		.text
		.code64

		.globl trap_entry
		.globl intr_entry

trap_entry:
		iretq

intr_entry:
		call	intr_handler
		iretq		

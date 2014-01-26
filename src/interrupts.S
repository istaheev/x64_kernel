#define ASM_FILE 1

#include <kernel.h>

#define DECLARE_INTR_HANDLER_ENTRY(name) \
		.globl	name                   ; \
        .type   name, @function        ; \
name:                

#define SAVE_REGS                            \
		movq	%rax, TRAP_REGS_RAX(%rsp)  ; \
		movq	%rbx, TRAP_REGS_RBX(%rsp)  ; \
		movq	%rcx, TRAP_REGS_RCX(%rsp)  ; \
		movq	%rdx, TRAP_REGS_RDX(%rsp)  ; \
		movq	%rdi, TRAP_REGS_RDI(%rsp)  ; \
		movq	%rsi, TRAP_REGS_RSI(%rsp)  ; \
		movq	%r8,  TRAP_REGS_R8(%rsp)   ; \
		movq	%r9,  TRAP_REGS_R9(%rsp)   ; \
		movq	%r10, TRAP_REGS_R10(%rsp)  ; \
		movq	%r11, TRAP_REGS_R11(%rsp)  ; \
		movq	%r12, TRAP_REGS_R12(%rsp)  ; \
		movq	%r13, TRAP_REGS_R13(%rsp)  ; \
		movq	%r14, TRAP_REGS_R14(%rsp)  ; \
		movq	%r15, TRAP_REGS_R15(%rsp)  ; \
		movq	%rbp, TRAP_REGS_RBP(%rsp)  ; \
        movq    %ds, %rax                  ; \
		movq	%rax, TRAP_REGS_DS(%rsp)   ; \
        movq    %es, %rax                  ; \
		movq	%rax, TRAP_REGS_ES(%rsp)   ; \
        movq    %fs, %rax                  ; \
		movq	%rax, TRAP_REGS_FS(%rsp)   ; \
        movq    %gs, %rax                  ; \
		movq	%rax, TRAP_REGS_GS(%rsp)   ;

#define RESTORE_REGS                         \
        movq    TRAP_REGS_DS(%rsp), %rax   ; \
        movq    %rax, %ds                  ; \
        movq    TRAP_REGS_ES(%rsp), %rax   ; \
        movq    %rax, %es                  ; \
        movq    TRAP_REGS_FS(%rsp), %rax   ; \
        movq    %rax, %fs                  ; \
        movq    TRAP_REGS_GS(%rsp), %rax   ; \
        movq    %rax, %gs                  ; \
        movq    TRAP_REGS_RAX(%rsp), %rax  ; \
        movq    TRAP_REGS_RBX(%rsp), %rbx  ; \
        movq    TRAP_REGS_RCX(%rsp), %rcx  ; \
        movq    TRAP_REGS_RDX(%rsp), %rdx  ; \
        movq    TRAP_REGS_RDI(%rsp), %rdi  ; \
        movq    TRAP_REGS_RSI(%rsp), %rsi  ; \
        movq    TRAP_REGS_R8(%rsp),  %r8   ; \
        movq    TRAP_REGS_R9(%rsp),  %r9   ; \
        movq    TRAP_REGS_R10(%rsp), %r10  ; \
        movq    TRAP_REGS_R11(%rsp), %r11  ; \
        movq    TRAP_REGS_R12(%rsp), %r12  ; \
        movq    TRAP_REGS_R13(%rsp), %r13  ; \
        movq    TRAP_REGS_R14(%rsp), %r14  ; \
        movq    TRAP_REGS_R15(%rsp), %r15  ; \
        movq    TRAP_REGS_RBP(%rsp), %rbp  ;

#define TRAP_NO_ERROR_CODE(trap_no, handler)        \
		subq	$TRAP_REGS_RIP, %rsp              ; \
		SAVE_REGS                                 ; \
        movq    $trap_no, TRAP_REGS_TRAP_NO(%rsp) ; \
        movq    $0, TRAP_REGS_ERROR_CODE(%rsp)    ; \
		movq	%rsp, %rdi                        ; \
		call	handler                           ; \
        RESTORE_REGS                              ; \
		addq	$TRAP_REGS_RIP, %rsp              ; \
		iretq                                     ;

#define TRAP_WITH_ERROR_CODE(trap_no, handler)      \
        subq    $TRAP_REGS_ERROR_CODE, %rsp       ; \
        SAVE_REGS                                 ; \
        movq    $trap_no, TRAP_REGS_TRAP_NO(%rsp) ; \
        movq    %rsp, %rdi                        ; \
        call    handler                           ; \
        RESTORE_REGS                              ; \
        addq    $TRAP_REGS_ERROR_CODE, %rsp       ; \
        iretq                                     ;

		.text
		.code64

DECLARE_INTR_HANDLER_ENTRY ( trap_0 )
		TRAP_NO_ERROR_CODE ( 0, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_1 )
		TRAP_NO_ERROR_CODE ( 1, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_2 )
        TRAP_NO_ERROR_CODE ( 2, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_3 )
        TRAP_NO_ERROR_CODE ( 3, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_4 )
        TRAP_NO_ERROR_CODE ( 4, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_5 )
        TRAP_NO_ERROR_CODE ( 5, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_6 )
        TRAP_NO_ERROR_CODE ( 6, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_7 )
        TRAP_NO_ERROR_CODE ( 7, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_8 )
        TRAP_WITH_ERROR_CODE ( 8, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_9 )
        TRAP_NO_ERROR_CODE ( 9, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_10 )
        TRAP_WITH_ERROR_CODE ( 10, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_11 )
        TRAP_WITH_ERROR_CODE ( 11, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_12 )
        TRAP_WITH_ERROR_CODE ( 12, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_13 )
        TRAP_WITH_ERROR_CODE ( 13, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_14 )
        TRAP_WITH_ERROR_CODE ( 14, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_15 )
        TRAP_NO_ERROR_CODE ( 15, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_16 )
        TRAP_NO_ERROR_CODE ( 16, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_17 )
        TRAP_WITH_ERROR_CODE ( 17, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_18 )
        TRAP_NO_ERROR_CODE ( 18, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_19 )
        TRAP_NO_ERROR_CODE ( 19, trap_handler )
DECLARE_INTR_HANDLER_ENTRY ( trap_20 )
        TRAP_NO_ERROR_CODE ( 20, trap_handler )

DECLARE_INTR_HANDLER_ENTRY ( interrupt_entry )
		TRAP_NO_ERROR_CODE ( 0x00, interrupt_handler )

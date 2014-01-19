#ifndef __KERNEL_H__
#define __KERNEL_H__ 1

// The kernel is linked to this virtual address.
// linker.ld should be updated accordingly if changed.
#define KERNEL_VMA 0xFFFFFFFF80000000

// Size of the kernel stack
#define KERNEL_STACK_SIZE 0x2000

// Returns physical address of a pointer located in the higher half
#define PHYS_ADDR(x) (x - KERNEL_VMA)

#ifndef ASM_FILE

extern char __link_kernel_begin_vaddr[];
extern char __link_kernel_end_vaddr[];

void kprint(const char *, ...);

#endif // ASM_FILE

#endif // __KERNEL_H__

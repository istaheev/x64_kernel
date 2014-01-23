#ifndef __KERNEL_H__
#define __KERNEL_H__ 1

// The kernel is linked to this virtual address.
// linker.ld should be updated accordingly if changed.
#define KERNEL_VMA 0xFFFFFFFF80000000

// Size of the kernel stack
#define KERNEL_STACK_SIZE 0x2000

#define GDT_ENTRIES_COUNT 3

#define IDT_ENTRIES_COUNT 0x30

// Returns physical address of a pointer located in the higher half
#define PHYS_ADDR(x) (x - KERNEL_VMA)

#ifndef ASM_FILE

#include <stdint.h>

// Symbols defined by linker script
extern char __link_kernel_begin_vaddr[];
extern char __link_kernel_end_vaddr[];

// Parts of segment_descriptor.seg_type
#define SEG_TYPE_DATA               0
#define SEG_TYPE_CODE               (1<<3)

#define SEG_TYPE_CODE_NON_CONFORMING    0
#define SEG_TYPE_CODE_CONFORMING        (1<<2)

#define SEG_TYPE_CODE_EXECUTE_ONLY  0
#define SEG_TYPE_CODE_READ_EXECUTE  (1<<1)

#define SEG_TYPE_DATA_EXPAND_UP     0
#define SEG_TYPE_DATA_EXPAND_DOWN   (1<<2)

#define SEG_TYPE_DATA_READ_ONLY     0
#define SEG_TYPE_DATA_READ_WRITE    (1<<1)

#define SEG_TYPE_NON_ACCESSED       0
#define SEG_TYPE_ACCESSED           (1<<0)

// Values for segment_descriptor.desc_type
#define DESC_TYPE_SYSTEM    0x00
#define DESC_TYPE_CODE_DATA 0x01

// segment_descriptor.op_size
#define SEG_OP_SIZE_16      0x00
#define SEG_OP_SIZE_32      0x01

// segment_descriptor.code_size
#define SEG_CODE_32         0x00
#define SEG_CODE_64         0x01

// segment_descriptor.granularity
#define SEG_GRAN_1B         0x00 
#define SEG_GRAN_4KB        0x01

struct segment_descriptor
{
    unsigned int limit_low  : 16;
    unsigned int base_low   : 24;
    unsigned int seg_type    : 4;
    unsigned int desc_type   : 1;
    unsigned int dpl         : 2;
    unsigned int present     : 1;
    unsigned int limit_high  : 4;
    unsigned int available   : 1;
    unsigned int code_size   : 1;
    unsigned int op_size     : 1;
    unsigned int granularity : 1;
    unsigned int base_high   : 8;
} __attribute__((packed));

typedef struct segment_descriptor segment_descriptor_t;

struct segment_descriptor_entry
{
    union
    {
        segment_descriptor_t desc;
        uint64_t value;
    } u;
};

typedef struct segment_descriptor_entry segment_descriptor_entry_t;

#define IDT_INTR_GATE 0x0E
#define IDT_TRAP_GATE 0x0F

struct idt_descriptor
{
    unsigned int offset_0_15  : 16;
    unsigned int seg_selector : 16;
    unsigned int stack_table  : 3;
    unsigned int _zero1       : 1;
    unsigned int _zero2       : 1;
    unsigned int _zero3       : 3;
    unsigned int type         : 4;
    unsigned int _zero4       : 1;
    unsigned int dpl          : 2;
    unsigned int present      : 1;
    unsigned int offset_16_31 : 16;
    unsigned int offset_32_63 : 32;
    unsigned int reserved     : 32;
} __attribute__((packed));

typedef struct idt_descriptor idt_descriptor_t;

void kprint(const char *, ...);

#endif // ASM_FILE

#endif // __KERNEL_H__

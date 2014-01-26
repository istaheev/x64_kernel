#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm.h>
#include <kernel.h>
#include <multiboot.h>
#include <stdlib.h>
#include <video.h>

// Kernel stack, initialized in bootstrap.S before call to kmain
char kernel_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned (4096))) = {0};

// GDT used in 64-bit mode:
// 0x00 - null
// 0x08 - code
// 0x10 - data
segment_descriptor_entry_t gdt[GDT_ENTRIES_COUNT] __attribute__ ((aligned (8))) = 
    { /* 0x00: NULL */
      {.u.value = 0x0} 
      /* 0x08: code */
    , {.u.desc = { .limit_low   = 0x0
                 , .base_low    = 0x0
                 , .seg_type    = SEG_TYPE_CODE | SEG_TYPE_CODE_READ_EXECUTE
                 , .desc_type   = DESC_TYPE_CODE_DATA
                 , .dpl         = 0
                 , .present     = 0x1
                 , .limit_high  = 0x0
                 , .available   = 0x0
                 , .code_size   = SEG_CODE_64
                 , .op_size     = SEG_OP_SIZE_16
                 , .granularity = SEG_GRAN_4KB
                 , .base_high   = 0x0
             }}
      /* 0x10: data */
    , {.u.desc = { .limit_low   = 0x0
                 , .base_low    = 0x0
                 , .seg_type    = SEG_TYPE_DATA | SEG_TYPE_DATA_READ_WRITE
                 , .desc_type   = DESC_TYPE_CODE_DATA
                 , .dpl         = 0
                 , .present     = 0x1
                 , .limit_high  = 0x0
                 , .available   = 0x0
                 , .code_size   = SEG_CODE_64
                 , .op_size     = SEG_OP_SIZE_16
                 , .granularity = SEG_GRAN_4KB
                 , .base_high   = 0x0
             }}
    };

idt_descriptor_t idt[IDT_ENTRIES_COUNT];

static 
void gdt_init ()
{
    load_gdt ( GDT_ENTRIES_COUNT, &gdt );
}

__attribute__((unused))
static 
void display_kprint_test ()
{
    unsigned int t = 0xf0000000;
    kprint ( "sizeof(unsigned int) = %u\n", sizeof(unsigned int) );
    kprint ( "sizeof(unsigned long) = %u\n", sizeof(unsigned long) );
    kprint ( "sizeof(unsigned long long) = %u\n", sizeof(unsigned long long) );
    kprint ( "sizeof(void*) = %u\n", sizeof(void*) );
    kprint ( "sizeof(intptr_t) = %u\n", sizeof(intptr_t) );
    kprint ( "\n" );

    kprint ( "0xffffffff = %u\n", t );
    kprint ( "0xffffffff = 0x%lx\n", (unsigned long)0xfffffffe );
    kprint ( "0xffffffff = 0x%llx\n", (unsigned long long)0xfffffffe );
    kprint ( "0xffffffffffffffff = 0x%x\n", (unsigned int)0xffffffffffffffff );
    kprint ( "0xffffffffffffffff = 0x%lx\n", (unsigned long)0xffffffffffffffff );
    kprint ( "0xffffffffffffffff = 0x%llx\n", (unsigned long long)0xffffffffffffffff );
    kprint ( "\n" );
}

static
void display_kernel_info ()
{
    kprint ( "Kernel placement:\n" );
    kprint ( "Physical: 0x%llx - 0x%llx (%lu bytes)\n", 
        PHYS_ADDR(__link_kernel_begin_vaddr), 
        PHYS_ADDR(__link_kernel_end_vaddr),
        __link_kernel_end_vaddr - __link_kernel_begin_vaddr );
    kprint ( "Virtual : 0x%016lx - 0x%016lx\n\n", 
        __link_kernel_begin_vaddr, 
        __link_kernel_end_vaddr );
}

static
void display_mbi ( const multiboot_info_t* mbi )
{
    struct multiboot_mmap_entry* mmap_ptr;

    uint64_t mmap_addr = (uint64_t) mbi->mmap_addr;
    uint64_t mmap_length = (uint64_t) mbi->mmap_length;

    kprint ( "Lower memory: %lu bytes\n", (uint64_t) mbi->mem_lower * 1024 );
    kprint ( "Upper memory: %lu bytes\n", (uint64_t) mbi->mem_upper * 1024 );

    kprint ( "\nMemory map (GRUB version):\n" );

    for( mmap_ptr = (struct multiboot_mmap_entry*) mmap_addr; 
         (multiboot_uint64_t) mmap_ptr < mmap_addr + mmap_length; 
         mmap_ptr = (struct multiboot_mmap_entry*) (
                        (multiboot_uint64_t) mmap_ptr +
                        mmap_ptr->size +
                        sizeof(mmap_ptr->size)) ) 
    {
        kprint ( "%s: %016llx - %016llx (%lu bytes)\n",
                 mmap_ptr->type == MULTIBOOT_MEMORY_AVAILABLE ? "AVAILABLE" : " RESERVED",
                 mmap_ptr->addr,
                 mmap_ptr->addr + mmap_ptr->len - 1,
                 mmap_ptr->len);
    }
}

static
void pic_end_of_interrupt ( bool slave )
{
    if ( slave )
        outportb ( 0xa0, 0x20 );
    outportb ( 0x20, 0x20 );
}

static
void pic_init ( uint8_t irq0_start, uint8_t irq8_start )
{
    outportb ( 0x20, 0x11 );
    outportb ( 0xa0, 0x11 );

    // map irq numbers
    outportb ( 0x21, irq0_start );
    outportb ( 0xa1, irq8_start );

    // setup master/slave wiring
    outportb ( 0x21, 0x04 );
    outportb ( 0xa1, 0x02 );

    outportb ( 0x21, 0x01 );
    outportb ( 0xa1, 0x01 );
}

static
void pic_mask ( uint16_t mask )
{
    outportb ( 0x21, 0x01 );
    outportb ( 0xa1, 0x01 );

    outportb ( 0x21, mask & 0xff );
    outportb ( 0xa1, (mask >> 8) & 0xff );
}

static 
void idt_entry_set ( idt_descriptor_t * entry, uintptr_t handlerp, unsigned int type, unsigned int selector, unsigned int dpl )
{
    uintptr_t handler_uintptr = (uintptr_t) handlerp;

    entry->offset_0_15  = handler_uintptr & 0xFFFF;
    entry->offset_16_31 = (handler_uintptr >> 16) & 0xFFFF;
    entry->offset_32_63 = (handler_uintptr >> 32) & 0xFFFFFFFF;
    entry->seg_selector = selector;
    entry->type         = type;
    entry->dpl          = dpl;
    entry->present      = 1;
}

static
void idt_init ()
{
    memset ( idt, 0, sizeof(idt) );

    idt_entry_set ( &idt[0],  (uintptr_t)trap_0,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[1],  (uintptr_t)trap_1,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[2],  (uintptr_t)trap_2,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[3],  (uintptr_t)trap_3,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[4],  (uintptr_t)trap_4,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[5],  (uintptr_t)trap_5,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[6],  (uintptr_t)trap_6,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[7],  (uintptr_t)trap_7,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[8],  (uintptr_t)trap_8,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[9],  (uintptr_t)trap_9,  IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[10], (uintptr_t)trap_10, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[11], (uintptr_t)trap_11, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[12], (uintptr_t)trap_12, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[13], (uintptr_t)trap_13, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[14], (uintptr_t)trap_14, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[15], (uintptr_t)trap_15, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[16], (uintptr_t)trap_16, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[17], (uintptr_t)trap_17, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[18], (uintptr_t)trap_18, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[19], (uintptr_t)trap_19, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[20], (uintptr_t)trap_20, IDT_INTR_GATE, 0x08, 0x0 );

    idt_entry_set ( &idt[0x20], (uintptr_t)interrupt_entry, IDT_INTR_GATE, 0x08, 0x0 );
    idt_entry_set ( &idt[0x21], (uintptr_t)interrupt_entry, IDT_INTR_GATE, 0x08, 0x0 );

    load_idt ( IDT_ENTRIES_COUNT, idt );
}

// called from interrupt.S
void trap_handler ( trap_regs_t * regs )
{
    kprint ( "Trap handler\n" );
    kprint ( "Exception: %d\n", regs->trap_no );
    kprint ( "CS:RIP = %03llx:%016llx\n", regs->cs, regs->rip );
    kprint ( "SS:RSP = %03llx:%016llx\n", regs->ss, regs->rsp );
    kprint ( "RFLAGS = %016llx\n", regs->rflags );
    kprint ( "ERROR  = %lld\n", regs->error_code );
    halt();
}

static
uint16_t pic_get_irr ( void )
{
    outportb ( 0x20, 0x0a );
    outportb ( 0xa0, 0x0a );
    return ((uint16_t)inportb ( 0xa0 ) << 8) | inportb ( 0x20 );
}

static
uint16_t pic_get_isr ( void )
{
    outportb ( 0x20, 0x0b );
    outportb ( 0xa0, 0x0b );
    return ((uint16_t)inportb ( 0xa0 ) << 8) | inportb ( 0x20 );
}

// called from interrupt.S
void interrupt_handler ( trap_regs_t * regs __attribute__((unused)) )
{
    uint16_t isr = pic_get_isr ();

    if ( 2 == isr )
    {
        inportb ( 0x60 );
        kprint ( "|" );
    }
    else
    {
        kprint ( "." );
    }

    pic_end_of_interrupt ( false );
}

static
void khalt ()
{
    kprint ( "Kernel halted." );
    while (1)
    {
        halt ();
    }
}

__attribute__((unused))
static
void simulate_page_fault ( void )
{
    char * p = (char *)KERNEL_VMA;
    *(p-0x1000) = 1;
}

/* Entry point for kernel, called from bootstrap.S 
   after preparing of the environment.

   Machine state at this stage:
   1. CPU is in the long mode
   2. Interrupts are disabled, IDT is not set up
   3. Paging is set up so the first 1Gb of of memory is identity
      mapped to itself and is also available from 0xFFFFFFFF80000000
*/      
void kmain ( const multiboot_info_t* mbi )
{
    // reload GDT using its virtual address so that lower memory can be unmapped
    gdt_init ();

    // initialize video subsystem, so kprint can work (don't call kprint before!)
    video_init ();

    idt_init ();
    pic_init ( 0x20, 0x28 );
    pic_mask ( 0xfffc ); // enable keyboard
    sti ();

    // display_kprint_test ();
    display_kernel_info ();
    display_mbi ( mbi );

    // simulate_page_fault ();
    khalt ();
}


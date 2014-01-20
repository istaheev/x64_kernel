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
    , {.u.desc = { .limit_low       = 0x0
                 , .base_low        = 0x0
                 , .seg_type        = SEG_TYPE_CODE | SEG_TYPE_CODE_READ_EXECUTE
                 , .desc_type       = DESC_TYPE_CODE_DATA
                 , .privilege_level = 0
                 , .present         = 0x1
                 , .limit_high      = 0x0
                 , .available       = 0x0
                 , .code_size       = SEG_CODE_64
                 , .op_size         = SEG_OP_SIZE_16
                 , .granularity     = SEG_GRAN_4KB
                 , .base_high       = 0x0
             }}
      /* 0x10: data */
    , {.u.desc = { .limit_low       = 0x0
                 , .base_low        = 0x0
                 , .seg_type        = SEG_TYPE_DATA | SEG_TYPE_DATA_READ_WRITE
                 , .desc_type       = DESC_TYPE_CODE_DATA
                 , .privilege_level = 0
                 , .present         = 0x1
                 , .limit_high      = 0x0
                 , .available       = 0x0
                 , .code_size       = SEG_CODE_64
                 , .op_size         = SEG_OP_SIZE_16
                 , .granularity     = SEG_GRAN_4KB
                 , .base_high       = 0x0
             }}
    };

static 
void gdt_init ()
{
    load_gdt ( GDT_ENTRIES_COUNT, &gdt );
}

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

    video_init ();

    //display_kprint_test ();
    display_kernel_info ();
    display_mbi ( mbi );

    while(1)
    {
        halt ();
    }
}


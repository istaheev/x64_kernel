#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm.h>
#include <kernel.h>
#include <multiboot.h>
#include <stdlib.h>
#include <video.h>

/*** Kernel data area begin ***/

// Kernel stack, initialized in bootstrap.S before call to kmain
char kernel_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned (4096))) = {0};

/*** Kernel data area end ***/

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

#define KPRINT_BUF_SIZE 256

char kprint_buf[KPRINT_BUF_SIZE];

// kprint is not reentrant now (static buffer is used)
void kprint(const char * format, ...)
{
    va_list args;
    int r;

    va_start( args, format );
    r = vsprintf( kprint_buf, format, args );
    va_end( args );

    if ( r > KPRINT_BUF_SIZE-1 )
    	video_putstr ( "POSSIBLE KPRINT BUFFER OVERFLOW!!" );
    else
    	video_putstr ( kprint_buf );
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
	//unsigned int t = 0xf0000000;
	video_init ();

//	kprint ( "sizeof(unsigned int) = %u\n", sizeof(unsigned int) );
//	kprint ( "sizeof(unsigned long) = %u\n", sizeof(unsigned long) );
//	kprint ( "sizeof(unsigned long long) = %u\n", sizeof(unsigned long long) );
//	kprint ( "sizeof(void*) = %u\n", sizeof(void*) );
//	kprint ( "sizeof(intptr_t) = %u\n", sizeof(intptr_t) );

//  sprintf tests:
//	kprint ( "\n" );
//	kprint ( "0xffffffff = %u\n", t );
//	kprint ( "0xffffffff = 0x%lx\n", (unsigned long)0xfffffffe );
//	kprint ( "0xffffffff = 0x%llx\n", (unsigned long long)0xfffffffe );
//	kprint ( "0xffffffffffffffff = 0x%x\n", (unsigned int)0xffffffffffffffff );
//	kprint ( "0xffffffffffffffff = 0x%lx\n", (unsigned long)0xffffffffffffffff );
//	kprint ( "0xffffffffffffffff = 0x%llx\n", (unsigned long long)0xffffffffffffffff );

	display_kernel_info ();
	display_mbi ( mbi );

	while(1)
	{
		halt ();
	}
}


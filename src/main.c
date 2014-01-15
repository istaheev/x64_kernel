#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm.h>
#include <kernel.h>
#include <stdlib.h>
#include <video.h>

/*** Kernel data area begin ***/

// Kernel stack, initialized in bootstrap.S before call to kmain
char kernel_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned (4096))) = {0};

/*** Kernel data area end ***/

/* Entry point for kernel, called from bootstrap.S 
   after preparing of the environment.

   Machine state at this stage:
   1. CPU is in the long mode
   2. Interrupts are disabled, IDT is not set up
   3. Paging is set up so the first 1Gb of of memory is identity
      mapped to itself and is also available from 0xFFFFFFFF80000000
*/      
void kmain (void)
{
	int i;
	char num[10];

	video_init ();

	for ( i = 0; i < 30; i++ )
	{
		video_putstr ( "Line: " );
		video_putstr ( itoa( i + 1, num, 10 ) );
		video_putstr ( "\n" );
	}

	while(1)
	{
		halt ();
	}
}

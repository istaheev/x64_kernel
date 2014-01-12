#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static char* videomem = (char*)0xB8000;
static size_t screen_x = 0, screen_y = 0;

void putchar(char c)
{
	videomem[screen_y * 160 + screen_x*2] = c;
	screen_x++;
	if (screen_x >= 80)
	{
		screen_x = 0;
		screen_y++;
	}
}

void print(const char* str)
{
	const char* s;
	for(s = str; *s; s++)
		putchar(*s);
}

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
	char* p = (char*)0xB8000;
	*p = '!';

	print("Hello, World!");

    while(1) {}
}

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

void kmain (void)
{
	char* p = (char*)0xB8000;
	*p = '!';

	print("Hello, World!");

    while(1) {}
}

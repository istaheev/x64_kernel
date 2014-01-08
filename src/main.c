static char* videomem = (char*)0xB8000;
static int screen_x = 0, screen_y = 0;

void putchar(char c)
{
	*(videomem + screen_y * 160 + screen_x*2 + 1) = c;
	if( screen_x >= 80)
	{
		screen_x = 0;
		screen_y++;
	}
}

void print(const char* str)
{
	const char* s = str;
	for(; *s; s++)
		putchar(*s);
}

void kmain (void)
{
	//print("Hello, World!");

    while(1) {}
}

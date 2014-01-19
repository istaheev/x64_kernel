#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm.h>
#include <stdlib.h>
#include <video.h>

typedef uint16_t video_cell_t;

static video_cell_t* video_memory;
static uint16_t cursor_x, cursor_y;
static uint8_t current_attr;

#define SCREEN_W 80
#define SCREEN_H 25

#define LINEAR_CURSOR_POS ( cursor_y * 80 + cursor_x )

#define CHAR_REPR( c, attr ) ( ((video_cell_t)attr) << 8 | (c & 0xFF))

static 
void video_clear_region ( int start_pos, int chars_count )
{
	video_cell_t * ptr;

	if ( start_pos < 0 )
		return;

	if ( start_pos + chars_count > SCREEN_W * SCREEN_H )
		return;

	ptr = video_memory + start_pos;

	while ( chars_count > 0 )
	{
		*ptr = CHAR_REPR(0x20, 0x07);
		ptr++;
		chars_count--;
	}
}

static 
void video_scroll_up ( uint16_t lines )
{
	uint16_t offset = lines * SCREEN_W;
	uint16_t lines_to_move = max( SCREEN_H - lines, 0 );
	memmove ( video_memory, video_memory + offset, lines_to_move * SCREEN_W * 2 );
	video_clear_region ( lines_to_move * SCREEN_W, offset );
}

static
void video_place_cursor_on_screen ( void )
{
	uint16_t position = LINEAR_CURSOR_POS;
	outportb ( 0x3D4, 0x0F );
	outportb ( 0x3D5, position & 0xFF );
	outportb ( 0x3D4, 0x0E );
	outportb ( 0x3D5, (position >> 8) & 0xFF );
}

static
void video_carriage_return ( void )
{
	cursor_x = 0;
}

static
void video_line_feed ( void )
{
	if ( cursor_y >= (SCREEN_H - 1) )
	{
		video_scroll_up ( 1 );
	}
	else
	{
		cursor_y++;
	}
}

static 
void video_advance_cursor ( void )
{
	cursor_x++;
	if ( cursor_x >= (SCREEN_W - 1) )
	{
		video_carriage_return ();
		video_line_feed ();
	}
}

void video_init ( void )
{
	video_memory = (video_cell_t*)0xB8000;
	cursor_x = cursor_y = 0;
	current_attr = 0x07;
	video_clear_screen ();
	video_place_cursor_on_screen ();
}

void video_clear_screen ( void )
{
	video_clear_region ( 0, SCREEN_W * SCREEN_H );
	cursor_x = cursor_y = 0;
	video_place_cursor_on_screen();
}

void video_putchar ( char c )
{
	uint16_t offset = LINEAR_CURSOR_POS;
	video_memory[ offset ] = CHAR_REPR( c, current_attr );
	video_advance_cursor ();
	video_place_cursor_on_screen ();
}

void video_putstr ( const char* str )
{
	const char* s;

	uint16_t offset = LINEAR_CURSOR_POS;

	for ( s = str; *s; s++ )
	{
		switch(*s)
		{
		case '\n':
			video_carriage_return ();
			video_line_feed ();
			offset = LINEAR_CURSOR_POS;
			break;

		default:
			video_memory[ offset ] = CHAR_REPR( *s, current_attr );
			offset++;
			video_advance_cursor ();
		}
	}

	video_place_cursor_on_screen ();
}
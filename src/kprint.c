#include <stdarg.h>
#include <stddef.h>

#include <kernel.h>
#include <stdlib.h>
#include <video.h>

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

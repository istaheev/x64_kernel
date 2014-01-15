#ifndef __STDLIB_H__
#define __STDLIB_H__ 1

#include <stdint.h>

#define min( a, b ) ( b < a ? b : a )

#define max( a, b ) ( b > a ? b : a )

void* memset8 ( void*, uint8_t, size_t );
void* memset16 ( void*, uint16_t, size_t );
void* memcpy ( void*, const void*, size_t );

char* itoa ( int, char*, int );

#endif
#ifndef __STDLIB_H__
#define __STDLIB_H__ 1

#include <stdint.h>
#include <stdarg.h>

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

typedef __builtin_va_list va_list;

#define min( a, b ) ( b < a ? b : a )

#define max( a, b ) ( b > a ? b : a )

void * memchr( const void *, int, size_t );
void * memcpy ( void*, const void*, size_t );
void * memmove( void *, const void *, size_t );
void * memset ( void*, int, size_t );

char * strchr( const char *, int );
size_t strlen( const char * );

char * itostr  ( int, char *, int );
char * uitostr ( unsigned int, char *, int );
char * ltostr  ( long, char *, int );
char * ultostr ( unsigned long, char *, int );

int sprintf ( char *, const char *, ... );
int vsprintf ( char *, const char *, va_list args );

#endif
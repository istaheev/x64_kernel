#ifndef __ASM_H__
#define __ASM_H__ 1

#include <stdint.h>

// C99 doesn't support asm
#define asm __asm__

static inline 
void outportb( uint16_t port, uint8_t value )
{
    asm volatile( "outb %0, %1"
                  : : "a"(value), "Nd"(port) );
}

static inline
uint8_t inportb( uint16_t port )
{
    uint8_t value;
    asm volatile( "inb %1, %0"
                  : "=a"(value) : "Nd"(port) );
    return value;
}

static inline
void halt ()
{
	asm volatile( "hlt" );
}

#endif
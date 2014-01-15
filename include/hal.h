#ifndef __HAL_H__
#define __HAL_H__ 1

#include <stdint.h>

static inline 
void hal_outb( uint16_t port, uint8_t value )
{
    asm volatile( "outb %0, %1"
                  : : "a"(value), "Nd"(port) );
}

static inline
uint8_t hal_inb( uint16_t port )
{
    uint8_t value;
    asm volatile( "inb %1, %0"
                  : "=a"(value) : "Nd"(port) );
    return value;
}

#endif
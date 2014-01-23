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
void load_gdt ( uint16_t num_entries, const void * gdt )
{
	struct gdt_ptr
	{
		uint16_t  gdt_size;
		uintptr_t gdt;
	} __attribute__((packed));

	struct gdt_ptr gdt_ptr = { num_entries * 8 - 1, (uint64_t)gdt };

    asm volatile( "lgdt (%0)"
                  : : "r"(&gdt_ptr) );
}

static inline
void load_idt ( uint16_t num_entries, const void * idt )
{
	struct idt_ptr
	{
		uint16_t  idt_size;
		uintptr_t idt;
	} __attribute__((packed));

	struct idt_ptr idt_ptr = { num_entries * 16 - 1, (uint64_t)idt };

    asm volatile( "lidt (%0)"
                  : : "r"(&idt_ptr) );
}

static inline
void cli ()
{
	asm volatile( "cli" );
}

static inline
void sti ()
{
	asm volatile( "sti" );
}

static inline
void halt ()
{
	asm volatile( "hlt" );
}

#endif
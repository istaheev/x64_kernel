OUTPUT_FORMAT("elf64-x86-64")
ENTRY(entry)

KERNEL_LMA = 0x100000;

SECTIONS
{
    . = KERNEL_LMA;

    .bootstrap :
    {
        obj/bootstrap.o(.text)
    }

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(.bss)
        *(COMMON)
        _end_bss = .;
    }
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
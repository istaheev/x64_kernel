OUTPUT_FORMAT("elf64-x86-64")
ENTRY(entry)

SECTIONS
{
    .grub_sig 0xFFFFFFFF80100000 : AT(0x100000)
    {
        *(.grub_sig)
    }
    .text :
    {
        obj/bootstrap.o(.text)
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
    }
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
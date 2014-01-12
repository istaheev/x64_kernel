OUTPUT_FORMAT("elf64-x86-64")
ENTRY(entry)

KERNEL_LMA = 0x100000;
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS
{
    . = KERNEL_LMA;

    .bootstrap :
    {
        obj/bootstrap.o(.text)
        obj/bootstrap.o(COMMON)
    
        /* placeholders for paging structures */
        
        /* . = ALIGN(0x1000); */
        
        /* _page_ml4 = .; */      /* Single PML4 table */
        /* . += 0x1000; */

        /* _page_dir_ptr = .; */  /* Single pade-directory-pointer table */
        /* . += 0x1000; */

        /* _page_dir = .; */      /* Single page-directory table */
        /* . += 0x1000; */

        /* _page_table = .; */    /* 512 page-table entries */
        /* . += 512*0x1000; */

        /* _page_data_size = . - _page_ml4; */
    }

    . += KERNEL_VMA;

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VMA)
    {
        *(.text)
        *(EXCLUDE_FILE(*obj/bootstrap.o) .text)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VMA)
    {
        *(.data)
    }

    __link_load_end = . - KERNEL_VMA;

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VMA)
    {
        *(.bss)
    }

    __link_bss_end = . - KERNEL_VMA;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
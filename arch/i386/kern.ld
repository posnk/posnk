/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(i386_start)

PHDRS
{
  text PT_LOAD FILEHDR PHDRS ;
  text2 PT_LOAD;
  rodata PT_LOAD ;
  data PT_LOAD ;
}

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;
	kern_elf_base = . + 0xC0000000;
	. += SIZEOF_HEADERS;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.setup BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.setup)
	} :text

	. += 0xC0000000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}:text2

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}:rodata

	.dynsym ALIGN(4K) : AT(ADDR(.dynsym) - 0xC0000000)
	{
		kern_symtab_base = .;
		KEEP(*(.symtab));
		KEEP(*(.dynsym));
		KEEP(*(.dynsym*));
		kern_symtab_end  = .;
	}:rodata

	.dynstr ALIGN(4K) : AT(ADDR(.dynstr) - 0xC0000000)
	{
		kern_symstr_base = .;
		KEEP(*(.strtab));
		KEEP(*(.dynstr));
		KEEP(*(.dynstr*));
		kern_symstr_end  = .;
	}:rodata

	/* Read-write data (initialized) */
	.data ALIGN (4096) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)

	}:data

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		i386_start_kheap = .;
	}:data

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}

#include "kernel/paging.h"
#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#define CON_SRC "multiboot"
#include "kernel/console.h"
#include "arch/i386/multiboot.h"
#include "kernel/elf.h"
#include "kdbg/dbgapi.h"
#include <string.h>

#define PHYS_TO_VIRTP( a ) ((void*)( a + 0xC0000000 ))

static uintptr_t mbelf_sym_start    = 0;
static size_t    mbelf_sym_size     = 0;
static int       mbelf_sym_count    = 0;
static uintptr_t mbelf_symstr_start = 0;
static size_t    mbelf_symstr_size  = 0;

/**
 * Reserves the physical frames containing the data for an ELF section
 */
static void mbelf_reserve_section( Elf32_Shdr *sec ) {
	printf( CON_DEBUG, "reserving ELF section %08x with size %x",
	                   sec->sh_addr, sec->sh_size );
	physmm_claim_range( (physaddr_t) sec->sh_addr,
	                    (physaddr_t) sec->sh_addr + sec->sh_size );
}

/**
 * Looks up a section of the multiboot kernel ELF by ID
 * @return A pointer to the section header if it was found, NULL otherwise
 */
static Elf32_Shdr *mbelf_get_section( multiboot_info_t *mbt, unsigned idx )
{
	Elf32_Shdr *first;

	first = PHYS_TO_VIRTP( mbt->u.elf_sec.addr );

	if ( idx >= mbt->u.elf_sec.num )
		return NULL;

	return elf_get_shdr( first, mbt->u.elf_sec.size, idx );

}

/**
 * Looks up a section of the multiboot kernel ELF by name
 * @return A pointer to the section header if it was found, NULL otherwise
 */
static Elf32_Shdr *mbelf_resolve_section( multiboot_info_t *mbt, const char *name )
{
	unsigned i;
	const char *strtab;
	Elf32_Shdr *strtab_sh;
	Elf32_Shdr *val;

	/* Get the string table containing the section names */
	strtab_sh = mbelf_get_section( mbt, mbt->u.elf_sec.shndx );
	strtab = PHYS_TO_VIRTP( strtab_sh->sh_addr );

	/* Iterate over the sections and compare the names */
	for ( i = 0; i < mbt->u.elf_sec.num; i++ ) {
		val = mbelf_get_section( mbt, i );
		if ( strcmp( strtab + val->sh_name, name ) == 0 )
			return val;
	}

	return NULL;

}

/**
 * Parse the ELF file passed by the bootloader and reserve the physical
 * memory it occupies.
 */
void mbelf_parse_reserve( multiboot_info_t* mbt )
{
	Elf32_Shdr *strtab_sh;
	Elf32_Shdr *symtab_sh;
	Elf32_Shdr *symstr_sh;

	printf( CON_INFO, "flags=0x%x",mbt->flags);

	if ( MULTIBOOT_INFO_ELFSYM & ~mbt->flags )
		return;

	printf( CON_INFO, "elf symbol header"
	                "num:%i size:%i addr:%x shndx:%x",
	                mbt->u.elf_sec.num,
	                mbt->u.elf_sec.size,
	                mbt->u.elf_sec.addr,
	                mbt->u.elf_sec.shndx);

	/* Reserve section headers */
	physmm_claim_range( (physaddr_t) mbt->u.elf_sec.addr,
	                    (physaddr_t) mbt->u.elf_sec.addr +
	                                 mbt->u.elf_sec.size *
	                                 mbt->u.elf_sec.num );

	/* Resolve section name string table */
	strtab_sh = mbelf_get_section( mbt, mbt->u.elf_sec.shndx );

	/* Reserve section name string table */
	mbelf_reserve_section( strtab_sh );

	/* Resolve symbol section */
	symtab_sh = mbelf_resolve_section( mbt, ".symtab" );
	if ( !symtab_sh ) {
		printf( CON_WARN,
		        "elf header but no .symtab section" );
		return;
	}

	/* Reserve symbol section */
	mbelf_reserve_section( symtab_sh );

	mbelf_sym_start = symtab_sh->sh_addr;
	mbelf_sym_size  = symtab_sh->sh_size;
	mbelf_sym_count = symtab_sh->sh_size / symtab_sh->sh_entsize;

	/* Resolve symbol strings section */
	symstr_sh = mbelf_get_section( mbt, symtab_sh->sh_link );
	if ( !symstr_sh ) {
		printf( CON_WARN,
		        "could not find symbol table strings" );
		return;
	}

	/* Reserve symbol strings section */
	mbelf_reserve_section( symstr_sh );

	mbelf_symstr_start = symstr_sh->sh_addr;
	mbelf_symstr_size  = symstr_sh->sh_size;

	dbgapi_set_symtab(
		PHYS_TO_VIRTP( mbelf_sym_start ),
		PHYS_TO_VIRTP( mbelf_symstr_start ),
		mbelf_sym_count,
		mbelf_symstr_size );

}

/**
 * Actually load the ELF file symbol table.
 * This is split out from the parse_reserve function because it
 * requires the kernel memory manager, and the parse_reserve function
 * runs before that is available.
 */
void mbelf_load_syms( void )
{
	physmap_t *symmap, *strmap;
	Elf32_Sym *symtab;
	const char *strtab;

	symmap = paging_map_phys_range( mbelf_sym_start, mbelf_sym_size, 0 );
	if ( !symmap ) {
		printf( CON_WARN,
		        "could not map debug symtab" );
		return;
	}

	strmap = paging_map_phys_range( mbelf_symstr_start,
	                                mbelf_symstr_size, 0 );
	if ( !strmap ) {
		printf( CON_WARN,
		        "could not map debug strtab" );
		paging_unmap_phys_range( symmap );
		return;
	}

	symtab = symmap->virt;
	strtab = strmap->virt;

	dbgapi_set_symtab( symtab, strtab, mbelf_sym_count, mbelf_symstr_size );
}


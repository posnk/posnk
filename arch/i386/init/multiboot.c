#include "kernel/paging.h"
#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#define CON_SRC "multiboot"
#include "kernel/console.h"
#include "arch/i386/multiboot.h"
#include "kernel/system.h"
#include "arch/i386/vbe.h"
#include "driver/block/ramblk.h"
#include <string.h>

void mbelf_parse_reserve( multiboot_info_t* mbt );
void mbelf_load_syms( void );

vbe_mode_info_t	  vbe_mode;

#define PHYS_TO_VIRTP( a ) ((void*)( a + 0xC0000000 ))

static uint32_t parse_mem_map( multiboot_info_t* mbt )
{
	uint32_t available = 0;

	multiboot_memory_map_t* mmapr = PHYS_TO_VIRTP(mbt->mmap_addr);
	multiboot_memory_map_t* mmap  = mmapr;

	physmm_free_range(0x400000, 0x800000);

	while(((uintptr_t)mmap) < (((uintptr_t)mmapr) + mbt->mmap_length)) {
			/*earlycon_printf("MMAP ENTRY baseh=0x%x basel=0x%x lenh=0x%x lenl=0x%x type=0x%x",
				 mmap->base_addr_high,
				 mmap->base_addr_low,
				 mmap->length_high,
				 mmap->length_low,
				 mmap->type);*/
			// TODO : Verify page alignment of mmap entries
			if (mmap->base_addr_low >= 0x100000){

			if ((mmap->type == 1) && (mmap->base_addr_high == 0)) {
				if (mmap->length_high != 0)
					mmap->length_low = 0xFFFFFFFF - mmap->base_addr_low;
				available += mmap->length_low;
				physmm_free_range(mmap->base_addr_low, mmap->base_addr_low+mmap->length_low);
			}

		}
		mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}
	return available;
}

static void reserve_modules( multiboot_info_t *mbt )
{
	unsigned int i;
	multiboot_module_t *modules = PHYS_TO_VIRTP(mbt->mods_addr);

	/* If there are no modules available, do not call physmm_claim_range */
	/* as it does not allow zero-size ranges */
	if ( !mbt->mods_count )
		return;

	/* Claim the module table */
	physmm_claim_range( (physaddr_t)mbt->mods_addr,
	                    ((physaddr_t)mbt->mods_addr) +
	                     sizeof(multiboot_module_t)*(mbt->mods_count));

	for (i = 0; i < mbt->mods_count; i++){
		/* Claim the module string range */
		physmm_claim_range( (physaddr_t) modules[i].string,
		                    (physaddr_t) modules[i].string + 80 );

		/* Claim the module data range */
		physmm_claim_range( (physaddr_t)modules[i].mod_start,
		                    (physaddr_t)modules[i].mod_end );
	}
}

static void load_modules( multiboot_info_t *mbt )
{
	unsigned int i;
	size_t mod_size;
	physmap_t *map;

	multiboot_module_t *modules = PHYS_TO_VIRTP( mbt->mods_addr );

	for (i = 0; i < mbt->mods_count; i++){
		//char *name = (char *)(modules[i].string + 0xC0000000);

		mod_size = modules[i].mod_end - modules[i].mod_start;

		map = paging_map_phys_range( modules[i].mod_start,
		                             mod_size,
		                             PAGING_PAGE_FLAG_RW );

		if ( !map ) {
			printf( CON_ERROR, "could not map module %i", i );
		}

		//TODO: Unify Module handling across architectures

		ramblk_register( i, (aoff_t) mod_size, (void*) map->virt );
		//tar_extract_mem((void *) page_ptr);
		//for (ptr = 0; ptr < mod_size; ptr+=PHYSMM_PAGE_SIZE) {
		//	paging_unmap((void *) (page_ptr + ptr));
		//	physmm_free_frame((physaddr_t)(modules[i].mod_start + ptr));
		//}

	}
}


static void handle_vbe( multiboot_info_t *mbt )
{
	/* Check if we have video info */
	if ( ( ~mbt->flags & MULTIBOOT_INFO_VIDEO_INFO ) ||
	     ( !mbt->vbe_mode_info ) ) {
		puts(CON_ERROR, "BOOTLOADER DID NOT SET UP VIDEO MODE");
		vbe_mode.Xres = 0;
		return;
	}

	/* Copy VBE mode information */
	memcpy( &vbe_mode,
	        PHYS_TO_VIRTP( mbt->vbe_mode_info ),
	        sizeof( vbe_mode_info_t ) );

	/* Log the video mode that was detected */
	printf( CON_DEBUG,
	        "Bootloader VBE info @%x : "
	        "{mode: %i,  lfb: %x, w:%i, h: %i, bpp: %i}",
	        mbt->vbe_mode_info,
	        mbt->vbe_mode,
	        vbe_mode.physbase,
	        vbe_mode.Xres,
	        vbe_mode.Yres,
	        vbe_mode.bpp );

	//TODO: Implement descriptors for "non-PNP" features to allow
	//      video init to probe bootloader-provided framebuffer,
	//      this could be used on multiple platforms

}

static void handle_cmdline( multiboot_info_t *mbt )
{
	/* Check if we had a command line */
	if ( (~mbt->flags & MULTIBOOT_INFO_CMDLINE) || (!mbt->cmdline) )
		return;

	/* Copy cmdline string naively */
	memcpy( kernel_cmdline,
	        PHYS_TO_VIRTP( mbt->cmdline ),
		CONFIG_CMDLINE_MAX_LENGTH );

	/* To prevent crashes on malformed input, tack a guard NUL on the end */
	kernel_cmdline[ CONFIG_CMDLINE_MAX_LENGTH - 1 ] = 0;
}


uint32_t multiboot_mem_probe( multiboot_info_t *mbt )
{
	uint32_t mem_avail;

	mem_avail = parse_mem_map( mbt );
	reserve_modules( mbt );
	handle_vbe( mbt );
	handle_cmdline( mbt );
	mbelf_parse_reserve( mbt );

	return mem_avail;
}

void multiboot_load_data( multiboot_info_t *mbt )
{
	load_modules( mbt );
	mbelf_load_syms( );
}

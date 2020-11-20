/**
 * arch/i386/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#include "kernel/init.h"
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#define CON_SRC "i386_init"
#include "kernel/console.h"
#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "arch/i386/x86.h"
#include "arch/i386/idt.h"
#include "arch/i386/paging.h"
#include "arch/i386/multiboot.h"
#include "arch/i386/protection.h"
#include "arch/i386/vbe.h"
#include "driver/block/ramblk.h"
#include "kdbg/dbgapi.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

multiboot_info_t *mb_info;

//XXX: Dirty Hack
vbe_mode_info_t	  vbe_mode;

extern uint32_t i386_start_kheap;

uint32_t i386_init_multiboot_memprobe(multiboot_info_t* mbt)
{
	uint32_t available = 0;
	multiboot_memory_map_t* mmapr= (multiboot_memory_map_t*) (((uintptr_t)mbt->mmap_addr) + 0xC0000000) ;
	multiboot_memory_map_t* mmap = mmapr;
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

void i386_init_reserve_modules(multiboot_info_t *mbt)
{
	unsigned int i;
	multiboot_module_t *modules = (multiboot_module_t *) (mbt->mods_addr + 0xC0000000);
	if ( !mbt->mods_count )
		return;
	physmm_claim_range((physaddr_t)mbt->mods_addr, ((physaddr_t)mbt->mods_addr) + sizeof(multiboot_module_t)*(mbt->mods_count));
	for (i = 0; i < mbt->mods_count; i++){
		physmm_claim_range((physaddr_t)modules[i].string, (physaddr_t)modules[i].string + 80);
		physmm_claim_range((physaddr_t)modules[i].mod_start, (physaddr_t)modules[i].mod_end);
	}
}

void i386_init_handle_elf(multiboot_info_t *mbt)
{
	printf( CON_INFO, "multiboot: flags=0x%x",mbt->flags);
	if ( MULTIBOOT_INFO_ELFSYM & ~mbt->flags )
		return;
	printf( CON_INFO, "multiboot: elf symbol header"
	                "num:%i size:%i addr:%x shndx:%x",
	                mbt->u.elf_sec.num,
	                mbt->u.elf_sec.size,
	                mbt->u.elf_sec.addr,
	                mbt->u.elf_sec.shndx);
}
//TODO: Unify Module handling across architectures
void i386_init_load_modules(multiboot_info_t *mbt)
{
	unsigned int i;
	uintptr_t page_ptr = 0x40000000;
	size_t ptr = 0;
	size_t mod_size;
	multiboot_module_t *modules = (multiboot_module_t *) (mbt->mods_addr + 0xC0000000);
	if ( !mbt->mods_count )
		return;
	for (i = 0; i < mbt->mods_count; i++){
		//char *name = (char *)(modules[i].string + 0xC0000000);
		mod_size = modules[i].mod_end - modules[i].mod_start;
		page_ptr = (uintptr_t) heapmm_alloc_alligned( mod_size, 4096 );
		for (ptr = 0; ptr < mod_size; ptr+=PHYSMM_PAGE_SIZE) {
			physmm_free_frame(
				paging_get_physical_address((void *) (page_ptr + ptr)));
			paging_unmap((void *) (page_ptr + ptr));
			paging_map((void *) (page_ptr + ptr), (physaddr_t)(modules[i].mod_start + ptr), I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
		}
		ramblk_register( i, (aoff_t) mod_size, (void*) page_ptr );
		//tar_extract_mem((void *) page_ptr);
		//for (ptr = 0; ptr < mod_size; ptr+=PHYSMM_PAGE_SIZE) {
		//	paging_unmap((void *) (page_ptr + ptr));
		//	physmm_free_frame((physaddr_t)(modules[i].mod_start + ptr));
		//}

	}
}

void i386_init_handle_vbe(multiboot_info_t *mbt)
{
	if ((~mbt->flags & MULTIBOOT_INFO_VIDEO_INFO) || (!mbt->vbe_mode_info)) {
		puts(CON_ERROR, "BOOTLOADER DID NOT SET UP VIDEO MODE");
		vbe_mode.Xres = 0;
		return;
	}
	mbt->vbe_mode_info |= 0xC0000000;
	memcpy(&vbe_mode, (void *) mbt->vbe_mode_info, sizeof(vbe_mode_info_t));
	printf( CON_DEBUG, "Bootloader VBE info @%x : {mode: %i,  lfb: %x, w:%i, h: %i, bpp: %i}", (mbt->vbe_mode_info), (int)mbt->vbe_mode, (int)vbe_mode.physbase, (int)vbe_mode.Xres, (int)vbe_mode.Yres, (int)vbe_mode.bpp);
}

void i386_init_handle_cmdline(multiboot_info_t *mbt)
{
	/* Check if we had a command line */
	if ( (~mbt->flags & MULTIBOOT_INFO_CMDLINE) || (!mbt->cmdline) )
		return;
	memcpy( kernel_cmdline, (char *)(mbt->cmdline| 0xC0000000),
			CONFIG_CMDLINE_MAX_LENGTH );
	kernel_cmdline[CONFIG_CMDLINE_MAX_LENGTH-1] = 0;
}

extern char i386_resvmem_start;
extern char i386_resvmem_end;
void i386_init_mm(multiboot_info_t* mbd, unsigned int magic)
{
	size_t initial_heap = 4096;
	uint32_t mem_avail = 0;
	void *pdir_ptr;

	mbd= (multiboot_info_t*) (((uintptr_t)mbd) + 0xC0000000);

	/* Before setting up kernel memory we want to be able to produce debug
	 * output. Here we set up the debugger output */
	con_init();
	earlycon_init();
	debugcon_init();
	con_register_src("i386_init");
	puts( CON_DEBUG, "Debugger console up on ttyS0");

	i386_fpu_initialize();

	puts( CON_DEBUG, "Initializing physical memory manager...");
	physmm_init();

	puts( CON_DEBUG, "Registering available memory...");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		puts(CON_WARN, "Not loaded by multiboot");
		puts(CON_WARN, "Assuming 8MB of RAM and trying again...");
		physmm_free_range(0x100000, 0x800000);
		mem_avail = 0x700000;
	} else {
		mem_avail = i386_init_multiboot_memprobe(mbd);
		i386_init_reserve_modules(mbd);
		i386_init_handle_vbe(mbd);
		i386_init_handle_cmdline(mbd);
		i386_init_handle_elf(mbd);
	}
	physmm_claim_range(0x100000, 0x400000);
	physmm_free_range((physaddr_t)&i386_resvmem_start - 0xc0000000,(physaddr_t)&i386_resvmem_end - 0xc0000000);
	printf( CON_DEBUG, "Reserved memory: %x %x", &i386_resvmem_start - 0xc0000000,&i386_resvmem_end - 0xc0000000);
	printf( CON_DEBUG, "Detected %i MB of RAM.", (mem_avail/0x100000));
	puts( CON_DEBUG, "Enabling paging...");
	paging_init();

	puts( CON_DEBUG, "Initializing kernel heap manager...");
	kdbg_initialize();

	initial_heap = heapmm_request_core((void *)0xD0000000, initial_heap);

	if (initial_heap == 0) {
		puts( CON_DEBUG,"Could not allocate first page of heap!");
		for(;;); //TODO: PANIC!!!
	}

	heapmm_init((void *)0xD0000000, initial_heap);

	pdir_ptr = heapmm_alloc_page();
	physmm_free_frame(paging_get_physical_address(pdir_ptr));
	paging_map(pdir_ptr, paging_get_physical_address((void *)0xFFFFF000), PAGING_PAGE_FLAG_RW);
	paging_active_dir->content = pdir_ptr;
	puts( CON_DEBUG, "initializing kernel stack...");

	puts( CON_DEBUG, "calling kmain...");
	mb_info = mbd;
	paging_unmap( 0 );
	/**
	 * Once we are here everything is set up properly
	 * First 4 MB are both at 0xC000 0000 and 0x0000 0000 to support GDT trick!
	 * Task Stack	is at 0xBFFF DFFF downwards
	 * ISR Stack	is at 0xBFFF E000
	 * --------- KERNEL RAM BOUNDARY ----------
	 * Code    	is at 0xC000 0000
	 * Heap		is at 0xD000 0000
	 * Pagedir	is at 0xFFC0 0000
	 */
	kmain();


}

void arch_init_early() {
	puts( CON_DEBUG, "loading exception handlers");
	i386_idt_initialize();
	i386_protection_init();
}

void arch_init_late() {
	puts( CON_DEBUG, "loading module files");
	i386_init_load_modules(mb_info);
}

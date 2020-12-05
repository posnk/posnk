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
#include "kdbg/dbgapi.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

#define PHYS_TO_VIRTP( a ) ((void*)( a + 0xC0000000 ))

multiboot_info_t *mb_info;

extern uint32_t i386_start_kheap;

extern char i386_resvmem_start;
extern char i386_resvmem_end;

uint32_t multiboot_mem_probe( multiboot_info_t *mbt );

void multiboot_load_data( multiboot_info_t *mbt );

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
		mem_avail = multiboot_mem_probe( mbd );
	}

	physmm_claim_range(0x100000, 0x400000);
	physmm_free_range((physaddr_t)&i386_resvmem_start - 0xc0000000,(physaddr_t)&i386_resvmem_end - 0xc0000000);

	printf( CON_DEBUG, "Reserved memory: %x %x", &i386_resvmem_start - 0xc0000000,&i386_resvmem_end - 0xc0000000);

	printf( CON_DEBUG, "Detected %i MB of RAM.", (mem_avail/0x100000));

	puts( CON_DEBUG, "Enabling paging...");
	dbgapi_set_symtab( NULL, NULL, 0, 0 );
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
	puts( CON_DEBUG, "calling kmain...");
	kmain();


}

void arch_init_early() {
	puts( CON_DEBUG, "loading exception handlers");
	i386_idt_initialize();
	i386_protection_init();
	i386_enable_smep();
}

void arch_init_late() {
	puts( CON_DEBUG, "loading module files");
	multiboot_load_data( mb_info );
}

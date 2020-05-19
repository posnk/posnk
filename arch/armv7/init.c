/**
 * arch/armv7/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2014 - Created
 */
#include <fcntl.h>
#include <sys/errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include "kernel/interrupt.h"
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/physmm.h"
#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/vfs.h"
#include "kernel/tar.h"
#include "kernel/ipc.h"
#include "kernel/device.h"
#include "kernel/tty.h"
#include "kernel/drivermgr.h"
#include "kernel/init.h"
#include "kdbg/dbgapi.h"
#include "arch/armv7/bootargs.h"
#include "arch/armv7/exception.h"
#include "arch/armv7/cpu.h"
void sercon_init();

char *armv7_cmdline[128];
uint32_t armv7_initrd_start_pa;
uint32_t armv7_initrd_end_pa;

void halt()
{
	for(;;);
}

void wait_int() {
       	armv7_enable_ints();
}

void armv7_initrd_extr(physaddr_t start, physaddr_t end)
{
	physaddr_t pa, pac;
	uintptr_t va, vap;
	
	pa = start & ~0xFFF;
	va = 0x80000000;
	vap = va;
	for (pac = pa; pac < end; pac+=4096) {
		paging_map ((void *)vap, pac, PAGING_PAGE_FLAG_RW );
		vap+=4096;
	}

	vap = va + (start & 0xFFF);

	tar_extract_mem((void *)vap);

	for (pac = pa; pac< end; pac+=4096)  {
		paging_unmap((void *)va);
		physmm_free_frame(pac);
		va += 4096;
	}

}

void armv7_init( armv7_bootargs_t *bootargs )
{
	size_t initial_heap = 4096;
	char * rootfs_type = "ramfs";
	sercon_init();

	earlycon_printf("posnk kernel built on %s %s\n", __DATE__, __TIME__);

	earlycon_printf("bootargs: loading boot arguments from 0x%x\n", bootargs);

	strcpy(armv7_cmdline, bootargs->ba_cmd);
	
	armv7_initrd_start_pa = bootargs->ba_initrd_pa;	
	armv7_initrd_end_pa = bootargs->ba_initrd_pa + bootargs->ba_initrd_sz;

	earlycon_printf("bootargs: command line = %s\n", armv7_cmdline);
	earlycon_printf("bootargs: initrd [0x%x-0x%x]\n", armv7_initrd_start_pa,
							armv7_initrd_end_pa);

	memcpy(physmm_bitmap, bootargs->ba_pm_bitmap, 32768 * sizeof(uint32_t));
	armv7_exception_init();
	earlycon_printf("physmm: initializing...\n");

	physmm_free_range((physaddr_t)bootargs->ba_pm_bitmap,
			 ((physaddr_t)bootargs->ba_pm_bitmap)
				 + 32768 * sizeof(uint32_t));

	earlycon_printf("physmm: %i MB available\n",
			physmm_count_free() / 0x100000);

	earlycon_printf("paging: initializing virtual memory support\n");
	armv7_paging_init(bootargs);
	
	earlycon_printf("kdbg: initializing kernel debugger\n");
	kdbg_initialize();
	earlycon_printf("heapmm: initializing kernel heap manager\n");
	initial_heap = heapmm_request_core((void *)0xd0000000, initial_heap);
	if (initial_heap == 0) {
		earlycon_printf("could not alloc first page of heap!\n");
		halt();
	}
	heapmm_init((void *) 0xd0000000, initial_heap);

	paging_map((void *) 0xBFFFF000, physmm_alloc_frame(), 
			PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);

	paging_map((void *) 0xBFFFE000, physmm_alloc_frame(), 
			PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
	stack_switch_call(kmain, 0xBFFFFFF0);

	halt();
}

void arch_init_early() {
}

void arch_init_late() {
	earlycon_printf("vfs: extracting initrd archive\n");
	armv7_initrd_extr(armv7_initrd_start_pa, armv7_initrd_end_pa);
}




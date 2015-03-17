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

#include <stdint.h>
#include "kernel/physmm.h"
#include "kernel/earlycon.h"
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


void armv7_init( armv7_bootargs_t *bootargs )
{
	sercon_init();

	earlycon_printf("posnk kernel built on %s %s\n", __DATE__,__TIME__);

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

	//armv7_paging_init(bootargs);

	halt();
	
}

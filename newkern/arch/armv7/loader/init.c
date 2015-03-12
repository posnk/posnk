/**
 * arch/armv7/loader/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-03-2014 - Created
 */

#include <stdint.h>
#include <string.h>
#include "arch/armv7/atags.h"
#include "kernel/physmm.h"
#include "arch/armv7/loader.h"
#include "arch/armv7/cpu.h"
#include "arch/armv7/mmu.h"
#include "arch/armv7/exception.h"
#include "arch/armv7/bootargs.h"

/*
 * The CPU must be in SVC (supervisor) mode with both IRQ and FIQ interrupts disabled.
 * The MMU must be off, i.e. code running from physical RAM with no translated addressing.
 * Data cache must be off
 * Instruction cache may be either on or off
 * CPU register 0 must be 0
 * CPU register 1 must be the ARM Linux machine type
 * CPU register 2 must be the physical address of the parameter list
 * Our assembly stub has prepared 4K stack for us.
 */
/*
 * 0x90000000   --------------------- END OF RAM 
 *
 * 0x82000000	| posnk kernel[text]|
 *		|-------------------|
 * 0x8????000	|   	vmpos	    |
 * 	|	|-------------------|
 * 0x80008000	|   armv7_loader    |
 * 	|	|-------------------|
 * 0x80000000	| BOOTLOADER, ATAGS |
 * 		--------------------- START OF RAM
 */

armv7_bootargs_t *bootargs;

void halt()
{
	for(;;);
}

void armv7_diepagep()
{
	sercon_printf("mmu: i pagefault 0x%x\n", armv7_mmu_pf_abort_addr());
	halt();
}

void armv7_diepaged()
{
	sercon_printf("mmu: d pagefault 0x%x\n", armv7_mmu_data_abort_addr());
	halt();
}

void armv7_add_kmap(uint32_t pa, uint32_t va, uint32_t sz, uint32_t fl)
{
	int count = bootargs->ba_kmap_count++;
	bootargs->ba_kmap[count].ba_kmap_pa = pa;
	bootargs->ba_kmap[count].ba_kmap_va = va;
	bootargs->ba_kmap[count].ba_kmap_sz = sz;
	bootargs->ba_kmap[count].ba_kmap_fl = fl;
}

void armv7_call_kern(void *main, void *stack, void *arg);

void armv7_boot_kern()
{
	physaddr_t frame;
	void *vad = (void *) elf_top;
	frame = physmm_alloc_frame();
	armv7_add_kmap(frame, vad, 4096, ARMV7_BA_KMAP_READ | ARMV7_BA_KMAP_WRITE);
	if (frame == PHYSMM_NO_FRAME)	{
		sercon_printf("NO MEMORY!\n");
		halt();
	}
	armv7_mmu_map((void *)vad, frame);
	armv7_call_kern(elf_kmain, vad, bootargs);
}

void armv7_entry(uint32_t unused_reg, uint32_t mach_type, uint32_t atag_addr)
{
	physaddr_t ldr_start, ldr_end;

	sercon_init();

	sercon_printf("posnk armv7_loader built on %s %s\n", __DATE__,__TIME__);
	sercon_printf("initial state:\nr0: 0x%x, r1: 0x%x, r2:0x%x\n", 
				unused_reg, 
				mach_type, 
				atag_addr);

	sercon_printf("physmm: initializing...\n");
	physmm_init();

	sercon_printf("parsing atags...\n");
	armv7_parse_atags( (void *) atag_addr );

	sercon_printf("physmm: registered %i MB RAM\n",
			physmm_count_free() / 0x100000);

	ldr_start = 0x80000000;
	ldr_end = (physaddr_t) &_armv7_start_kheap;
	ldr_end = (ldr_end + 0xfff) & 0xFFFFF000;

	physmm_claim_range(ldr_start, ldr_end);
	sercon_printf("initrd: found initrd from 0x%x-0x%x\n",
			armv7_atag_initrd_pa, 
			   armv7_atag_initrd_pa + armv7_atag_initrd_sz);

	physmm_claim_range(armv7_atag_initrd_pa, 
			   armv7_atag_initrd_pa + armv7_atag_initrd_sz + 0xfff);

	sercon_printf("physmm: %i MB available\n",
			physmm_count_free() / 0x100000);

	sercon_printf("mmu: initializing, identity mapping RAM...\n");

	armv7_init_mmu(0x80000000,0x90000000);

	sercon_printf("mmu: initialized\n");

	sercon_printf("cpu: registering exception handlers...\n");

	armv7_handler_table[VEC_DATA_ABORT] = &armv7_diepaged;
	armv7_handler_table[VEC_PREFETCH_ABORT] = &armv7_diepagep;
	armv7_exception_init();

	sercon_printf("bootargs: initializing boot argument list...\n");
	bootargs = physmm_alloc_frame();

	bootargs->ba_magic = ARMV7_BOOTARGS_MAGIC;
	bootargs->ba_initrd_pa = armv7_atag_initrd_pa;
	bootargs->ba_initrd_sz = armv7_atag_initrd_sz;
	bootargs->ba_cmd = (char *) (bootargs + sizeof(armv7_bootargs_t));
	strcpy((char *)bootargs->ba_cmd, armv7_atag_cmdline);
	bootargs->ba_kmap = (armv7_ba_kmap_t *) (bootargs + 
						 sizeof(armv7_bootargs_t) +
					strlen((char *)bootargs->ba_cmd) + 1);
	bootargs->ba_kmap_count = 0;
	bootargs->ba_pm_bitmap = physmm_alloc_bmcopy();

	sercon_printf("bootargs: command line = %s\n", bootargs->ba_cmd);

	sercon_printf("elf: loading payload...\n");

	elf_load((char *)&_binary_payload_armv7_start);

	sercon_printf("elf: payload loaded\n");

	sercon_printf("physmm: releasing loader memory...\n");
	physmm_free_range(ldr_start, ldr_end);

	sercon_printf("physmm: %i MB available for kernel use\n",
			physmm_count_free() / 0x100000);

	sercon_printf("bootargs: copying physical memory usage map...\n");

	memcpy(bootargs->ba_pm_bitmap, physmm_bitmap, 32768 * sizeof(uint32_t));

	sercon_printf("invoking kernel...\n\n\n");

	armv7_boot_kern();

	halt();
	
}

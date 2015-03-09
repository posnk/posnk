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
#include "arch/armv7/atags.h"
#include "kernel/physmm.h"
#include "arch/armv7/loader.h"

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

void sercon_puts(const char *string);
void sercon_init();

uint32_t armv7_start_kheap;

static struct atag *params; /* used to point at the current tag */

uint32_t	armv7_get_mode( void );

void halt()
{
	for(;;);
}

void armv7_entry(uint32_t unused_reg, uint32_t mach_type, uint32_t atag_addr)
{
	physaddr_t ldr_start, ldr_end;

	sercon_init();

	sercon_printf("posnk armv7_loader built on %s %s\n", __DATE__,__TIME__);
	sercon_printf("initial state:\nr0: 0x%x, r1: 0x%x, r2:0x%x\n", unused_reg, mach_type, atag_addr);

	sercon_printf("physmm: initializing...\n");
	physmm_init();

	sercon_printf("parsing atags...\n");
	armv7_parse_atags( (void *) atag_addr );

	sercon_printf("physmm: registered %i MB RAM\n",
			physmm_count_free() / 0x100000);

	ldr_start = 0x80000000;
	ldr_end = (physaddr_t) &armv7_start_kheap;
	ldr_end = (ldr_end + 0xfff) & 0xFFFFF000;

	physmm_claim_range(ldr_start, ldr_end);

	sercon_printf("physmm: %i MB available\n",
			physmm_count_free() / 0x100000);

	sercon_printf("mmu: initializing, identity mapping RAM...\n");

	armv7_init_mmu(0x80000000,0x90000000);

	sercon_printf("mmu: initialized\n");

	halt();
	
}

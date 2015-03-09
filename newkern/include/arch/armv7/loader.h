/**
 * arch/armv7/loader.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2015 - Created
 */

#ifndef __ARCH_ARMV7_LOADER_H__
#define __ARCH_ARMV7_LOADER_H__

void armv7_init_mmu(physaddr_t ram_start, physaddr_t ram_stop);
void armv7_mmu_map(void * virt_addr, physaddr_t phys_addr);
void armv7_mmu_unmap(void * virt_addr);

#endif

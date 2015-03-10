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

void sercon_puts(const char *string);
int sercon_printf(const char *fmt, ...);
void sercon_init();

extern uint32_t _armv7_start_kheap;
extern uint32_t _binary_payload_armv7_start;

uint32_t	armv7_get_mode( void );

void armv7_init_mmu(physaddr_t ram_start, physaddr_t ram_stop);
void armv7_mmu_map(void * virt_addr, physaddr_t phys_addr);
void armv7_mmu_unmap(void * virt_addr);

void armv7_add_kmap(uint32_t pa, uint32_t va, uint32_t sz, uint32_t fl);

void (*elf_kmain)(uint32_t);

int elf_load(char * file);
#endif

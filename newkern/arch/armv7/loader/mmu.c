/**
 * arch/armv7/loader/mmu.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2015 - Created
 */

#include "arch/armv7/mmu.h"
#include "util/llist.h"
#include "kernel/physmm.h"
#include "config.h"
#include <string.h>
#include <assert.h>

armv7_l1_table_t 	*armv7_l1_table;

void armv7_init_mmu(physaddr_t ram_start, physaddr_t ram_stop)
{
	physaddr_t pa;

	armv7_l1_table = (armv7_l1_table_t *) physmm_alloc_quadframe();
	memset(armv7_l1_table, 0, sizeof(armv7_l1_table_t));
	/* Identity map all RAM */
	for (pa = ram_start; pa < ram_stop; pa += ARMV7_SECTION_SIZE) {
		armv7_l1_table->entries[ARMV7_TO_L1_IDX(pa)] = 
			ARMV7_L1_SECTION_PA(pa) |
			ARMV7_L1_SECTION_AP(3)  |
			ARMV7_L1_TYPE_SECTION;
	}
	for (pa = 0x40000000; pa < 0x50000000; pa += ARMV7_SECTION_SIZE) {
		armv7_l1_table->entries[ARMV7_TO_L1_IDX(pa)] = 
			ARMV7_L1_SECTION_PA(pa) |
			ARMV7_L1_SECTION_AP(3)  |
			ARMV7_L1_TYPE_SECTION;
	}
	earlycon_printf("mmu: l1table address: 0x%x [0x%x]\n", armv7_l1_table, armv7_l1_table->entries[ARMV7_TO_L1_IDX(0x80000000)]);
	armv7_mmu_flush_tlb();
	armv7_mmu_set_dacr (0xFFFFFFFF);
	armv7_mmu_set_ttbr0((uint32_t)armv7_l1_table);
	armv7_mmu_set_ttbr1((uint32_t)armv7_l1_table);
	armv7_mmu_set_ttbcr(0x00000000);
	armv7_mmu_enable();

}

void armv7_mmu_map(void * virt_addr, physaddr_t phys_addr)
{
	uint32_t l1_entry, l2_entry;
	physaddr_t table_addr;
	armv7_l2_table_t *table;
	uintptr_t l1_idx = ARMV7_TO_L1_IDX(virt_addr);
	uintptr_t l2_idx = ARMV7_TO_L2_IDX(virt_addr);

	if (armv7_l1_table->entries[l1_idx] == 0) {
		/* Page table does not yet exist */
		table_addr = physmm_alloc_frame();
		if (table_addr == PHYSMM_NO_FRAME) {
			assert(table_addr != PHYSMM_NO_FRAME);
		}
		l1_entry = 	ARMV7_L1_TYPE_PAGE_TABLE | 
				ARMV7_L1_PAGE_TABLE_PA(table_addr);

		table = (armv7_l2_table_t *) table_addr;
		armv7_l1_table->entries[l1_idx] = l1_entry;
		memset(table, 0, sizeof(armv7_l2_table_t));
	} else {
		table = (armv7_l2_table_t *) 
			ARMV7_L1_PAGE_TABLE_PA(armv7_l1_table->entries[l1_idx]);
	}

	if (table->pages[l2_idx] & 3) {
		/* Tried to map something that was already mapped */
		/* We will allow this but maybe warn here */
		//TODO: Determine whether we have to free previously mapped frame
		table->pages[l2_idx] = 0;
	}
	
	l2_entry = 	ARMV7_L2_TYPE_PAGE4 | 
			ARMV7_L2_PAGE_AP(3) | 
			ARMV7_L2_PAGE4_PA(phys_addr);
	table->pages[l2_idx] = l2_entry;
	armv7_mmu_flush_tlb_single((uintptr_t)virt_addr);
}

void armv7_mmu_unmap(void * virt_addr)
{
	uintptr_t l1_idx = ARMV7_TO_L1_IDX(virt_addr);
	uintptr_t l2_idx = ARMV7_TO_L2_IDX(virt_addr);
	armv7_l2_table_t *table = (armv7_l2_table_t *) ARMV7_L1_PAGE_TABLE_PA(armv7_l1_table->entries[l1_idx]);
	table->pages[l2_idx] = 0;
}


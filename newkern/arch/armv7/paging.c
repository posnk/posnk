/**
 * arch/armv7/paging.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 11-03-2015 - Created
 */
#include "arch/armv7/bootargs.h"
#include "arch/armv7/mmu.h"
#include "util/llist.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "config.h"
#include <string.h>
#include <assert.h>

#define ARMV7_PAGE_L2TABLE	(0xFFFFF000)
#define ARMV7_PAGE_PHYSWIN	(0xFFFFE000)
#define ARMV7_INITIAL_L1	(0xFFFFA000)
#define ARMV7_PAGE_L2SPEC	(0xFFFF9000)

llist_t			armv7_l1_table_list;

armv7_l2_table_t	*armv7_special_l2;
armv7_l1_table_list_t	armv7_l1_table_list_initial;
page_dir_t 		armv7_page_dir_initial;

unsigned int		armv7_level_2_count;
physaddr_t		armv7_level_2_alloc_base;

void paging_init(){}

void armv7_paging_init(armv7_bootargs_t *args)
{
	unsigned int	  kmap_ctr;
	uint32_t	  l1_entry;
	uintptr_t	  km_va_end, km_va;
	physaddr_t	  level1_pa, spec_l2_pa, level2_pa, km_pa;
	armv7_l1_table_t *level1_va;
	armv7_l2_table_t *level2_va;

	/* Initialize table list */
	llist_create(&armv7_l1_table_list);

	/* Allocate initial level 1 table */
	level1_pa = physmm_alloc_quadframe();

	//TODO: Check for out of memory error

	/* We haven't yet got paging running so use PA */
	level1_va = (armv7_l1_table_t *) level1_pa;

	/* Clear page directory */
	memset(level1_va, 0, sizeof(armv7_l1_table_t));

	/* Allocate "special" level2 table */
	spec_l2_pa = physmm_alloc_frame();

	//TODO: Check for out of memory error

	/* Initialize level 2 allocator */
	armv7_level_2_alloc_base = spec_l2_pa;
	armv7_level_2_count = 1;

	/* We haven't yet got paging running so use PA */
	armv7_special_l2 = (armv7_l2_table_t *) spec_l2_pa;

	/* Clear page table */
	memset(armv7_special_l2, 0, sizeof(armv7_l2_table_t));

	/* Set up page directory entry for special level 2 table */
	level1_va->entries[ARMV7_TO_L1_IDX(ARMV7_PAGE_L2SPEC)] =
				ARMV7_L1_TYPE_PAGE_TABLE |
				ARMV7_L1_PAGE_TABLE_PA(spec_l2_pa);

	/* Set up page table entry for special level 2 table */
	armv7_special_l2->pages[ARMV7_TO_L2_IDX(ARMV7_PAGE_L2SPEC)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(spec_l2_pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(1);

	/* Set up page table entries for initial page dir */
	km_va_end = ARMV7_INITIAL_L1 + ARMV7_L1_TABLE_SIZE * sizeof(uint32_t);
	km_pa = level1_pa;
	for (km_va = ARMV7_INITIAL_L1; km_va < km_va_end; km_va += 4096) {
		armv7_special_l2->pages[ARMV7_TO_L2_IDX(km_va)] = 
					ARMV7_L2_TYPE_PAGE4 |
					ARMV7_L2_PAGE4_PA(km_pa) | 
					ARMV7_L2_PAGE4_TEX(0) | 
					ARMV7_L2_PAGE_AP(1);
		km_pa+=4096;
	}

	/* Map kernel mappings */
	for (kmap_ctr = 0; kmap_ctr < args->ba_kmap_count; kmap_ctr++) {

		km_va	  = args->ba_kmap[kmap_ctr].ba_kmap_va;
		km_va_end = km_va + args->ba_kmap[kmap_ctr].ba_kmap_sz;
		km_pa 	  = args->ba_kmap[kmap_ctr].ba_kmap_pa;
		sercon_printf("paging: mapping k: 0x%x [0x%x-0x%x]\n", km_pa,km_va,km_va_end);
		for (; km_va < km_va_end; km_va += 4096) {
			
			l1_entry = level1_va->entries[ARMV7_TO_L1_IDX(km_va)];
			
			if ((l1_entry & 3) == ARMV7_L1_TYPE_FAULT) {
				level2_pa = armv7_level_2_alloc_base + 
					ARMV7_L2_TABLE_SIZE * sizeof(uint32_t)
					 * armv7_level_2_count++;
				if (armv7_level_2_count == 4) {
					armv7_level_2_count = 0;
					armv7_level_2_alloc_base 
						= physmm_alloc_frame();

					//TODO: Check for out of memory error
				}
				memset( (void *) level2_pa, 
						0, 
						ARMV7_L2_TABLE_SIZE * 
						sizeof(uint32_t));
				l1_entry = ARMV7_L1_TYPE_PAGE_TABLE |
					ARMV7_L1_PAGE_TABLE_PA(level2_pa);
				level1_va->entries[ARMV7_TO_L1_IDX(km_va)] = 
					l1_entry;
			}
			level2_pa = ARMV7_L1_PAGE_TABLE_PA(l1_entry);
			level2_va = (armv7_l2_table_t *) level2_pa;
			level2_va->pages[ARMV7_TO_L2_IDX(km_va)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(km_pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(ARMV7_AP_PRIV_RW_USR_NA);
			km_pa += 4096;
		}

	}

 	/* Identity map all DEVICES (for now) */
	for (km_pa = 0x40000000; km_pa < 0x50000000; km_pa += ARMV7_SECTION_SIZE) {
		level1_va->entries[ARMV7_TO_L1_IDX(km_pa)] = 
				ARMV7_L1_SECTION_PA(km_pa) 	|
				ARMV7_L1_SECTION_AP(ARMV7_AP_PRIV_RW_USR_NA)  |
				ARMV7_L1_TYPE_SECTION;
	}


	sercon_printf("paging: l1table address: 0x%x [0x%x]\n", level1_pa, level1_va->entries[ARMV7_TO_L1_IDX(0xC0000000)]);
	armv7_mmu_set_dacr (0xFFFFFFFF);

	armv7_mmu_set_ttbr0((uint32_t)level1_pa);
	armv7_mmu_set_ttbr1((uint32_t)level1_pa);
	armv7_mmu_set_ttbcr(0x00000000);
	armv7_mmu_flush_tlb();

	armv7_special_l2 = (void *)ARMV7_PAGE_L2SPEC;
	level1_va = (void *)ARMV7_INITIAL_L1;
	sercon_printf("paging: online , l1table address: 0x%x [0x%x]\n", level1_pa, level1_va->entries[ARMV7_TO_L1_IDX(0xC0000000)]);

	paging_active_dir = &armv7_page_dir_initial;
	paging_active_dir->content = level1_va;
	llist_add_end(&armv7_l1_table_list, 
				(llist_t *) &armv7_l1_table_list_initial);
	armv7_l1_table_list_initial.dir = &armv7_page_dir_initial;
}

armv7_l2_table_t *armv7_paging_map_l2( physaddr_t table_pa )
{
	uintptr_t table_va = table_pa & PHYSMM_PAGE_ADDRESS_MASK;

	armv7_special_l2->pages[ARMV7_TO_L2_IDX(ARMV7_PAGE_L2TABLE)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(table_pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(1);

	armv7_mmu_flush_tlb_single(ARMV7_PAGE_L2TABLE);

	return (armv7_l2_table_t *) table_va;
}

void paging_switch_dir(page_dir_t *new_dir)
{
	armv7_l1_table_t *level1_va;
	physaddr_t	  level1_pa;

	level1_va = (armv7_l1_table_t *) new_dir->content;

	assert ( level1_va != NULL);

	level1_pa = paging_get_physical_address ( level1_va );

	assert ( level1_pa != 0);

	paging_active_dir = new_dir;

	armv7_mmu_set_ttbr0((uint32_t)level1_pa);
	armv7_mmu_set_ttbr1((uint32_t)level1_pa);
	armv7_mmu_flush_tlb();

}

page_dir_t	*paging_create_dir()
{
	return NULL;
}

void		paging_free_dir(page_dir_t *dir)
{
}

void paging_map(void * virt_addr, physaddr_t phys_addr, page_flags_t flags)
{
	uint32_t l1_entry, ap;
	physaddr_t level2_pa;
	uintptr_t va = (uintptr_t) virt_addr;
	armv7_l1_table_t *level1_va;
	armv7_l2_table_t *level2_va;

	level1_va = (armv7_l1_table_t *) paging_active_dir->content;

	l1_entry = level1_va->entries[ARMV7_TO_L1_IDX(va)];
	if ((l1_entry & 3) == ARMV7_L1_TYPE_FAULT) {
		level2_pa = armv7_level_2_alloc_base + 
				ARMV7_L2_TABLE_SIZE * sizeof(uint32_t)
					 * armv7_level_2_count++;
		if (armv7_level_2_count == 4) {
			armv7_level_2_count = 0;
			armv7_level_2_alloc_base = physmm_alloc_frame();
			//TODO: Check for out of memory error
		}
		memset( (void *) level2_pa, 
			0, 
			ARMV7_L2_TABLE_SIZE * sizeof(uint32_t));
		l1_entry = 	ARMV7_L1_TYPE_PAGE_TABLE |
				ARMV7_L1_PAGE_TABLE_PA(level2_pa);
		level1_va->entries[ARMV7_TO_L1_IDX(va)] = l1_entry;
	}

	level2_pa = ARMV7_L1_PAGE_TABLE_PA(l1_entry);
	level2_va = armv7_paging_map_l2(level2_pa);

	if (flags & PAGING_PAGE_FLAG_USER) {
		if (flags & PAGING_PAGE_FLAG_RW) 
			ap = ARMV7_AP_PRIV_RW_USR_RW;
		else
			ap = ARMV7_AP_PRIV_RW_USR_RO;
	} else {
		if (flags & PAGING_PAGE_FLAG_RW) 
			ap = ARMV7_AP_PRIV_RW_USR_NA;
		else
			ap = ARMV7_AP_PRIV_RW_USR_NA;//Consistency with i386
		//TODO: Check if this is correct
						
	} 

	level2_va->pages[ARMV7_TO_L2_IDX(va)] = 
		ARMV7_L2_TYPE_PAGE4 |
		ARMV7_L2_PAGE_AP(ap) |
		ARMV7_L2_PAGE4_PA(phys_addr);
	
	armv7_mmu_flush_tlb_single( va );
}

void paging_unmap(void * virt_addr)
{
	uint32_t		 l1_entry, l2_entry;
	uintptr_t		 l1_idx, l2_idx;
	physaddr_t 		 level2_pa;
	armv7_l1_table_t 	*level1_va;
	armv7_l2_table_t 	*level2_va;

	assert ( paging_active_dir != NULL);

	level1_va = (armv7_l1_table_t *) paging_active_dir->content;

	assert ( level1_va != NULL);

	l1_idx = ARMV7_TO_L1_IDX( virt_addr );
	l2_idx = ARMV7_TO_L2_IDX( virt_addr );
	
	l1_entry = level1_va->entries[l1_idx];

	assert ( (l1_entry & 3) != ARMV7_L1_TYPE_FAULT );	

	level2_pa = ARMV7_L1_PAGE_TABLE_PA( l1_entry );
	
	level2_va = armv7_paging_map_l2( level2_pa );
	
	l2_entry = level2_va->pages[ l2_idx ];
	l2_entry = (l2_entry & ~3) | ARMV7_L2_TYPE_FAULT;
	level2_va->pages[ l2_idx ] = l2_entry;
	
	armv7_mmu_flush_tlb_single( (uint32_t) virt_addr );
}

page_tag_t	paging_get_tag(void * virt_addr)
{
	return (page_tag_t) 0;
}


void		paging_tag(void * virt_addr, page_tag_t tag)
{
}

uintptr_t	paging_get_physical_address(void * virt_addr)
{
	uint32_t par;
	uintptr_t va;
	uintptr_t pa;

	va = (uintptr_t) virt_addr;

	par = armv7_mmu_translate(va, ARMV7_TRANSLATE_OP_PRIV_R);

	if ( par & ARMV7_PAR_FAULT )
		return 0;

	pa  = ARMV7_PAR_PA(par);
	pa |= va & PHYSMM_PAGE_ADDRESS_MASK;

	return pa;
}


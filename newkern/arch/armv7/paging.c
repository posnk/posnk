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
#include "kernel/earlycon.h"
#include "config.h"
#include <string.h>
#include <assert.h>

#define ARMV7_PAGE_L2TABLE	(0xFFFFF000)
#define ARMV7_PAGE_PHYSWIN	(0xFFFFE000)
#define ARMV7_INITIAL_L1	(0xFFFFA000)
#define ARMV7_PAGE_L2SPEC	(0xFFFF9000)
#define ARMV7_PAGE_PHYSWI2	(0xFFFF8000)

llist_t			armv7_l1_table_list;

armv7_l2_table_t	*armv7_special_l2;
armv7_l1_table_list_t	armv7_l1_table_list_initial;
page_dir_t 		armv7_page_dir_initial;

unsigned int		armv7_level_2_count;
physaddr_t		armv7_level_2_alloc_base;

void paging_init(){}

physaddr_t armv7_paging_alloc_l2( void )
{
	physaddr_t level2_pa;

	level2_pa = armv7_level_2_alloc_base + 
		ARMV7_L2_TABLE_SIZE * sizeof(uint32_t) * armv7_level_2_count++;
	if (armv7_level_2_count == 4) {
		armv7_level_2_count = 0;
		armv7_level_2_alloc_base = physmm_alloc_frame();
		//TODO: Check for out of memory error
	}

	return level2_pa;
}

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
		for (; km_va < km_va_end; km_va += 4096) {
			
			l1_entry = level1_va->entries[ARMV7_TO_L1_IDX(km_va)];
			
			if ((l1_entry & 3) == ARMV7_L1_TYPE_FAULT) {
				level2_pa = armv7_paging_alloc_l2();
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


	armv7_mmu_set_dacr (0x55555555);

	armv7_mmu_set_ttbr0((uint32_t)level1_pa);
	armv7_mmu_set_ttbr1((uint32_t)level1_pa);
	armv7_mmu_set_ttbcr(0x00000000);
	armv7_mmu_flush_tlb();

	armv7_special_l2 = (void *)ARMV7_PAGE_L2SPEC;
	level1_va = (void *)ARMV7_INITIAL_L1;

	paging_active_dir = &armv7_page_dir_initial;
	paging_active_dir->content = level1_va;
	llist_add_end(&armv7_l1_table_list, 
				(llist_t *) &armv7_l1_table_list_initial);
	armv7_l1_table_list_initial.dir = &armv7_page_dir_initial;
}

armv7_l2_table_t *armv7_paging_map_l2( physaddr_t table_pa )
{
	uintptr_t table_va = table_pa & PHYSMM_PAGE_ADDRESS_MASK;

	table_va |= ARMV7_PAGE_L2TABLE;

	armv7_special_l2->pages[ARMV7_TO_L2_IDX(ARMV7_PAGE_L2TABLE)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(table_pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(1);

	armv7_mmu_flush_tlb_single(ARMV7_PAGE_L2TABLE);

	return (armv7_l2_table_t *) table_va;
}

void *armv7_paging_map_phys( physaddr_t pa )
{
	uintptr_t va = pa & PHYSMM_PAGE_ADDRESS_MASK;

	va |= ARMV7_PAGE_PHYSWIN;
	armv7_special_l2->pages[ARMV7_TO_L2_IDX(ARMV7_PAGE_PHYSWIN)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(1);

	armv7_mmu_flush_tlb_single(ARMV7_PAGE_PHYSWIN);

	return (void *) va;
}

void *armv7_paging_map_phys2( physaddr_t pa )
{
	uintptr_t va = pa & PHYSMM_PAGE_ADDRESS_MASK;

	va |= ARMV7_PAGE_PHYSWI2;
	armv7_special_l2->pages[ARMV7_TO_L2_IDX(ARMV7_PAGE_PHYSWI2)] = 
				ARMV7_L2_TYPE_PAGE4 |
				ARMV7_L2_PAGE4_PA(pa) | 
				ARMV7_L2_PAGE4_TEX(0) | 
				ARMV7_L2_PAGE_AP(1);

	armv7_mmu_flush_tlb_single(ARMV7_PAGE_PHYSWI2);

	return (void *) va;
}

/**
 * @brief Allocate a level 1 translation table
 * @return A pointer to a physically contiguous block of four pages OR NULL if
 * 	   not enough free memory was available
 */

armv7_l1_table_t *armv7_paging_alloc_l1( void )
{
	int cont = 1;
	physaddr_t 		 pa;
	uintptr_t		 va, va_ctr;
	armv7_l1_table_t 	*l1;
	
	/* Allocate a 16K, 16K Alligned chunk of address space */
	l1 = heapmm_alloc_alligned( 	sizeof( armv7_l1_table_t ), 
					sizeof( armv7_l1_table_t ) );

	/* Pass out of memory error up the call chain */
	if (l1 == NULL) 
		return NULL;

	/* Get the address */
	va = (uintptr_t) l1;

	/* Check if the block is physically contiguous */
	for (	va_ctr = va; 
		va_ctr < (va + sizeof( armv7_l1_table_t )); 
		va_ctr += 4096) {
		
		/* If this is the first page, get the physical address */
		if ( va_ctr == va ) {
			pa = paging_get_physical_address ( (void *) va_ctr );
			if (pa & 0x3FFF){
				cont = 0;
				break;
			}
		} else if (paging_get_physical_address ((void *) va_ctr) != pa ) {
			/* Frame did not match expected frame */
			cont = 0;
			break;
		}
		
		pa += 4096;

	}
	
	/* If the heap manager gave us a contiguous chunk of memory, accept it*/
	if ( cont )
		return l1;

	/* If not, unmap the pages and remap them from a contiguous quad frame*/

	/* Unmap the pages */
	for (	va_ctr = va; 
		va_ctr < (va + sizeof( armv7_l1_table_t )); 
		va_ctr += 4096) {

		/* Get the frame to unmap */		
		pa = paging_get_physical_address ( (void *) va_ctr );

		/* Unmap the page */
		paging_unmap( (void *) va_ctr );

		/* Free the frame */
		physmm_free_frame( pa );
		
	}

	/* Allocate a quadframe */
	pa = physmm_alloc_quadframe();

	/* Check for out of memory error */
	if ( pa == PHYSMM_NO_FRAME ) {
		paging_handle_out_of_memory();
		pa = physmm_alloc_quadframe();
	}

	/* Remap the pages */
	for (	va_ctr = va; 
		va_ctr < (va + sizeof( armv7_l1_table_t )); 
		va_ctr += 4096) {

		/* Map the page */
		paging_map( (void *) va_ctr, pa, PAGING_PAGE_FLAG_RW );

		/* Get the next of the four frames */
		pa += 4096;
		
	}

	/* Return the now contiguous level 1 table */
	return l1;
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

physaddr_t armv7_clone_l2( physaddr_t orig_pa )
{
	armv7_l2_table_t	*clone_va;
	armv7_l2_table_t	*orig_va;
	void			*src_va;
	void			*dst_va;
	physaddr_t		 clone_pa;
	physaddr_t		 src_pa;
	physaddr_t		 dst_pa;
	int			 page_ctr;
	uint32_t		 l2_entry;

	/* Allocate the clone frame */
	clone_pa = armv7_paging_alloc_l2();
	
	/* Temporarily map it into LEVEL2WIN */
	clone_va = armv7_paging_map_l2( clone_pa );
	
	/* Temporarily map original into PHYSWIN */
	orig_va = armv7_paging_map_phys( orig_pa );
	
	/* Copy level2 table over */
	memcpy( clone_va, orig_va, sizeof(armv7_l2_table_t) );
	
	/* NOTE: At this point we do not need orig anymore, so PHYSWIN is free*/
	
	/* Iterate over entries */
	for (page_ctr = 0; page_ctr < ARMV7_L2_TABLE_SIZE; page_ctr++) {
		/* Get the entry */
		l2_entry = clone_va->pages[ page_ctr ];

		/* Handle types */
		switch ( l2_entry & 3 ) {
			case ARMV7_L2_TYPE_PAGE4:
			case ARMV7_L2_TYPE_PAGE4_XN:
				if ( l2_entry & ARMV7_L2_PAGE_NOT_GLOBAL) {
					/* NOT GLOBAL, Clone the frame */

					/* Get the physical addr of the src fr*/
					src_pa = ARMV7_L2_PAGE4_PA( l2_entry );

					/* Allocate the cloned frame */
					dst_pa = physmm_alloc_frame();
					if ( dst_pa == PHYSMM_NO_FRAME ) {
						paging_handle_out_of_memory();
						dst_pa = physmm_alloc_frame();
					}
					earlycon_printf("cloning frame:0x%x to 0x%x\n", src_pa, dst_pa);	
					/* Map the source and dest */
					src_va = armv7_paging_map_phys(src_pa);
					dst_va = armv7_paging_map_phys2(dst_pa);
					
					/* Source & Dest on PHYSWIN & PHYSWI2 */
					memcpy(dst_va, src_va,PHYSMM_PAGE_SIZE);
		
					/* Update the entry */
					l2_entry &=
						 ~ARMV7_L2_PAGE4_PA(0xFFFFFFFF);
					l2_entry|=ARMV7_L2_PAGE4_PA(dst_pa);				

				}
				break;
			default:
				//TODO: Support 64KB Pages
				l2_entry = ARMV7_L2_TYPE_FAULT;
			case ARMV7_L2_TYPE_FAULT:
				continue;

		}
		clone_va->pages[page_ctr] = l2_entry;
	}

	/* Return the cloned L2 table */
	return clone_pa;
}

page_dir_t	*paging_create_dir()
{
	uint32_t	  	 l1_entry, l1_ptr;
	physaddr_t	  	 nlevel1_pa, nlevel2_pa;
	physaddr_t	  	 clevel2_pa;
	armv7_l1_table_t 	*nlevel1_va;
	armv7_l1_table_t 	*clevel1_va;
	page_dir_t	 	*page_dir;
	armv7_l1_table_list_t	*list_entry;

	assert ( paging_active_dir != NULL);

	clevel1_va = (armv7_l1_table_t *) paging_active_dir->content;

	assert ( clevel1_va != NULL);

	nlevel1_va = armv7_paging_alloc_l1();
	nlevel1_pa = paging_get_physical_address ( nlevel1_va );
	assert( nlevel1_pa != 0 );

	memset(nlevel1_va, 0, sizeof(armv7_l1_table_t));

	for ( l1_ptr = 0; l1_ptr < ARMV7_L1_TABLE_SIZE-1; l1_ptr++) {
		l1_entry = clevel1_va->entries[ l1_ptr ];
		switch ( l1_entry & 3 ) {
			case ARMV7_L1_TYPE_FAULT:
				nlevel1_va->entries[ l1_ptr ] = l1_entry;
				continue;
			case ARMV7_L1_TYPE_PAGE_TABLE:
				clevel2_pa  = ARMV7_L1_PAGE_TABLE_PA(l1_entry);
				earlycon_printf("cloning ptable[%i] 0x%x at 0x%x\n",l1_ptr, l1_entry, clevel2_pa);
				nlevel2_pa  = armv7_clone_l2(clevel2_pa);
				l1_entry &= ~ARMV7_L1_PAGE_TABLE_PA(0xFFFFFFFF);
				l1_entry |=  ARMV7_L1_PAGE_TABLE_PA(nlevel2_pa);
				nlevel1_va->entries[ l1_ptr ] = l1_entry;
			case ARMV7_L1_TYPE_SECTION:
				nlevel1_va->entries[ l1_ptr ] = l1_entry;
				break;
		}
	}

	nlevel1_va->entries[4095] = clevel1_va->entries[4095];

	page_dir = heapmm_alloc(sizeof(page_dir));
	
	assert(page_dir != NULL);
	
	page_dir->content = nlevel1_va;
	
	list_entry = heapmm_alloc(sizeof(armv7_l1_table_list_t));
	
	list_entry->dir = page_dir;

	llist_add_end(&armv7_l1_table_list, (llist_t *)list_entry);

	return page_dir;
}

void		paging_free_dir(page_dir_t *dir)
{
}

void armv7_paging_map(	armv7_l1_table_t *level1_va,
			void * virt_addr, 
			physaddr_t phys_addr, 
			page_flags_t flags)
{
	uint32_t l1_entry, ap, ng;
	physaddr_t level2_pa;
	uintptr_t va = (uintptr_t) virt_addr;
	armv7_l2_table_t *level2_va;

	l1_entry = level1_va->entries[ARMV7_TO_L1_IDX(va)];

	if ((l1_entry & 3) == ARMV7_L1_TYPE_FAULT) {

		level2_pa = armv7_paging_alloc_l2();

		level2_va = armv7_paging_map_l2(level2_pa);

		memset( (void *) level2_va, 
			0, 
			ARMV7_L2_TABLE_SIZE * sizeof(uint32_t));

		l1_entry = 	ARMV7_L1_TYPE_PAGE_TABLE |
				ARMV7_L1_PAGE_TABLE_PA(level2_pa);

		level1_va->entries[ARMV7_TO_L1_IDX(va)] = l1_entry;
	} else {
		level2_pa = ARMV7_L1_PAGE_TABLE_PA(l1_entry);
		level2_va = armv7_paging_map_l2(level2_pa);
	}

	if (flags & PAGING_PAGE_FLAG_USER) {
		if (flags & PAGING_PAGE_FLAG_RW) 
			ap = ARMV7_AP_PRIV_RW_USR_RW;
		else
			ap = ARMV7_AP_PRIV_RW_USR_RO;
		ng = ARMV7_L2_PAGE_NOT_GLOBAL;
	} else {
		if (flags & PAGING_PAGE_FLAG_RW) 
			ap = ARMV7_AP_PRIV_RW_USR_NA;
		else
			ap = ARMV7_AP_PRIV_RW_USR_NA;//Consistency with i386
		ng = 0;
		//TODO: Check if this is correct
						
	}

	level2_va->pages[ARMV7_TO_L2_IDX(va)] = 
		ARMV7_L2_TYPE_PAGE4 |
		ARMV7_L2_PAGE_AP(ap) |
		ng |
		ARMV7_L2_PAGE4_PA(phys_addr);
}

typedef struct i386_set_pd_param {
	void		*va;
	physaddr_t	 pa;
	page_flags_t	 fl;
} armv7_map_param_t;

int paging_map_iterator (llist_t *node, void *param)
{
	armv7_l1_table_t *dir = (armv7_l1_table_t *)
				((armv7_l1_table_list_t *) node)->dir->content;
	armv7_map_param_t *p = (armv7_map_param_t *) param;
	
	armv7_paging_map( dir, p->va, p->pa, p->fl );

	return 0;
}

void paging_map(void * virt_addr, physaddr_t phys_addr, page_flags_t flags)
{
	armv7_l1_table_t *level1_va;
	armv7_map_param_t param;

	level1_va = (armv7_l1_table_t *) paging_active_dir->content;

	if (flags & PAGING_PAGE_FLAG_USER) {
		armv7_paging_map(level1_va, virt_addr, phys_addr, flags);
	} else {
		param.va = virt_addr;
		param.pa = phys_addr;
		param.fl = flags;
		llist_iterate_select(	&armv7_l1_table_list, 
					&paging_map_iterator, 
					(void *) &param);									
	}
	
	armv7_mmu_flush_tlb_single( (uint32_t) virt_addr );
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
	uint32_t par, l1_idx,l1_entry,l2_idx,l2_entry;
	uintptr_t va;
	uintptr_t pa;
	armv7_l1_table_t *l1_table;
	armv7_l2_table_t *level2_va;
	physaddr_t level2_pa;
	va = (uintptr_t) virt_addr;

	par = armv7_mmu_translate(va, ARMV7_TRANSLATE_OP_PRIV_R);

	if ( par & ARMV7_PAR_FAULT )
		return 0;
	if (par == 0) {
		l1_table = (armv7_l1_table_t *) paging_active_dir->content;
		l1_idx = ARMV7_TO_L1_IDX(virt_addr);
		l2_idx = ARMV7_TO_L2_IDX(virt_addr);
		l1_entry = l1_table->entries[l1_idx];
		if ((l1_entry & 3) == ARMV7_L1_TYPE_FAULT)
			return 0;
		level2_pa = ARMV7_L1_PAGE_TABLE_PA(l1_entry);
		assert ( (l1_entry & 3) == ARMV7_L1_TYPE_PAGE_TABLE);
		level2_va = armv7_paging_map_l2(level2_pa);
		l2_entry = level2_va->pages[l2_idx];
		if ((l2_entry & 3) == ARMV7_L2_TYPE_FAULT)
			return 0;
	
		par = ARMV7_L2_PAGE4_PA(l2_entry);
	}
	pa  = ARMV7_PAR_PA(par);
	pa |= va & PHYSMM_PAGE_ADDRESS_MASK;
//	earlycon_printf("par: 0x%x, pa: 0x%x\n", par, pa);
	return pa;
}


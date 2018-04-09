/**
 * arch/i386/paging.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#include "arch/i386/paging.h"
#include "util/llist.h"
#include "kernel/heapmm.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "config.h"
#include <string.h>
#include <assert.h>

llist_t			i386_page_directory_list;
i386_page_dir_list_t	i386_page_dir_list_initial;
page_dir_t 		i386_page_dir_initial;

uint32_t i386_force_order;

/**
 * Taken from Linux source
 */
static inline void i386_native_flush_tlb_single(uintptr_t addr)
{
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

/**
 * Taken from Linux source
 */
static inline void i386_native_write_cr3(physaddr_t val)
{
	asm volatile("mov %0,%%cr3": : "r" (val), "m" (i386_force_order));
}

void paging_switch_dir(page_dir_t *new_dir)
{ 
	if ( paging_active_dir == new_dir )
		return;
	paging_active_dir = new_dir;	
	i386_native_write_cr3(paging_get_physical_address(new_dir->content));
}

void *i386_alloc_page() {
	if (paging_active_dir != NULL)
		return heapmm_alloc_page();
	else
		return (void *) (0xC0000000 + physmm_alloc_frame());
}

page_dir_t *paging_create_dir()
{
	i386_page_dir_t *active_dir;
	i386_page_dir_t *dir = (i386_page_dir_t *) i386_alloc_page();
	i386_page_dir_list_t *list_node = (paging_active_dir == NULL) ? (&i386_page_dir_list_initial) :
			 ((i386_page_dir_list_t *) heapmm_alloc(sizeof(i386_page_dir_list_t)));
	page_dir_t *dir_w = (paging_active_dir == NULL) ? (&i386_page_dir_initial) :
			 ((page_dir_t *) heapmm_alloc(sizeof(page_dir_t)));
	i386_page_table_t *new_table_ptr;
	i386_page_table_t *table_ptr;
	physaddr_t table_phys, new_table_phys, frame_phys, new_frame_phys;
	uintptr_t copy_counter, frame_counter;

	process_mmap_t *region;

	memset(dir, 0, sizeof(i386_page_dir_t));

	if (paging_active_dir != NULL){
		active_dir = (i386_page_dir_t *) paging_active_dir->content;
		new_table_ptr = (i386_page_table_t *) heapmm_alloc_page();
		table_phys = paging_get_physical_address(new_table_ptr);
		dir->directory[0] = active_dir->directory[0];
		for (copy_counter = 0x300; copy_counter < 0x3FF; copy_counter++)
			dir->directory[copy_counter] = active_dir->directory[copy_counter];

		for (copy_counter = 1; copy_counter < 0x300; copy_counter++) {
			if (active_dir->directory[copy_counter] & I386_PAGE_FLAG_PRESENT) {
				//earlycon_printf("Copying table : %i ,%x, %x, %x,%x", copy_counter,active_dir,active_dir->directory[copy_counter],dir_w,paging_active_dir);

				/* Table is present, copy it */
				new_table_phys = physmm_alloc_frame();
				if (new_table_phys == PHYSMM_NO_FRAME) {
					paging_handle_out_of_memory();
					new_table_phys = physmm_alloc_frame();
				}
				paging_map(new_table_ptr, new_table_phys, I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
				table_ptr = I386_PTADDR(copy_counter);

				for (frame_counter = 0; frame_counter < 1024; frame_counter++) {
					if (table_ptr->pages[frame_counter] & I386_PAGE_FLAG_PRESENT){
						region = procvmm_get_memory_region((void *)((copy_counter << 22) | (frame_counter << 12)));
						if (region && (region->flags & PROCESS_MMAP_FLAG_PUBLIC)) {
							new_table_ptr->pages[frame_counter] = table_ptr->pages[frame_counter];
						} else {
							new_frame_phys = physmm_alloc_frame();
							if (new_frame_phys == PHYSMM_NO_FRAME) {
								paging_handle_out_of_memory();
								new_frame_phys = physmm_alloc_frame();
							}
							frame_phys = (physaddr_t)(table_ptr->pages[frame_counter] & ~0xFFF);
							i386_phys_copy_frame(frame_phys, new_frame_phys);
							new_table_ptr->pages[frame_counter] = (table_ptr->pages[frame_counter] & 0xFFF) | new_frame_phys;
						}
					} else
						new_table_ptr->pages[frame_counter] = 0;
				}

				dir->directory[copy_counter] = (active_dir->directory[copy_counter] & 0xFFF) | new_table_phys;
			} else {
				dir->directory[copy_counter] = 0;
			}
			
		}
		paging_map(new_table_ptr, table_phys, I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
		heapmm_free(new_table_ptr, PHYSMM_PAGE_SIZE);
	}
	dir->directory[0x3FF] = ((uint32_t) paging_get_physical_address(dir)) | I386_PAGE_FLAG_RW | I386_PAGE_FLAG_NOCACHE | I386_PAGE_FLAG_PRESENT;
	llist_add_end(&i386_page_directory_list, (llist_t *) list_node);
	list_node->dir = dir_w;
	dir_w->content = (void *) dir;
	dir_w->content_phys = paging_get_physical_address(dir);
	return dir_w;
}
void paging_free_dir(page_dir_t *dir) //TODO: Implement
{
	i386_page_dir_t *pdir = (i386_page_dir_t *) dir->content;
	i386_page_dir_list_t *list;
	uintptr_t copy_counter;

	for (copy_counter = 1; copy_counter < 0x300; copy_counter++) {
		if ( !( pdir->directory[copy_counter] & I386_PAGE_FLAG_PRESENT ) )
			continue;
		physmm_free_frame( pdir->directory[copy_counter] & 0xFFFFF000 );
	}

	for (	list = (i386_page_dir_list_t*)i386_page_directory_list.next;
			( (llist_t *) list ) != &i386_page_directory_list;
			list = (i386_page_dir_list_t*) list->node.next )
		if ( list->dir == dir )
			break;
	assert ( ( (llist_t *) list ) != &i386_page_directory_list );
	llist_unlink( (llist_t*) list);
	heapmm_free( pdir, sizeof(i386_page_dir_t) );
	heapmm_free( dir, sizeof(page_dir_t));
	heapmm_free( list, sizeof(i386_page_dir_list_t));
}

typedef struct i386_set_pd_param {
	uintptr_t	   index;
	uint32_t	   value;
} i386_set_pd_param_t;

int paging_set_pd_iterator (llist_t *node, void *param)
{
	i386_page_dir_t *dir = (i386_page_dir_t *)((i386_page_dir_list_t *) node)->dir->content;
	i386_set_pd_param_t *p = (i386_set_pd_param_t *) param;
	dir->directory[p->index] = p->value;
	return 0;
}

void paging_map(void * virt_addr, physaddr_t phys_addr, page_flags_t flags)
{
	uint32_t pd_entry, pt_entry;
	physaddr_t table_addr;
	i386_page_table_t *table;
	i386_set_pd_param_t param;
	uintptr_t pt_idx = I386_ADDR_TO_PT_IDX(virt_addr);
	uintptr_t pd_idx = I386_ADDR_TO_PD_IDX(virt_addr);
	i386_page_dir_t *page_dir = (i386_page_dir_t *) paging_active_dir->content;

	if (page_dir->directory[pd_idx] == 0) {
		/* Page table does not yet exist */
		table_addr = physmm_alloc_frame();
		if (table_addr == PHYSMM_NO_FRAME) {
			paging_handle_out_of_memory();
			table_addr = physmm_alloc_frame();
		}
		pd_entry = I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT;
		pd_entry |= table_addr;
		table = (i386_page_table_t *) (((uintptr_t)I386_ADDR_TO_PTEPTR(virt_addr)) & 0xFFFFF000);	

		if (!(flags & PAGING_PAGE_FLAG_USER)) {
			/* Kernel page! Need to add this table to all directories */
			pd_entry |= I386_PAGE_FLAG_GLOBAL;
			param.index = pd_idx;
			param.value = pd_entry;
			llist_iterate_select(&i386_page_directory_list, &paging_set_pd_iterator, (void *) &param);
		} else {
			/* User page, Just add it to the active directory */
			pd_entry |= I386_PAGE_FLAG_USER;
			page_dir->directory[pd_idx] = pd_entry;
		}
		i386_native_flush_tlb_single((uintptr_t)I386_ADDR_TO_PTEPTR(virt_addr));
		memset(table, 0, sizeof(i386_page_table_t));
	} else {
		table = (i386_page_table_t *) (((uintptr_t)I386_ADDR_TO_PTEPTR(virt_addr)) & 0xFFFFF000);
	}

	if (table->pages[pt_idx] & I386_PAGE_FLAG_PRESENT) {
		/* Tried to map something that was already mapped */
		/* We will allow this but maybe warn here */
		//TODO: Determine whether we have to free previously mapped frame
		table->pages[pt_idx] = 0;
	}
	
	pt_entry = I386_PAGE_FLAG_PRESENT;
	if (flags & PAGING_PAGE_FLAG_USER)
		pt_entry |= I386_PAGE_FLAG_USER;
	else
		pt_entry |= I386_PAGE_FLAG_GLOBAL;
	if (flags & PAGING_PAGE_FLAG_NOCACHE)
		pt_entry |= I386_PAGE_FLAG_NOCACHE;
	if (flags & PAGING_PAGE_FLAG_RW)
		pt_entry |= I386_PAGE_FLAG_RW;
	table->pages[pt_idx] = pt_entry | ((uint32_t) (phys_addr & 0xFFFFF000));
	i386_native_flush_tlb_single((uintptr_t)virt_addr);
}

void paging_unmap(void * virt_addr)
{
	uint32_t *pt_entry = I386_ADDR_TO_PTEPTR(virt_addr);	
	*pt_entry &= ~I386_PAGE_FLAG_PRESENT;
	i386_native_flush_tlb_single((uintptr_t)virt_addr);
}

void paging_tag(void * virt_addr, page_tag_t tag)
{
	uint32_t *pt_entry = I386_ADDR_TO_PTEPTR(virt_addr);	
	*pt_entry &= ~0xE00;
	*pt_entry |= ( ((uint32_t)tag) << 9) & 0xE00;	
}

page_tag_t paging_get_tag(void * virt_addr)
{
	uint32_t *pt_entry = I386_ADDR_TO_PTEPTR(virt_addr);	
	return (page_tag_t) (((*pt_entry) & 0xE00) >> 9);	
}

void i386_paging_enable()
{
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}

void i386_init_switch_gdt(void);

void paging_init()
{	
	physaddr_t map_addr;
	page_dir_t *pdir;
	i386_page_dir_t *dir;
	i386_page_table_t *initial_table = (i386_page_table_t *) i386_alloc_page();

	llist_create(&i386_page_directory_list);
	debugcon_printf("\nInitial table: %x\n", initial_table);
	
	pdir = paging_create_dir();
	dir = (i386_page_dir_t *) pdir->content;
	debugcon_printf("\nInitial dir: %x\n", dir);
	debugcon_printf("\nInitial dir: %x\n", dir->directory);

	dir->directory[0x3FF] = (((uint32_t) dir) - 0xC0000000) | I386_PAGE_FLAG_RW | I386_PAGE_FLAG_NOCACHE | I386_PAGE_FLAG_PRESENT;
	
	memset(initial_table, 0, sizeof(i386_page_table_t));
	
	for ( map_addr = 0; map_addr < 0x400000;map_addr += 4096 ) { //Map the first 4MB
		initial_table->pages[I386_ADDR_TO_PT_IDX(map_addr)] 
			= ((uint32_t)map_addr) | I386_PAGE_FLAG_PRESENT | I386_PAGE_FLAG_GLOBAL | I386_PAGE_FLAG_RW | I386_PAGE_FLAG_USER;
	}
	/* Identity map the first 4 MB */
	dir->directory[0x000] = (((uint32_t) initial_table) - 0xC0000000) | I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT | I386_PAGE_FLAG_GLOBAL | I386_PAGE_FLAG_USER;
	/* Map the first 4 MB to 3GB too */
	dir->directory[0x300] = (((uint32_t) initial_table) - 0xC0000000) | I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT | I386_PAGE_FLAG_GLOBAL | I386_PAGE_FLAG_USER;
	/* Activate new directory */		
	paging_active_dir = pdir;	
	i386_native_write_cr3( ((uint32_t) dir) - 0xC0000000);
	/* Activate paging */
	i386_paging_enable();
	/* Undo GDT trick */
	i386_init_switch_gdt();
	pdir->content = (void *)0xFFFFF000;
}

uintptr_t paging_get_physical_address_other(page_dir_t *dir,void * virt_addr) {
	i386_page_dir_t *page_dir = (i386_page_dir_t *) dir->content;
	uintptr_t pd_idx = I386_ADDR_TO_PD_IDX(virt_addr);
	uintptr_t pt_idx = I386_ADDR_TO_PT_IDX(virt_addr);
	uint32_t pd_entry = page_dir->directory[pd_idx];
	physaddr_t pt_o;
	i386_page_table_t *pt;	
	uint32_t pt_v;
	if (pd_entry & I386_PAGE_FLAG_PRESENT) {
		pt = heapmm_alloc_page();	
		pt_o = paging_get_physical_address( pt );
		paging_map( pt, pd_entry & 0xFFFFF000, I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
		pt_v = pt->pages[pt_idx];
	
		paging_map( pt, pt_o, I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
		heapmm_free( pt, 4096);
		if (pt_v & I386_PAGE_FLAG_PRESENT)
			return (pt_v & 0xFFFFF000) | ((uintptr_t) virt_addr & 0xFFF);
		else
			return 0;
	} else
		return 0;
}

uintptr_t paging_get_physical_address(void * virt_addr) {
	if ( paging_active_dir == NULL )
		return paging_active_dir - 0xc0000000;
	i386_page_dir_t *page_dir = (i386_page_dir_t *) paging_active_dir->content;
	uintptr_t pd_idx = I386_ADDR_TO_PD_IDX(virt_addr);
	uint32_t *pt_entry = I386_ADDR_TO_PTEPTR(virt_addr);
	uint32_t pd_entry = page_dir->directory[pd_idx];
	if ((pd_entry) & I386_PAGE_FLAG_PRESENT) {
		if ((*pt_entry) & I386_PAGE_FLAG_PRESENT)
			return ((*pt_entry) & 0xFFFFF000) | ((uintptr_t) virt_addr & 0xFFF);
		else
			return 0;
	} else
		return 0;
}

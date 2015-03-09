/**
 * kernel/paging.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#ifndef __KERNEL_PAGING_H__
#define __KERNEL_PAGING_H__

#include "kernel/physmm.h"
#include <stdint.h>

typedef uint_fast16_t		page_flags_t;
typedef uint_fast8_t		page_tag_t;
typedef struct page_dir		page_dir_t;

struct page_dir {
	//TODO: Determine what else to add here
	void		*content;
	phys_addr_t	 content_phys;
};

#define PAGING_PAGE_FLAG_USER		(0x1)
#define PAGING_PAGE_FLAG_RW		(0x2)
#define PAGING_PAGE_FLAG_NOCACHE	(0x4)

#define PAGING_PAGE_TAG_KERNEL_DATA	(0)
#define PAGING_PAGE_TAG_KERNEL_CODE	(1)
#define PAGING_PAGE_TAG_MM_HARDWARE	(2)
#define PAGING_PAGE_TAG_DMA_BUFFER	(3)
#define PAGING_PAGE_TAG_USER_DATA	(4)
#define PAGING_PAGE_TAG_USER_CODE	(5)
#define PAGING_PAGE_TAG_USER_MMAP	(6)


extern page_dir_t *paging_active_dir;

void		paging_switch_dir(page_dir_t *new_dir);

page_dir_t	*paging_create_dir();

void		paging_free_dir(page_dir_t *dir);

void		paging_map(void * virt_addr, physaddr_t phys_addr, page_flags_t flags);

void		paging_unmap(void * virt_addr);

page_tag_t	paging_get_tag(void * virt_addr);

void		paging_tag(void * virt_addr, page_tag_t tag);

void		paging_init();

void		paging_handle_out_of_memory();

void		paging_handle_fault(void *virt_addr, void * instr_ptr, void *state, size_t state_size
								, int present, int write, int user);

int		paging_soft_check(void *virt_addr, size_t size);

uintptr_t	paging_get_physical_address(void * virt_addr);

#endif

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
	physaddr_t	 content_phys;
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

typedef struct {
	void *     virt;
	size_t     size;
	physaddr_t phys;
	int        flags;
	void *     map_start;
	size_t     map_size;
} physmap_t;

extern page_dir_t *paging_active_dir;

void		paging_switch_dir(page_dir_t *new_dir);

page_dir_t	*paging_create_dir(void);

void		paging_free_dir(page_dir_t *dir);

void		paging_map(void * virt_addr, physaddr_t phys_addr, page_flags_t flags);

void		paging_unmap(void * virt_addr);

page_tag_t	paging_get_tag(const void * virt_addr);

void		paging_tag(void * virt_addr, page_tag_t tag);

void		paging_init(void);

void		paging_handle_out_of_memory();

void		paging_handle_fault(void *virt_addr, void * instr_ptr
								, int present, int write, int user);

int		paging_soft_check(void *virt_addr, size_t size);

uintptr_t	paging_get_physical_address(const void * virt_addr);

uintptr_t paging_get_physical_address_other(page_dir_t *dir,const void * virt_addr);

physmap_t *paging_map_phys_range( physaddr_t addr, size_t size, int flags );

int paging_unmap_phys_range( physmap_t *map );

#endif

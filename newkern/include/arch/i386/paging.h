/**
 * arch/i386/paging.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#ifndef __ARCH_I386_PAGING_H__
#define __ARCH_I386_PAGING_H__

#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "util/llist.h"

#define I386_PAGE_FLAG_PRESENT		  (0x1)
#define I386_PAGE_FLAG_RW		  (0x2)
#define I386_PAGE_FLAG_USER		  (0x4)
#define I386_PAGE_FLAG_WRITETHROUGH	  (0x8)
#define I386_PAGE_FLAG_NOCACHE		 (0x10)
#define I386_PAGE_FLAG_ACCESSED		 (0x20)
#define I386_PAGE_FLAG_DIRTY		 (0x40)
#define I386_PAGE_FLAG_PAGE_SIZE	 (0x80)
#define I386_PAGE_FLAG_GLOBAL		(0x100)

#define I386_TABLE_TAG_RESERVED		(0x200)

#define I386_PAGE_DIRECTORY_SIZE	(1024)
#define I386_PAGE_TABLE_SIZE		(1024)

typedef struct i386_page_dir	i386_page_dir_t;
typedef struct i386_page_table	i386_page_table_t;
typedef struct i386_page_dir_list	i386_page_dir_list_t;

struct i386_page_dir {
	uint32_t		directory[I386_PAGE_DIRECTORY_SIZE];
};

struct i386_page_table {
	uint32_t		pages[I386_PAGE_TABLE_SIZE];
};

struct i386_page_dir_list {
	llist_t		node;
	page_dir_t *dir;
};

#define I386_ADDR_TO_PT_IDX(a)		( (((uintptr_t)a) & 0x003FF000) >> 12 )
#define I386_ADDR_TO_PD_IDX(a)		( (((uintptr_t)a) & 0xFFC00000) >> 22 )

#define I386_ADDR_TO_PTEPTR(a)	((uint32_t *)(0xFFC00000 | \
				( ( ((uintptr_t)a) >> 10) & 0x3FF000 ) | (I386_ADDR_TO_PT_IDX(a)<<2)))

#define I386_PTADDR(a)	((i386_page_table_t *)(0xFFC00000 | \
				( ( ((uintptr_t)a) << 12) & 0x3FF000 )))

void i386_phys_copy_frame(physaddr_t source, physaddr_t dest);

#endif

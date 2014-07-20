/**
 * kernel/procvmm.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-04-2014 - Created
 */

#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/earlycon.h"
#include <string.h>
#include <sys/errno.h>

void procvmm_clear_mmaps()
{	
	process_mmap_t *region;
	for (	region = (process_mmap_t *) llist_get_last(scheduler_current_task->memory_map);
		region != (process_mmap_t *) scheduler_current_task->memory_map;
		region = (process_mmap_t *) llist_get_last(scheduler_current_task->memory_map))
		procvmm_unmmap(region);
}

int procvmm_do_exec_mmaps()
{
	scheduler_current_task->heap_start = (void *) scheduler_current_task->image_end;     //End of program image_base
	scheduler_current_task->heap_end   = scheduler_current_task->heap_start;	
	scheduler_current_task->heap_max = (void *) 0x40000000;     //User mmap area starts here
	scheduler_current_task->stack_bottom = (void *) 0xBFBFEFFF; //Start of kernel stack area etc etc etc
	scheduler_current_task->stack_top = (void *) 0xBF400000; //TODO: Implement dynamic stack size
	procvmm_mmap_anon((void *) 0xBFBFF000, 0x1000, PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK, "(sigstack)");
	return procvmm_mmap_anon(scheduler_current_task->stack_top, 0x7FEFFF, PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK, "(stack)");
}

int procvmm_check(void *dest, size_t size) {
	uintptr_t p = ((uintptr_t)dest) & ~PHYSMM_PAGE_ADDRESS_MASK;
	if (scheduler_current_task->pid == 0)
		return 1;
	for (; p < size; p += PHYSMM_PAGE_SIZE)
		if (!procvmm_get_memory_region((void *) p))
			return 0;
	return 1;
}

/** 
 * Iterator function that finds the mmap for the given address
 */
int procvmm_get_mmap_iterator (llist_t *node, void *param)
{
	process_mmap_t *m = (process_mmap_t *) node;
	uintptr_t p = (uintptr_t) param;
	uintptr_t b_s = (uintptr_t) m->start;
	uintptr_t b_e = b_s + m->size;
	return (p >= b_s) && (p < b_e);
}

process_mmap_t *procvmm_get_memory_region(void *address)
{
	if (!scheduler_current_task)
		return 0;
	return (process_mmap_t *) llist_iterate_select(scheduler_current_task->memory_map, &procvmm_get_mmap_iterator, address);
}

int procvmm_mmap_copy_iterator (llist_t *node, void *param)
{
	llist_t *table = (llist_t *) param;
	process_mmap_t *m = (process_mmap_t *) node;
	process_mmap_t *n;
	n = heapmm_alloc(sizeof(process_mmap_t));
	if (!n) {
		return 1;
	}
	if ((m->flags & PROCESS_MMAP_FLAG_FILE) && m->file)		
		m->file->usage_count++;
	n->flags = m->flags;
	n->start = m->start;
	n->size = m->size;
	n->file = m->file;
	n->file_sz = m->file_sz;
	n->offset = m->offset;
	n->name = heapmm_alloc(strlen(m->name)+1);
	strcpy(n->name, m->name);
	llist_add_end(table, (llist_t *) n);	
	return 0;
}

int procvmm_copy_memory_map (llist_t *target)
{
	return llist_iterate_select(scheduler_current_task->memory_map, &procvmm_mmap_copy_iterator, (void *) target) == NULL;
}

//A         b                C             d
//b           A              d            C
//b           A              C		   d

/** 
 * Iterator function that finds the mmap for the given address
 */
int procvmm_collcheck_iterator (llist_t *node, void *param)
{
	process_mmap_t *m = (process_mmap_t *) node;
	process_mmap_t *m2 = (process_mmap_t *) param;
	uintptr_t b_s = (uintptr_t) m->start;
	uintptr_t b_e = b_s + m->size;
	uintptr_t c_s = (uintptr_t) m2->start;
	uintptr_t c_e = c_s + m2->size;
	if (m == m2)
		return 0;
	return ((c_s >= b_s) && (c_s < b_e)) || ((c_e >= b_s) && (c_e < b_e)) || 
	       ((b_s >= c_s) && (b_s < c_e)) || ((b_e >= c_s) && (b_e < c_e));
}

int procvmm_resize_map(void *start, size_t newsize)
{
	size_t oldsize;
	process_mmap_t *region = procvmm_get_memory_region(start);
	if (!region)
		return 1;
	oldsize = region->size;
	region->size = newsize;
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		region->size = oldsize;
		return EINVAL;
	}
	return 0;
}

int procvmm_mmap_anon(void *start, size_t size, int flags, char *name)
{
	process_mmap_t *region;
	uintptr_t in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;
	if (in_page) {
		start = (void*)(((uintptr_t)start) & ~PHYSMM_PAGE_ADDRESS_MASK);
		size += (size_t) in_page;
	}
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;
	region = heapmm_alloc(sizeof(process_mmap_t));
	if (!region)
		return ENOMEM;
	if (name == NULL)
		name = "(anon)";
	region->name = heapmm_alloc(strlen(name)+1);
	strcpy(region->name, name);
	region->start = start;
	region->size = size;
	region->flags = flags & ~PROCESS_MMAP_FLAG_FILE;
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		heapmm_free(region->name, strlen(name)+1);
		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;
	}
	llist_add_end(scheduler_current_task->memory_map, (llist_t *)region);
	return 0;
}

int procvmm_mmap_file(void *start, size_t size, inode_t* file, off_t offset, off_t file_sz, int flags, char *name)
{
	uintptr_t in_page;
	process_mmap_t *region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;
	region = heapmm_alloc(sizeof(process_mmap_t));
	if (!region)
		return ENOMEM;
	if (name == NULL)
		name = file->name;
	if (file_sz == -1)
		file_sz = file->size;
	in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;
	if (in_page) {
		start = (void*)(((uintptr_t)start) & ~PHYSMM_PAGE_ADDRESS_MASK);
		if (offset < (off_t) in_page)
			return EINVAL; 
		offset -= (off_t) in_page;
		size += in_page;
		file_sz += in_page;
	}
	region->name = heapmm_alloc(strlen(name)+1);
	strcpy(region->name, name);
	region->start = start;
	region->size = size;
	region->file = file;
	region->offset = offset;
	region->flags = flags | PROCESS_MMAP_FLAG_FILE;
	region->file_sz = file_sz;
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		heapmm_free(region->name, strlen(name)+1);
		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;
	}
	file->usage_count++;
	llist_add_end(scheduler_current_task->memory_map, (llist_t *)region);
	return 0;
}

void procvmm_unmmap(process_mmap_t *region)
{
	uintptr_t start = (uintptr_t) region->start;
	uintptr_t page;
	physaddr_t frame;
	llist_unlink((llist_t *) region);
	for (page = start; page < (start +region->size); page += PHYSMM_PAGE_SIZE) {
		frame = paging_get_physical_address((void *)page);
		if (frame) {
			physmm_free_frame(frame);
			paging_unmap((void *) page);
		}
	}
	if (region->flags & PROCESS_MMAP_FLAG_FILE) {
		region->file->usage_count--;
	}
	heapmm_free(region->name, strlen(region->name) + 1);
	heapmm_free(region, sizeof(process_mmap_t));		
}

int procvmm_handle_fault(void *address)
{//TODO: Implement public mappings
	page_flags_t flags = PAGING_PAGE_FLAG_USER;
	uintptr_t in_region;
	aoff_t file_off;
	size_t read_size = PHYSMM_PAGE_SIZE;
	size_t rd_count = 0;
	physaddr_t frame;
	process_mmap_t *region = procvmm_get_memory_region(address);
	int status;
	address = (void *) (((uintptr_t)address) & ~PHYSMM_PAGE_ADDRESS_MASK);
	if (!region)
		return 0;
	frame = physmm_alloc_frame();
	if (!frame)
		return 0;
	if (region->flags & PROCESS_MMAP_FLAG_WRITE)
		flags |= PAGING_PAGE_FLAG_RW;
	paging_map(address, frame, flags);
	memset(address, 0, read_size);
	if (region->flags & PROCESS_MMAP_FLAG_FILE) {
		if (!(region->file)) {
			paging_unmap(address);
			physmm_free_frame(frame);
			return 0;
		}
		in_region = ((uintptr_t) address) - ((uintptr_t) region->start);
		file_off = region->offset + ((off_t) in_region);
		if ((in_region < (uintptr_t) region->file_sz) && (file_off < region->file->size)) {
			if ((in_region + read_size) > (uintptr_t) region->file_sz)
				read_size = (size_t) (region->file_sz - in_region);
			status = vfs_read(region->file, file_off, address, read_size, &rd_count, 0);
			if (status) {
				paging_unmap(address);
				physmm_free_frame(frame);
				return 0;
			}
			if (rd_count != read_size) {
				paging_unmap(address);
				physmm_free_frame(frame);
				return 0;				
			}
		}		
	}
	return 1;
	
}

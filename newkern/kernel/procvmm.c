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
#include "kernel/streams.h"
#include "kernel/vfs.h"
#include "kernel/device.h"
#include "kernel/syscall.h"
#include "kernel/shm.h"
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>

void procvmm_clear_mmaps()
{	
	process_mmap_t *region;
	for (	region  = (process_mmap_t *) 
				llist_get_last( scheduler_current_task->memory_map );
			region != NULL;
			region  = (process_mmap_t *) 
				llist_get_last(scheduler_current_task->memory_map))
		procvmm_unmmap(region);
}

int procvmm_do_exec_mmaps()
{
	scheduler_current_task->heap_start	=
		(void *) scheduler_current_task->image_end;//End of program image_base
	scheduler_current_task->heap_end	= scheduler_current_task->heap_start;	
	scheduler_current_task->heap_max	= 
				(void *) 0x40000000; //User mmap area starts here
	scheduler_current_task->stack_bottom = 
				(void *) 0xBFBFEFFF; //Start of kernel stack area etc etc etc
	scheduler_current_task->stack_top	= 
				(void *) 0xBF400000; //TODO: Implement dynamic stack size
	procvmm_mmap_anon(
				(void *) 0xBFBFF000, 
				0x1000,
				PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK, 
				"(sigstack)" );
				
	return procvmm_mmap_anon(
				scheduler_current_task->stack_top, 
				0x7FEFFF, 
				PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK, 
				"(stack)" );
}

int procvmm_check( const void *dest, size_t size) {
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
		n->file = vfs_inode_ref( m->file );
	else
		n->file = m->file;
	n->flags = m->flags;
	n->start = m->start;
	n->size = m->size;
	n->file_sz = m->file_sz;
	n->offset = m->offset;
	n->shm = m->shm;
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
	return ((c_s >= b_s) && (c_s < b_e)) || ((c_e > b_s) && (c_e < b_e)) || 
	       ((b_s >= c_s) && (b_s < c_e)) || ((b_e > c_s) && (b_e < c_e));
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
	process_mmap_t *region;

	assert(file != NULL);

	/* Check if region is already in use */
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;

	/* Allocate memory for region */
	region = heapmm_alloc(sizeof(process_mmap_t));

	/* Check for errors */
	if (!region)
		return ENOMEM;

	/* Handle NULL name field */
	if (name == NULL)
		name = "(anon file)";

	/* Handle unspecified in-file size */
	if (file_sz == 0)
		file_sz = file->size;

	/* If start is not page alligned, try to adjust offset in such a way 
	 * that the file can be loaded at the start of the page */
	in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;
	if (in_page) {
		/* Page allign start */
		start = (void*)(((uintptr_t)start) & ~PHYSMM_PAGE_ADDRESS_MASK);

		/* If the new offset would be before the start of the file 
		 * bail out */
		if (offset < (off_t) in_page) {
			heapmm_free(region, sizeof(process_mmap_t));
			return EINVAL; 
		}

		/* Adjust offset, size */
		offset -= (off_t) in_page;
		size += in_page;
		file_sz += in_page;
	}

	/* Allocate memory for the region name */
	region->name = heapmm_alloc(strlen(name)+1);

	/* Copy region name */
	strcpy(region->name, name);

	if (!region->name) {
		heapmm_free(region, sizeof(process_mmap_t));
		return ENOMEM; 
	}


	/* Fill region fields */
	region->start = start;
	region->size = size;
	region->file = vfs_inode_ref(file);
	region->offset = offset;
	region->flags = flags | PROCESS_MMAP_FLAG_FILE;
	region->file_sz = file_sz;

	/* Test for region collisions */
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		heapmm_free(region->name, strlen(name)+1);
		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;
	}

	/* Add to region list */
	llist_add_end(scheduler_current_task->memory_map, (llist_t *)region);
	
	return 0;
}

int procvmm_mmap_shm(void *start, shm_info_t *shm, int flags, char *name)
{
	process_mmap_t *region;

	assert(shm != NULL);

	/* Check if region is already in use */
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;

	/* Allocate memory for region */
	region = heapmm_alloc(sizeof(process_mmap_t));

	/* Check for errors */
	if (!region)
		return ENOMEM;

	/* Handle NULL name field */
	if (name == NULL)
		name = "(sysv shm)";
	//debugcon_printf("aname\n");
	/* Allocate memory for the region name */
	region->name = heapmm_alloc(strlen(name)+1);
	//debugcon_printf("cname\n");

	/* Copy region name */
	strcpy(region->name, name);
	//debugcon_printf("tname\n");

	if (!region->name) {
		heapmm_free(region, sizeof(process_mmap_t));
		return ENOMEM; 
	}
	//debugcon_printf("freg\n");

	/* Fill region fields */
	region->start = start;
	region->size = shm->info.shm_segsz;
	region->flags = flags | PROCESS_MMAP_FLAG_SHM | PROCESS_MMAP_FLAG_PUBLIC;
	region->shm = shm;
	//debugcon_printf("tcol\n");

	/* Test for region collisions */
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		heapmm_free(region->name, strlen(name)+1);
		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;
	}
	//debugcon_printf("bna\n");

	/* Bump file usage count */
	shm->info.shm_nattch++;
	//debugcon_printf("alist\n");

	/* Add to region list */
	llist_add_end(scheduler_current_task->memory_map, (llist_t *)region);
	
	return 0;
}

int procvmm_mmap_stream(void *start, size_t size, int fd, aoff_t offset, aoff_t file_sz, int flags, char *name)
{
	uintptr_t in_page;
	process_mmap_t *region;
	inode_t* file;
	stream_ptr_t *ptr;
	int st;

	//TODO: Permission check

	/* Get the stream pointer */
	ptr = stream_get_ptr(fd);

	/* Check if it exists */
	if (!ptr)
		return EBADF;

	/* Check if it is a file stream */
	if (ptr->info->type != STREAM_TYPE_FILE)
		return ENODEV;

	/* Get inode */
	file = ptr->info->inode;

	assert(file != NULL);
	
	/* Check if region is already in use */
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;

	/* Allocate memory for region */
	region = heapmm_alloc(sizeof(process_mmap_t));

	/* Check for errors */
	if (!region)
		return ENOMEM;

	/* Handle NULL name field */
	if (name == NULL)
		name = "(anon file)";

	/* Handle unspecified in-file size */
	if (file_sz == 0)
		file_sz = file->size;

	/* If start is not page alligned, try to adjust offset in such a way 
	 * that the file can be loaded at the start of the page */
	in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;
	if (in_page) {
		/* Page allign start */
		start = (void*)(((uintptr_t)start) & ~PHYSMM_PAGE_ADDRESS_MASK);

		/* If the new offset would be before the start of the file 
		 * bail out */
		if (offset < in_page) {
			heapmm_free(region, sizeof(process_mmap_t));
			return EINVAL; 
		}

		/* Adjust offset, size */
		offset -= (off_t) in_page;
		size += in_page;
		file_sz += in_page;
	}

	/* Check whether this is a device mapping */
	if (S_ISCHR(file->mode)) {
		/* It is */
		flags |= PROCESS_MMAP_FLAG_DEVICE;

		/* Let the driver map the memory */
		st = device_char_mmap(file->if_dev, fd, flags, start, offset, file_sz);

		/* Check for errors */
		if (st) {
			heapmm_free(region, sizeof(process_mmap_t));
			return st;
		}
			
	}

	/* Allocate memory for the region name */
	region->name = heapmm_alloc(strlen(name)+1);

	/* Copy region name */
	strcpy(region->name, name);

	if (!region->name) {
		heapmm_free(region, sizeof(process_mmap_t));
		return ENOMEM; 
	}


	/* Fill region fields */
	region->start = start;
	region->size = size;
	region->file = vfs_inode_ref(file);
	region->fd = fd;
	region->offset = offset;
	region->flags = flags | PROCESS_MMAP_FLAG_FILE | PROCESS_MMAP_FLAG_STREAM;
	region->file_sz = file_sz;

	/* Test for region collisions */
	if (llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) region)) {
		heapmm_free(region->name, strlen(name)+1);
		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;
	}

	/* Add to region list */
	llist_add_end(scheduler_current_task->memory_map, (llist_t *)region);
	
	return 0;
}

void procvmm_unmmap(process_mmap_t *region)
{//TODO: Enable writeback
	uintptr_t start = (uintptr_t) region->start;
	uintptr_t page;
	physaddr_t frame;
	llist_unlink((llist_t *) region);
	if (region->flags & PROCESS_MMAP_FLAG_FILE) {
		vfs_inode_release( region->file );
	}
	//if (region->flags & PROCESS_MMAP_FLAG_SHM) {
	//	region->shm->info.shm_nattch--;
	//}
	if (region->flags & PROCESS_MMAP_FLAG_DEVICE) {
		device_char_unmmap(region->file->if_dev, region->fd, region->flags, region->start, region->offset, region->file_sz);
		heapmm_free(region->name, strlen(region->name) + 1);
		heapmm_free(region, sizeof(process_mmap_t));	
		return;
	}
	for (page = start; page < (start +region->size); page += PHYSMM_PAGE_SIZE) {
		frame = paging_get_physical_address((void *)page);
		if (frame) {
			if (!(region->flags & PROCESS_MMAP_FLAG_SHM))
				physmm_free_frame(frame);
			paging_unmap((void *) page);
		}
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
	if (region->flags & PROCESS_MMAP_FLAG_DEVICE)
		return 0; //No demand paging for device maps
	if (!(region->flags & PROCESS_MMAP_FLAG_SHM)) {
		frame = physmm_alloc_frame();
		if (!frame)
			return 0;
	}
	if (region->flags & PROCESS_MMAP_FLAG_WRITE)
		flags |= PAGING_PAGE_FLAG_RW;
	if (!(region->flags & PROCESS_MMAP_FLAG_SHM)) {
		paging_map(address, frame, flags);
		memset(address, 0, read_size);
	}
	if (region->flags & PROCESS_MMAP_FLAG_FILE) {
		assert(region->file != NULL);
		in_region = ((uintptr_t) address) - ((uintptr_t) region->start);
		file_off = region->offset + ((aoff_t) in_region);
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
	if (region->flags & PROCESS_MMAP_FLAG_SHM) {
		assert(region->shm != NULL);
		in_region = ((uintptr_t) address) - ((uintptr_t) region->start);
		//debugcon_printf("shmpf: 0x%x rsz:%i ir:%i pa:%x\n", address, region->size, in_region, region->shm->frames[in_region / PHYSMM_PAGE_SIZE]);
		paging_map(address, region->shm->frames[in_region / PHYSMM_PAGE_SIZE], flags);		
	}
	return 1;
	
}

void *procvmm_attach_shm(void *addr, shm_info_t *shm, int flags)
{
	int st;
	uintptr_t in_page;
	process_mmap_t region, *_r;
	if (addr == NULL) {
		region.start = (void *) scheduler_current_task->heap_max;
		region.size = shm->info.shm_segsz;
		while (1) {
			_r = (process_mmap_t *) llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) &region);
			if (!_r)
				break;
			region.start = (void *)((((uintptr_t)_r->start) + _r->size + PHYSMM_PAGE_ADDRESS_MASK) & ~PHYSMM_PAGE_ADDRESS_MASK);
		}
		addr = region.start;
	}
	//debugcon_printf("ipt\n");
	/* If start is not page alligned, try to adjust offset in such a way 
	 * that the file can be loaded at the start of the page */
	in_page = ((uintptr_t) addr) & PHYSMM_PAGE_ADDRESS_MASK;
	if (in_page) {
		/* Page allign start */
		addr = (void*)( ((uintptr_t)addr) - in_page);
	}
	//debugcon_printf("ashm: %x\n",addr);
	st = procvmm_mmap_shm(addr, shm, flags, NULL);
	if (st) {
		syscall_errno = st;
		return (void *) -1;
	}
	//debugcon_printf("raddr\n");
	return addr;
}

int procvmm_detach_shm(void *shmaddr)
{
	process_mmap_t *region = (process_mmap_t *) llist_iterate_select(scheduler_current_task->memory_map, &procvmm_get_mmap_iterator, shmaddr);
	if (!region) {
		syscall_errno = EINVAL;
		return -1;
	}
	region->shm->info.shm_nattch--;
	if (region->shm->del && !region->shm->info.shm_nattch)
		shm_do_delete(region->shm);
	procvmm_unmmap(region);
	return 0;
}

void *_sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	int st;
	int _flags = 0;
	process_mmap_t region, *_r;
	if (flags & MAP_SHARED)
		flags |= PROCESS_MMAP_FLAG_PUBLIC;
	if (!(flags & MAP_FIXED)) {
		region.start = (void *) scheduler_current_task->heap_max;
		region.size = len;
		while (1) {
			_r = (process_mmap_t *) llist_iterate_select(scheduler_current_task->memory_map, &procvmm_collcheck_iterator, (void *) &region);
			if (!_r)
				break;
			region.start = (void *)((((uintptr_t)_r->start) + _r->size + PHYSMM_PAGE_ADDRESS_MASK) & ~PHYSMM_PAGE_ADDRESS_MASK);
		}
		addr = region.start;
	}

	if (prot & PROT_WRITE)
		_flags |= PROCESS_MMAP_FLAG_WRITE;
	st = procvmm_mmap_stream(addr, len, fd, (aoff_t) offset, len, _flags, NULL);
	if (st) {
		syscall_errno = st;
		return MAP_FAILED;
	}
	return addr;
}

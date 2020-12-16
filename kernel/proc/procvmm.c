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
#define  CON_SRC "procvmm"
#include "kernel/console.h"
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

int strlistlen(const char **list);

/**
 * Unmap all regions from the current process
 */
void procvmm_clear_mmaps()
{
	process_info_t *cproc;
	process_mmap_t *region;

	cproc = current_process;

	/* Call procvmm_unmap for all regions */
	for ( region = (process_mmap_t *) llist_get_last( cproc->memory_map );
	      region != NULL;
	      region = (process_mmap_t *) llist_get_last( cproc->memory_map ) )
		procvmm_unmmap(region);
}

/**
 * Unmap all regions for a specific process
 */
void procvmm_clear_mmaps_other( process_info_t *info )
{
	process_mmap_t *region;

	/* Call procvmm_unmap for all regions */
	for ( region = (process_mmap_t *) llist_get_last( info->memory_map );
	      region != NULL;
	      region = (process_mmap_t *) llist_get_last( info->memory_map ) )
		procvmm_unmmap_other(info, region);
}
/*
 * User address map:
 * | Start    | End         | Description                           |
 * +----------+-------------+---------------------------------------+
 * | 00000000 | 00000FFF    | Zero page, always unmapped
 * | img_base | img_end - 1 | Executable image
 * | img_end  | 3FFFFFFF    | sbrk() heap
 * | 40000000 | BF3FFFFF    | mmap() auto generated address range
 * | BF400000 | BFBFEFFF    | Initial thread stack
 * | BFBFF000 | BFBFFFFF    | Initial thread signal handler stack
 */
int procvmm_do_exec_mmaps()
{
	int s;
	process_info_t *cproc;

	cproc = scheduler_current_task->process;

	cproc->heap_start   = (void *) cproc->image_end;
	cproc->heap_end     = cproc->heap_start;
	cproc->heap_max	    = (void *) 0x40000000;
	cproc->stack_bottom = (void *) 0xBFBFF000;
	cproc->stack_top    = (void *) 0xBF400000;

	s = procvmm_mmap_anon(
	    /* start */ cproc->stack_bottom,
	    /* size  */ 0x1000,
	    /* flags */ PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK,
	    /* name  */ "(sigstack)" );//TODO: Handle errors

	if ( s )
		return s;

	s = procvmm_mmap_anon(
	    /* start */ cproc->stack_top,
	    /* size  */ cproc->stack_bottom - cproc->stack_top,
	    /* flags */ PROCESS_MMAP_FLAG_WRITE | PROCESS_MMAP_FLAG_STACK,
	    /* name  */ "(stack)" );

	return s;
}

int procvmm_check( const void *dest, size_t size)
{
	uintptr_t b;
	uintptr_t p;

	/* Compute the start of the first page of the area */
	b = ((uintptr_t)dest) & ~PAGE_ADDR_MASK;

	/* If there is no current proces or the current process is the init
	 * process, we always allow creating the mapping */ //TODO: WHY?
	if ( (!current_process) || current_process->pid == 0 )
		return 1;

	/* Check if a mapping exists for any of the pages in the range */
	for ( p = 0; p < size; p += PAGE_SIZE ) {
		if (!procvmm_get_memory_region((void *) (b + p))) {
			return 0;
		}
	}

	/* No overlap found, allow creating the mapping */
	return 1;
}

/**
 * @brief Safely determines the minimal storage area for a string.
 *
 * This function effectively returns strlen(str) + 1, while preemptively
 * testing for fatal page faults.
 *
 */
int procvmm_check_string( const char *str, size_t size_max )
{
	uintptr_t p,lp,cp,b;

	/* If this task does not belong to a process or belongs to the
	 * kernel we do not need to check the string, and defer to strlen */
	if ( (!current_process) || current_process->pid == 0 )
		return strlen( str ) + 1;

	lp = 1;
	b = ( uintptr_t ) str;

	/* Iterate over the string until the max size is hit */
	for (p = 0; p < size_max; p++) {

		/* Keep track of the page we're on */
		cp = (b + p) & PAGE_ADDR_MASK;

		/* If this is a different page than last iteration, verify
		 * that we have access to it */
		if ( lp != cp ) {
			if ( !procvmm_get_memory_region( str ) ) {
				return PROCVMM_INV_MAPPING;
			}
			lp = cp;
		}

		/* Check for the string terminator, if found we're done */
		if ( !*str ) {
			return p + 1;
		}
		str++;
	}
	return PROCVMM_TOO_LARGE;
}

int procvmm_check_stringlist(	const char **dest,
				size_t len_max )
{
	uintptr_t p,lp,cp,b;

	if ((!current_process) ||
		current_process->pid == 0)
		return strlistlen( dest ) + 1;

	lp = 1;
	b = ( uintptr_t ) dest;

	for (p = 0; p < len_max; p++) {
		cp = (b + 4*p) & PAGE_ADDR_MASK;
		if ( lp != cp ) {
			if (!procvmm_get_memory_region(dest))
				return PROCVMM_INV_MAPPING;
			lp = cp;
		}
		if ( !*dest ) {
			return p + 1;
		};
		dest++;
	}
	return PROCVMM_TOO_LARGE;
}

/**
 * Iterator function that finds the mmap for the given address
 */
static int get_mmap_iter (llist_t *node, void *param)
{
	process_mmap_t *m = (process_mmap_t *) node;
	uintptr_t p   = (uintptr_t) param;
	uintptr_t b_s = (uintptr_t) m->start;
	uintptr_t b_e = b_s + m->size;
	return (p >= b_s) && (p < b_e);
}

/**
 * Gets the region containing an address
 */
process_mmap_t *procvmm_get_memory_region(const void *address)
{

	/* If this task does not belong to a process or belongs to the
	 * kernel the procvmm is not active, return NULL */
	if ( (!current_process) || current_process->pid == 0 )
		return NULL;

	return (process_mmap_t *) llist_iterate_select(
	    /* list     */ current_process->memory_map,
	    /* iterator */ &get_mmap_iter,
	    /* param    */ (void*) address );
}

static process_mmap_t *mmap_alloc_init( const char *name ) {
	process_mmap_t *dst;

	assert( name != NULL );

	/* Allocate the destination map descriptor */
	dst = heapmm_alloc( sizeof(process_mmap_t) );
	if ( !dst )
		goto error_desc;

	/* Zero out descriptor */
	memset( dst, 0, sizeof(process_mmap_t) );

	/* Allocate dst region name */
	dst->name = heapmm_alloc( strlen( name ) + 1 );
	if ( !dst->name )
		goto error_name;

	/* Copy region name */
	strcpy( dst->name, name );

	return dst;

error_name:
	heapmm_free( dst, sizeof(process_mmap_t) );
error_desc:
	return NULL;
}

static void mmap_free( process_mmap_t *region )
{

	/* Get rid of any file reference */
	if ( region->flags & PROCESS_MMAP_FLAG_FILE ) {
		vfs_inode_release( region->file );
	}

	/* Adjust SHM refcount, if needed */
	if ( region->flags & PROCESS_MMAP_FLAG_SHM ) {

		region->shm->info.shm_nattch--;
		if ( region->shm->del && !region->shm->info.shm_nattch )
			shm_do_delete( region->shm );

	}

	/* Free region name */
	heapmm_free( region->name, strlen( region->name ) + 1) ;

	/* Free region descriptor */
	heapmm_free( region, sizeof( process_mmap_t ) );
}

static int mmap_copy_iter (llist_t *_src, void *_dstlist)
{
	llist_t *dstlist;
	process_mmap_t *src;
	process_mmap_t *dst;

	dstlist = (llist_t *) _dstlist;
	src     = (process_mmap_t *) _src;

	/* Allocate the destination map descriptor */
	dst     = mmap_alloc_init( src->name );
	if ( !dst )
		goto error_desc;

	/* Copy over common metadata */
	dst->flags   = src->flags;
	dst->start   = src->start;
	dst->size    = src->size;
	dst->offset  = src->offset;
	dst->shm     = src->shm;

	/* If this mapping refers to a file we need to get a new ino ref */
	if ( (src->flags & PROCESS_MMAP_FLAG_FILE) && src->file ) {
		dst->file = vfs_inode_ref( src->file );
		dst->file_sz = src->file_sz;
	}

	/* If this mapping refers to a stream, copy over the fd */
	if ( src->flags & PROCESS_MMAP_FLAG_STREAM )
		dst->fd  = src->fd;

	/* If this mapping refers to a SysV SHM segment, reference it */
	if ( src->flags & PROCESS_MMAP_FLAG_SHM )
		dst->shm = src->shm;

	/* Add dst to dst list */
	llist_add_end( dstlist, (llist_t *) dst );

	return 0;

error_desc:
	/* Dirty hack: returning nonzero will cause llist_iter_sel
	 * to return the current entry, which is detected by
	 * procvmm_copy_memory_map as an error condition */
	return 1;
}

int procvmm_copy_memory_map ( llist_t *target )
{
	return llist_iterate_select(
	    /* list     */ current_process->memory_map,
	    /* iterator */ &mmap_copy_iter,
	    /* param    */ (void*) target ) != NULL;
}

/**
 * Iterator function that finds the mmap for the given address
 */
static int coll_check_iter ( llist_t *_b, void *_c )
{
	process_mmap_t *b, *c;
	uintptr_t b_s, b_e, c_s, c_e;

	b = (process_mmap_t *) _b;
	c = (process_mmap_t *) _c;

	/* Compute region B start and end */
	b_s = (uintptr_t) b->start;
	b_e = (uintptr_t) b->start + b->size;

	/* Compute region C start and end */
	c_s = (uintptr_t) c->start;
	c_e = (uintptr_t) c->start + c->size;

	/* Ignore collision with self */
	if ( b == c )
		return 0;

	/* If the start of region C is in region B, there is overlap */
	if ( (c_s >= b_s) && (c_s < b_e) )
		return 1;

	/* If the end of region C is in region B, there is overlap */
	if ( (c_e > b_s) && (c_e < b_e) )
		return 1;

	/* If the start of region B is in region C, there is overlap */
	if ( (b_s >= c_s) && (b_s < c_e) )
		return 1;

	/* If the end of region B is in region C, there is overlap */
	if ( (b_e > c_s) && (b_e < c_e) )
		return 1;

	/* If none of these conditions were hit, there is no overlap */
	return 0;
}

/**
 * @brief Check whether region collides with any existing region
 *
 * @param region     The region to check
 * @return           The region that collided with the arg region.
 */
static process_mmap_t * find_collide( process_mmap_t *region )
{
	llist_t *overlap;

	/* Find first overlapping region, if any */
	overlap = llist_iterate_select(
	     /* list     */ current_process->memory_map,
		 /* iterator */ &coll_check_iter,
		 /* param    */ region );

	/* If no region was found, there are no collisions */
	return ( process_mmap_t * ) overlap;

}

/**
 * @brief Check whether region collides with any existing region
 *
 * @param region     The region to check
 * @return           True iff the region collided with another
 */
static int does_collide( process_mmap_t *region )
{
	return find_collide( region ) != NULL;
}

/**
 * Tries to find a suitable address for a region of size
 * @param size        The size of the region to allocate
 * @return            The address that was found, or NULL if none was.
 */
static void *find_address( size_t size )
{
	process_mmap_t region, *_r;
	uintptr_t end_region;

	/* Set up the fake region to use to call find_collide */
	region.start = (void *) current_process->heap_max;
	region.size  = size;

	/* As long as the address range tried overlaps with an existing
	 * region, try to round up the end of the colliding region to a
	 * page boundary and try that as the next address. This effectively
	 * performs a greedy search for a sufficiently large hole in the
	 * processes address map */
	while ( ( _r = find_collide( &region ) ) != NULL ) {

		end_region = (uintptr_t) ( _r->start + _r->size );

		region.start = ( void *) PAGE_ROUND_UP( end_region );

		/* Check if we ran out of address space */
		if ( region.start >= current_process->stack_top )
			return NULL;

	}

	return region.start;
}


static int mmap_add( process_mmap_t *region )
{
	if ( does_collide( region ) ) {

		/* Collision: free region, return EINVAL */
		mmap_free( region );
		return EINVAL;

	} else {

		/* No collision: add to mmap list */
		llist_add_end( current_process->memory_map, (llist_t *) region );
		return 0;

	}
}
/**
 * @brief Resize region
 *
 * @param start      The start of the region to resize.
 * @param newsize    The new size of the region.
 * @return           Zero or an ERRNO code on error.
 * @exception EINVAL New size would have caused region to overlap with
 *                   another region.
 */
int procvmm_resize_map(void *start, size_t newsize)
{
	size_t oldsize;
	process_mmap_t *region;

	/* Try to find region */
	region = procvmm_get_memory_region( start );
	if ( !region )
		return 1;

	/* Try to resize region */
	oldsize = region->size;
	region->size = newsize;
	if ( does_collide( region ) ) {
		/* Region overlapped, restore old size and signal error */
		region->size = oldsize;
		return EINVAL;
	}

	/* Return success */
	return 0;
}

/**
 * @brief Map region of anonymous memory
 * @param start       The base address of the region.
 * @param size        The size of the region to map.
 * @param flags       Any of the non-type PROCESS_MMAP_FLAGs.
 * @param name        The region name.
 * @return            Zero on success, an ERRNO otherwise.
 * @exception ENOMEM  No kernel heap was available to hold descriptors.
 * @exception EINVAL  The requested region overlapped mapped memory.
 */
int procvmm_mmap_anon (
        void *       start,
        size_t       size,
        int          flags,
        const char * name )
{
	process_mmap_t *region;
	uintptr_t in_page;

	/* Round start down to start of page and adjust size accordingly */
	in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;
	if ( in_page ) {
		start -= in_page;
		size  += (size_t) in_page;
	}

	/* Initial, simple collision check: is start in any known region */
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;

	/* Handle unset name */
	if (name == NULL)
		name = "(anon)";

	/* Try to allocate region descriptor */
	region = mmap_alloc_init( name );
	if (!region)
		return ENOMEM;

	region->start = start;
	region->size  = size;
	region->flags = flags & ~PROCESS_MMAP_FLAG_FILE;


	/* Collision check and add to region list */
	return mmap_add( region );

}

/**
 * @brief Map file or stream into region of memory
 *
 * If size > file_sz the remaining area is filled with zeros
 * Device maps require fd to be set. If it is not, the device file will
 * be mapped as a regular file.
 *
 * @param start       The base address of the region.
 * @param size        The size of the region to map.
 * @param file        The inode to map.
 * @param offset      The offset into the file to map.
 * @param file_sz     The size of the file to map.
 * @param flags       Any of the non-type PROCESS_MMAP_FLAGs.
 * @param name        The region name.
 * @param fd          The stream ref to set.
 * @return            Zero on success, an ERRNO otherwise.
 * @exception ENOMEM  No kernel heap was available to hold descriptors.
 * @exception EINVAL  The requested region overlapped mapped memory.
 * @exception EINVAL  After adjusting the base to a page boundary the
 *                    offset is less than 0.
 */
static int int_mmap_file (
        void *       start,
        size_t       size,
        inode_t *    file,
        off_t        offset,
        off_t        file_sz,
        int          flags,
        const char * name,
        int          fd )
{
	int st;
	uintptr_t in_page;
	process_mmap_t *region;

	assert( file != NULL );

	/* Check if region is already in use */
	region = procvmm_get_memory_region(start);
	if ( region )
		return EINVAL;

	/* Handle NULL name field */
	if ( name == NULL )
		name = "(anon file)";

	/* Handle unspecified in-file size */
	if (file_sz == 0)
		file_sz = file->size;

	/* If start is not page alligned, try to adjust offset in such a way
	 * that the file can be loaded at the start of the page */
	in_page = ((uintptr_t) start) & PHYSMM_PAGE_ADDRESS_MASK;

	if ( offset < (off_t) in_page ) {
		/* If the new offset would be before the start of the file
		* bail out */

		heapmm_free(region, sizeof(process_mmap_t));
		return EINVAL;

	} else if ( in_page ) {
		/* Adjust start, offset, size */

		start   -= in_page;
		offset  -= (off_t) in_page;
		size    += in_page;
		file_sz += in_page;
	}

	/* Try to allocate region descriptor */
	region = mmap_alloc_init( name );
	if (!region)
		return ENOMEM;

	/* Fill region fields */
	region->start   = start;
	region->size    = size;
	region->flags   = flags | PROCESS_MMAP_FLAG_FILE;
	region->file    = vfs_inode_ref(file);
	region->offset  = offset;
	region->file_sz = file_sz;

	/* Handle stream */
	if ( fd >= 0 ) {
		region->fd     = fd;
		region->flags |= PROCESS_MMAP_FLAG_STREAM;

		/* Check whether this is a device mapping */
		if ( S_ISCHR(file->mode) ) {
			/* It is */
			flags |= PROCESS_MMAP_FLAG_DEVICE;

			/* Preemptively check for collisions because
			 * device_char_mmap already alters the page tables */
			if ( does_collide( region ) ) {
				st = EINVAL;
				goto error_collide;
			}

			/* Let the driver map the memory */
			st = device_char_mmap(
				/* dev    */ file->if_dev,
				/* fd     */ fd,
				/* flags  */ flags,
				/* base   */ start,
				/* offset */ offset,
				/* size   */ file_sz );

			/* Check for errors */
			if (st)
				goto error_dev;

		}
	}

	/* Collision check and add to region list */
	return mmap_add( region );

error_dev:
error_collide:
	mmap_free( region );
	return st;
}

/**
 * @brief Map file into region of memory
 *
 * If size > file_sz the remaining area is filled with zeros
 * This function does not support mapping device files.
 *
 * @param start       The base address of the region.
 * @param size        The size of the region to map.
 * @param file        The inode to map.
 * @param offset      The offset into the file to map.
 * @param file_sz     The size of the file to map.
 * @param flags       Any of the non-type PROCESS_MMAP_FLAGs.
 * @param name        The region name.
 * @return            Zero on success, an ERRNO otherwise.
 * @exception ENOMEM  No kernel heap was available to hold descriptors.
 * @exception EINVAL  The requested region overlapped mapped memory.
 * @exception EINVAL  After adjusting the base to a page boundary the
 *                    offset is less than 0.
 */
int procvmm_mmap_file (
        void *       start,
        size_t       size,
        inode_t *    file,
        off_t        offset,
        off_t        file_sz,
        int          flags,
        const char * name )
{
	return int_mmap_file(
	    start,
	    size,
	    file,
	    offset,
	    file_sz,
	    flags,
	    name,
	    /* fd */ -1 );
}

/**
 * @brief Map stream into region of memory
 *
 * If size > file_sz the remaining area is filled with zeros
 *
 * @param start       The base address of the region.
 * @param size        The size of the region to map.
 * @param fd          The stream to map.
 * @param offset      The offset into the file to map.
 * @param file_sz     The size of the file to map.
 * @param flags       Any of the non-type PROCESS_MMAP_FLAGs.
 * @param name        The region name.
 * @return            Zero on success, an ERRNO otherwise.
 * @exception ENOMEM  No kernel heap was available to hold descriptors.
 * @exception EINVAL  The requested region overlapped mapped memory.
 * @exception EINVAL  After adjusting the base to a page boundary the
 *                    offset is less than 0.
 * @exception EBADF   fd does not refer to a stream.
 * @exception ENODEV  The stream is not of a supported type.
 */
int procvmm_mmap_stream (
        void *       start,
        size_t       size,
        int          fd,
        aoff_t       offset,
        aoff_t       file_sz,
        int          flags,
        const char * name )
{
	inode_t* file;
	stream_ptr_t *ptr;

	//TODO: Permission check

	/* Get the stream pointer */
	ptr = stream_get_ptr(fd);

	/* Check if it exists */
	if (!ptr)
		return EBADF;

	/* Check if it is a file stream */
	if ( ptr->info->type != STREAM_TYPE_FILE ) {
		//TODO: Support external stream?
		return ENODEV;
	}

	/* Get inode */
	file = ptr->info->inode;

	assert(file != NULL);

	return int_mmap_file(
	    start,
	    size,
	    file,
	    offset,
	    file_sz,
	    flags,
	    name,
	    fd );
}

/**
 * @brief Map region of shared memory
 * @param start       The base address of the region.
 * @param shm         The SysV SHM segment to map.
 * @param flags       Any of the non-type PROCESS_MMAP_FLAGs.
 * @param name        The region name.
 * @return            Zero on success, an ERRNO otherwise.
 * @exception ENOMEM  No kernel heap was available to hold descriptors.
 * @exception EINVAL  The requested region overlapped mapped memory.
 */
int procvmm_mmap_shm(
        void *       start,
        shm_info_t * shm,
        int          flags,
        const char * name )
{
	process_mmap_t *region;

	assert( shm != NULL );

	/* Check if region is already in use */
	region = procvmm_get_memory_region(start);
	if (region)
		return EINVAL;

	/* Handle NULL name field */
	if (name == NULL)
		name = "(sysv shm)";

	/* Try to allocate region descriptor */
	region = mmap_alloc_init( name );
	if (!region)
		return ENOMEM;

	flags |= PROCESS_MMAP_FLAG_SHM | PROCESS_MMAP_FLAG_PUBLIC;

	/* Fill region fields */
	region->start = start;
	region->size = shm->info.shm_segsz;
	region->flags = flags;
	region->shm = shm;

	/* Bump file usage count */
	shm->info.shm_nattch++;

	/* Collision check and add to region list */
	return mmap_add( region );
}

void procvmm_unmmap_other(process_info_t *task, process_mmap_t *region)
{
	uintptr_t start, page;
	physaddr_t frame;

	llist_unlink( (llist_t *) region );

	start = (uintptr_t) region->start;

	if (region->flags & PROCESS_MMAP_FLAG_DEVICE) {

		//TODO: Unmap device

	} else {

		//TODO: Write back dirty pages

		for ( page = start;
		      page < (start +region->size);
		      page += PHYSMM_PAGE_SIZE ) {

			frame = paging_get_physical_address_other(
			        task->page_directory,
			        (void *) page );

			if (frame) {

				if ( ~region->flags & PROCESS_MMAP_FLAG_SHM )
					physmm_free_frame(frame);


				//TODO: Unmap-other?

			}
		}
	}

	mmap_free( region );
}


void procvmm_unmmap(process_mmap_t *region)
{
	uintptr_t start, page;
	physaddr_t frame;

	llist_unlink( (llist_t *) region );

	start = (uintptr_t) region->start;

	llist_unlink((llist_t *) region);

	if (region->flags & PROCESS_MMAP_FLAG_DEVICE) {

		device_char_unmmap(
		        region->file->if_dev,
		        region->fd,
		        region->flags,
		        region->start,
		        region->offset,
		        region->file_sz);

	} else {
		//TODO: Write back dirty pages

		for ( page = start;
		      page < (start +region->size);
		      page += PHYSMM_PAGE_SIZE ) {

			frame = paging_get_physical_address(
			        (void *) page );

			if (frame) {

				if ( ~region->flags & PROCESS_MMAP_FLAG_SHM )
					physmm_free_frame(frame);

				paging_unmap((void *) page);

			}
		}
	}

	mmap_free( region );
}

/**
 * Handle user land demand paging
 * @param _address The address where the fault occurred
 * @return         Returns whether or not the page fault could be
 *                 resolved
 */
int procvmm_handle_fault( void *_address )
{
	page_flags_t flags = PAGING_PAGE_FLAG_USER;
	uintptr_t in_region;
	aoff_t file_off;
	size_t read_size;
	aoff_t rd_count = 0;
	physaddr_t frame;
	process_mmap_t *region;
	int status;
	uintptr_t address;

	/* Find the region containing the address */
	region = procvmm_get_memory_region( _address );

	/* If there is none, this is a fatal fault */
	if ( !region )
		return 0;

	/* Device maps are not demand paged, return 0 (fail to map) */
	if ( region->flags & PROCESS_MMAP_FLAG_DEVICE )
		return 0;

	/* All logic from here on deals with a whole page, so round down */
	address = PAGE_ROUND_DOWN( (uintptr_t) _address );

	/* Determine how far into the region the page miss is */
	in_region = address - ( uintptr_t ) region->start;

	/* Handle WRITE flag */
	if (region->flags & PROCESS_MMAP_FLAG_WRITE)
		flags |= PAGING_PAGE_FLAG_RW;

	/* SHM has pre-allocated frames, no need to allocate one now */
	if ( region->flags & PROCESS_MMAP_FLAG_SHM ) {

		/* Make sure the region has a valid SHM segment pointer */
		assert(region->shm != NULL);

		/* Map the right frame */
		paging_map(
		    (void *) address,
		    region->shm->frames[in_region / PHYSMM_PAGE_SIZE],
		    flags );

	} else { //TODO: Handle shared mappings

		/* Try to allocate memory to fill the page */
		frame = physmm_alloc_frame();
		if ( frame == PHYSMM_NO_FRAME ) {
			//TODO: This should not kill process, only in the worst case
			//      freeze it until more memory is available.
			return 0;
		}

		/* Map and zero the newly allocated memory */
		paging_map( (void *) address, frame, flags );
		memset( (void *) address, 0, PAGE_SIZE );

		/* If this is a file mapping, load the contents of the file */
		if (region->flags & PROCESS_MMAP_FLAG_FILE) {

			/* Make sure that the region has a valid inode ref */
			assert( region->file != NULL );

			/* Compute the offset into the file */
			file_off = region->offset + (aoff_t) in_region;

			/* Make sure page is within file map range */
			if ( ( in_region < (uintptr_t) region->file_sz ) &&
			     ( file_off < region->file->size ) ) {

				read_size = PHYSMM_PAGE_SIZE;

				/* If only part of the page is in the file map range,
				 * truncate the read */
				if ( (in_region + read_size) > (uintptr_t) region->file_sz)
					read_size = (size_t) (region->file_sz - in_region);

				status = vfs_read(
				    /* file   */   region->file,
				    /* offset */   file_off,
				    /* buffer */   (void *) address,
				    /* size   */   read_size,
				    /* size_out */ &rd_count, 0);

				if ( status || rd_count != read_size ) {
					/* Could not read file, report error and undo
					 * already mapped memory */

					printf( CON_ERROR,
						"demand page read error "
						"%i byte %i in %s",
						status, rd_count, region->name );

					paging_unmap( (void *) address );
					physmm_free_frame( frame );

					return 0;
				}
			}
		}
	}

	return 1;

}

void *_sys_mmap(
        void *addr,
        size_t len,
        int prot,
        int flags,
        int fd,
        off_t offset )
{
	int st;
	int _flags = 0;

	/* If the address is not to be fixed, find a new address */
	if ( ~flags & MAP_FIXED )
		addr = find_address( len );

	/* Convert POSIX flags into internal procvmm flags */
	if ( flags & MAP_SHARED )
		_flags |= PROCESS_MMAP_FLAG_PUBLIC;

	if ( prot & PROT_WRITE )
		_flags |= PROCESS_MMAP_FLAG_WRITE;

	st = procvmm_mmap_stream(
		/* addr    */ addr,
		/* size    */ len,
		/* fd      */ fd,
		/* offset  */ (aoff_t) offset,
		/* file_sz */ len,
		/* flags   */ _flags,
		/* name    */ NULL );


	if ( st ) {
		syscall_errno = st;
		return MAP_FAILED;
	} else
		return addr;
}

void *procvmm_attach_shm(void *addr, shm_info_t *shm, int flags)
{
	int st;

	/* If the address is not set, find one */
	if ( addr == NULL )
		addr = find_address( shm->info.shm_segsz );

	/* If start is not page alligned, round down */
	addr = (void *) PAGE_ROUND_DOWN( (uintptr_t) addr );

	st = procvmm_mmap_shm(addr, shm, flags, NULL);

	if ( st ) {
		syscall_errno = st;
		return (void *) -1;
	} else {
		return addr;
	}
}

int procvmm_detach_shm(void *shmaddr)
{
	process_mmap_t *region;

	region = procvmm_get_memory_region( shmaddr );
	if (!region) {
		syscall_errno = EINVAL;
		return -1;
	}

	procvmm_unmmap( region );
	return 0;
}


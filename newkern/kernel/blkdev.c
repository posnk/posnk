/**
 * kernel/blkdev.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 17-04-2014 - Created
 */

#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/earlycon.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>

//TODO: TODO: TODO: IMPLEMENT BLOCK DEVICES

blk_dev_t **block_dev_table;

void device_block_init()
{
	block_dev_table = heapmm_alloc( sizeof(blk_dev_t *) * 256 );
	memset( block_dev_table, 0, sizeof(blk_dev_t *) * 256 );
}

int device_block_register(blk_dev_t *driver)
{
	dev_t minor, _m;

	if (!driver)
		return 0;

	if (block_dev_table[driver->major])
		return 0;

	driver->caches = 
		heapmm_alloc(sizeof(blkcache_cache_t *) 
				* driver->minor_count);

	if (!driver->caches)
		return 0;

	for (minor = 0; minor < driver->minor_count; minor++) {
		driver->caches[minor] = blkcache_create(driver->block_size,
						 driver->cache_size);
		if (!driver->caches[minor]) {
			for (_m = 0; _m < minor; _m++)
				blkcache_free(driver->caches[_m]);
			heapmm_free(
				driver->caches, 
				sizeof(blkcache_cache_t *) 
					* driver->minor_count);
			return 0;
		}
	}

	driver->locks = 
		heapmm_alloc(sizeof(semaphore_t *) 
				* driver->minor_count);

	if (!driver->locks) {
		for (_m = 0; _m < driver->minor_count; _m++)
			blkcache_free(driver->caches[_m]);
		heapmm_free(
			driver->caches, 
			sizeof(blkcache_cache_t *) 
				* driver->minor_count);
		return 0;
	}

	for (minor = 0; minor < driver->minor_count; minor++) {
		driver->locks[minor] = semaphore_alloc();
		semaphore_up(driver->locks[minor]);
		if (!driver->caches[minor]) {
			for (_m = 0; _m < minor; _m++)
				semaphore_free(driver->locks[_m]);
			for (_m = 0; _m < driver->minor_count; _m++)
				blkcache_free(driver->caches[_m]);
			heapmm_free(
				driver->locks, 
				sizeof(semaphore_t *) 
					* driver->minor_count);
			heapmm_free(
				driver->caches, 
				sizeof(blkcache_cache_t *) 
					* driver->minor_count);
			return 0;
		}
	}
		
	block_dev_table[driver->major] = driver;
	return 1;
}

int device_block_flush(dev_t device, aoff_t block_offset)
{
	int rv;
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	blkcache_entry_t *entry = blkcache_find(drv->caches[minor], block_offset);

	if (!entry) {
		return 0;
	}

	rv = drv->ops->write(device, block_offset, entry->data);
	
	if (!rv)
		entry->flags &= ~BLKCACHE_ENTRY_FLAG_DIRTY;

	return rv;
}

int device_block_fetch(dev_t device, aoff_t block_offset)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	blkcache_entry_t *entry = blkcache_get(drv->caches[minor], block_offset);

	if (!entry) {

		device_block_flush(device,
			blkcache_get_discard_candidate(drv->caches[minor])->offset);

		entry = blkcache_get(drv->caches[minor], block_offset);

	}

	if (entry == BLKCACHE_ENOMEM)
		return ENOMEM;

	return drv->ops->read(device, block_offset, entry->data);

}

int device_block_ioctl(dev_t device, int fd, int func, int arg)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	return drv->ops->ioctl(device, fd, func, arg);
}

int device_block_open(dev_t device, int fd, int options)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	return drv->ops->open(device, fd, options);
}

int device_block_close(dev_t device, int fd)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	return drv->ops->close(device, fd);
}

int device_block_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size)
{

	int rv;
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];
	blkcache_entry_t *entry;
	aoff_t	block_offset;
	aoff_t	in_block_count;
	uintptr_t in_block, in_buffer;

	if ((!drv) || (minor > drv->minor_count)) {
		debugcon_printf("invalid %i %i\n", (int)major, (int)minor);
		return ENODEV;
	}

	in_buffer = 0;

	semaphore_down(drv->locks[minor]);

	while (in_buffer != ((uintptr_t) count)) {

		in_block  = (file_offset + in_buffer) % drv->block_size;
	
		block_offset = (file_offset + in_buffer) - in_block;
		in_block_count = count - in_buffer;

		if(in_block_count > (drv->block_size - in_block))
			in_block_count = drv->block_size - in_block;	
		
		if (in_block_count != drv->block_size) {//Read-modify-write needed			
		
			entry = blkcache_find(drv->caches[minor], block_offset);
			if (!entry) {
				rv = device_block_fetch(device, block_offset);
				if (rv) {
					semaphore_up(drv->locks[minor]);
					return rv;
				}
			}			
		
			entry = blkcache_find(drv->caches[minor], block_offset);

		} else { //Just add it to the cache

			entry = blkcache_get(drv->caches[minor], block_offset);

			if (!entry) {

				device_block_flush(device,
					blkcache_get_discard_candidate(drv->caches[minor])->offset);

				entry = blkcache_get(drv->caches[minor], block_offset);

			}		

			if (entry == BLKCACHE_ENOMEM) {

				semaphore_up(drv->locks[minor]);
				return ENOMEM;

			}

		}

		memcpy( (void *) ( ((uintptr_t)entry->data) + in_block ) , (void *) ( ((uintptr_t) buffer) + in_buffer ), in_block_count ); 
		entry->flags |= BLKCACHE_ENTRY_FLAG_DIRTY;

		in_buffer += in_block_count;
		block_offset += drv->block_size;

		*write_size = in_buffer;
	}

	semaphore_up(drv->locks[minor]);

	return 0;
	
	
}

int device_block_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size)
{

	int rv;
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];
	blkcache_entry_t *entry;
	aoff_t	block_offset;
	aoff_t	in_block_count;
	uintptr_t in_block, in_buffer;

	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	in_buffer = 0;

	semaphore_down(drv->locks[minor]);

	while (in_buffer != ((uintptr_t) count)) {

		in_block  = (file_offset + in_buffer) % drv->block_size;
	
		block_offset = (file_offset + in_buffer) - in_block;

		in_block_count = count - in_buffer;

		if(in_block_count > (drv->block_size - in_block))
			in_block_count = drv->block_size - in_block;	
		
		entry = blkcache_find(drv->caches[minor], block_offset);
		if (!entry) {
			rv = device_block_fetch(device, block_offset);
			if (rv) {
				semaphore_up(drv->locks[minor]);
				return rv;
			}			
		
			entry = blkcache_find(drv->caches[minor], block_offset);
		}

		memcpy( (void *) ( ((uintptr_t) buffer) + in_buffer ), (void *) ( ((uintptr_t)entry->data) + in_block ), in_block_count ); 

		in_buffer += in_block_count;
		block_offset += drv->block_size;

		*read_size = in_buffer;
	}

	semaphore_up(drv->locks[minor]);

	return 0;
}

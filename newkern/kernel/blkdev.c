/**
 * @file kernel/blkdev.c
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * @li 17-04-2014 - Created
 * @li 14-07-2014 - Documented
 */

#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/earlycon.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <assert.h>

/**
 * @brief Stores the driver descriptors for each block major device 
 */
blk_dev_t **block_dev_table;

/**
 * @brief Initialize the block device interface
 */
void device_block_init()
{
	/* Allocate memory for the device table */
	block_dev_table = heapmm_alloc( sizeof(blk_dev_t *) * 256 );
	assert(block_dev_table != NULL);

	/* Clear the device table */
	memset( block_dev_table, 0, sizeof(blk_dev_t *) * 256 );
}

/**
 * @brief Register a new block device driver
 * @see blk_dev for more information on block device drivers
 * @param driver The driver descriptor for the driver to register
 * @return When successful 1 is returned, otherwise 0 will be returned
 */
int device_block_register(blk_dev_t *driver)
{
	dev_t minor, _m;

	/* Check for null pointers */
	assert(driver != NULL);

	/* Check if the driver id is already in use */
	if (block_dev_table[driver->major])
		return 0;

	/* Allocate memory for the minor device cache table */
	driver->caches = 
		heapmm_alloc(sizeof(blkcache_cache_t *) 
				* driver->minor_count);

	/* Check for errors */
	if (!driver->caches)
		return 0;

	/* Allocate the minor device caches */
	for (minor = 0; minor < driver->minor_count; minor++) {
		/* Allocate the block cache */
		driver->caches[minor] = blkcache_create(driver->block_size,
						 driver->cache_size);

		/* Check for errors */
		if (!driver->caches[minor]) {
			/* Clean up */
			for (_m = 0; _m < minor; _m++)
				blkcache_free(driver->caches[_m]);
			heapmm_free(
				driver->caches, 
				sizeof(blkcache_cache_t *) 
					* driver->minor_count);
			return 0;
		}
	}

	/* Allocate the minor device lock table */
	driver->locks = 
		heapmm_alloc(sizeof(semaphore_t *) 
				* driver->minor_count);

	/* Check for errors */
	if (!driver->locks) {
		/* Clean up */
		for (_m = 0; _m < driver->minor_count; _m++)
			blkcache_free(driver->caches[_m]);
		heapmm_free(
			driver->caches, 
			sizeof(blkcache_cache_t *) 
				* driver->minor_count);
		return 0;
	}
	
	/* Allocate the minor device locks */
	for (minor = 0; minor < driver->minor_count; minor++) {
		/* Allocate the lock */
		driver->locks[minor] = semaphore_alloc();

		/* Check for errors */
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
		
		/* Release the lock */
		semaphore_up(driver->locks[minor]);
	}
	
	/* Add the driver to the table */	
	block_dev_table[driver->major] = driver;
	return 1;
}

/**
 * @brief Flush a block from the cache
 * @param device The device id to operate on
 * @param block_offset The starting offset of the block to flush
 * @return 0 if successful, a valid errorcode if not
 * @exception ENXIO _device_ does not refer to a valid block device
 */

int device_block_flush(dev_t device, aoff_t block_offset)
{
	int rv;
	blkcache_entry_t *entry;
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENXIO;
	}

	/* Get the entry from the cache */
	entry = blkcache_find(drv->caches[minor], block_offset);

	/* If the block is not cached, return success */
	if (!entry) {
		return 0;
	}
	
	/* Call the driver to write the block to storage */
	rv = drv->ops->write(device, block_offset, entry->data);
	
	/* If successful clear the DIRTY flag from the cache entry */
	if (!rv)
		entry->flags &= ~BLKCACHE_ENTRY_FLAG_DIRTY;

	return rv;
}

/**
 * @brief Fetch a block from storage and add it to the cache
 * @param device The device id to operate on
 * @param block_offset The starting offset of the block to fetch
 * @return 0 if successful, a valid errorcode if not
 * @exception ENXIO _device_ does not refer to a valid block device
 */

int device_block_fetch(dev_t device, aoff_t block_offset)
{
	blkcache_entry_t *entry;
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENXIO;
	}

	/* Get the block from the cache, if it does not exist it will be added */
	entry = blkcache_get(drv->caches[minor], block_offset);

	/* Check if the cache was full */
	if (!entry) {
		/* If it is, flush the discard candidate */
		device_block_flush(device,
			blkcache_get_discard_candidate(drv->caches[minor])->offset);

		/* blkcache_get will flush the discard candidate if the cache
                 * is full but only if it is not DIRTY, we have just flushed it
                 * so it will not be. */
		entry = blkcache_get(drv->caches[minor], block_offset);

	}

	/* Check if the cache ran out of memory */
	if (entry == BLKCACHE_ENOMEM)
		return ENOMEM;
	assert(entry != NULL);

	/* Call the driver to actually read the data */
	//TODO: Invalidate cache entry if this fails
	return drv->ops->read(device, block_offset, entry->data);

}

/**
 * @brief Handles the ioctl(2) call on a device
 * 
 * This function merely dispatches the call to the driver
 * @see _sys_ioctl for more information
 * @param device The device this call is to be performed on
 * @param fd The fd used for the call
 * @param func The function to invoke
 * @param arg An argument for the function
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENOTTY _device_ does not refer to a known device
 */

int device_block_ioctl(dev_t device, int fd, int func, int arg)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENXIO;
	}

	return drv->ops->ioctl(device, fd, func, arg);
}

/**
 * @brief Notifies the device of an open(2) call
 * 
 * This function merely dispatches the call to the driver
 * @see _sys_open for more information
 * @param device The device this call is to be performed on
 * @param fd The fd used for the call
 * @param options The open flags
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_block_open(dev_t device, int fd, int options)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	/* Call the driver */
	return drv->ops->open(device, fd, options);
}

/**
 * @brief Notifies the device of an close(2) call
 * 
 * This function merely dispatches the call to the driver
 * @see _sys_close for more information
 * @param device The device this call is to be performed on
 * @param fd The fd used for the call
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_block_close(dev_t device, int fd)
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	blk_dev_t *drv = block_dev_table[major];

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	/* Call the driver */
	return drv->ops->close(device, fd);
}

/**
 * @brief Write data to a block device
 *
 * @see vfs_write for more information
 * @param device The device this call is to be performed on
 * @param file_offset The offset in the file to write to
 * @param buffer The buffer the data to be written resides in
 * @param count The number of bytes to write
 * @param write_size Output parameter for the number of bytes actually written
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

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

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		debugcon_printf("invalid %i %i\n", (int)major, (int)minor);
		return ENODEV;
	}

	in_buffer = 0;
	
	/* Acquire a lock on the device */
	semaphore_down(drv->locks[minor]);
	assert(*(drv->locks[minor]) == 0);

	/* Loop while we still have data left to write */
	while (in_buffer != ((uintptr_t) count)) {

		/* Calculate the offset into the block for this chunk */
		in_block  = (file_offset + in_buffer) % drv->block_size;
	
		/* Calculate the block we are writing to */
		block_offset = (file_offset + in_buffer) - in_block;

		/* Calculate the number of bytes to write within this block */
		in_block_count = count - in_buffer;

		/* Check whether we want to write more data than would fit,
                 * if so, limit to the end of the block */
		if(in_block_count > (drv->block_size - in_block))
			in_block_count = drv->block_size - in_block;	
		
		/* Check whether we are replacing the whole block */
		if (in_block_count != drv->block_size) {
			/* We are not, read-modify-write needed */
		
			/* Get the block from the cache */
			entry = blkcache_find(drv->caches[minor], block_offset);

			/* Check if it was cached */
			if (!entry) {
				/* It was not, fetch it from storage*/

				rv = device_block_fetch(device, block_offset);

				/* Check for errors */
				if (rv) {
					/* Release the lock on this device */
					semaphore_up(drv->locks[minor]);

					/* Pass the error to the caller */
					return rv;
				}
			}			
		
			/* The block should be cached now, get it from the cache */
			entry = blkcache_find(drv->caches[minor], block_offset);
			assert(entry != NULL);

		} else { //Just add it to the cache
			/* We are replacing the whole block */

			/* Get a new block cache entry from the cache */
			entry = blkcache_get(drv->caches[minor], block_offset);

			/* Check if the cache was full */
			if (!entry) {
				/* It is */

				/* Flush the discard candidate */
				device_block_flush(device,
					blkcache_get_discard_candidate(drv->caches[minor])->offset);

				/* Retry */
				entry = blkcache_get(drv->caches[minor], block_offset);

			}		
			
			/* Check whether the cache ran out of memory */
			if (entry == BLKCACHE_ENOMEM) {
				/* Release the lock on this device */
				semaphore_up(drv->locks[minor]);

				/* Pass the error to the caller */
				return ENOMEM;

			}

			assert(entry != NULL);

		}

		/* Copy the block data to the cache entry */
		memcpy( (void *) ( ((uintptr_t)entry->data) + in_block ),
		        (void *) ( ((uintptr_t) buffer) + in_buffer ),
                        in_block_count ); 

		/* Set the dirty flag on the entry */
		entry->flags |= BLKCACHE_ENTRY_FLAG_DIRTY;

		/* Advance position */
		in_buffer += in_block_count;
		block_offset += drv->block_size;
		
		/* Update write_size */
		*write_size = in_buffer;
	}

	/* Release the lock on this device */
	semaphore_up(drv->locks[minor]);

	return 0;
	
	
}

/**
 * @brief Read data from a block device
 *
 * @see vfs_write for more information
 * @param device The device this call is to be performed on
 * @param file_offset The offset in the file to read from
 * @param buffer The buffer to put the data in
 * @param count The number of bytes to read
 * @param read_size Output parameter for the number of bytes actually read
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */
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

	/* Check if the device exists */
	if ((!drv) || (minor > drv->minor_count)) {
		return ENODEV;
	}

	in_buffer = 0;
	
	/* Acquire a lock on the device */
	semaphore_down(drv->locks[minor]);
	assert(*(drv->locks[minor]) == 0);

	/* Loop while we still have data left to read */
	while (in_buffer != ((uintptr_t) count)) {

		/* Calculate the offset into the block for this chunk */
		in_block  = (file_offset + in_buffer) % drv->block_size;
	
		/* Calculate the block we are writing to */
		block_offset = (file_offset + in_buffer) - in_block;

		/* Calculate the number of bytes to write within this block */
		in_block_count = count - in_buffer;

		/* Check whether we want to read more data than the block 
                 * contains. if so, limit to the end of the block */
		if(in_block_count > (drv->block_size - in_block))
			in_block_count = drv->block_size - in_block;	
		
		/* Get the block from the cache */		
		entry = blkcache_find(drv->caches[minor], block_offset);

		/* Check if it was cached */
		if (!entry) {
			/* If not, fetch it and retry */
			rv = device_block_fetch(device, block_offset);

			/* Check for errors */
			if (rv) {
				/* Release the lock on this device */
				semaphore_up(drv->locks[minor]);

				/* Pass the error to the caller */
				return rv;
			}			
		
			/* Retry */
			entry = blkcache_find(drv->caches[minor], block_offset);
		}

		assert(entry != NULL);

		/* Copy the block data from the cache entry */
		memcpy( (void *) ( ((uintptr_t) buffer) + in_buffer ),
			(void *) ( ((uintptr_t)entry->data) + in_block ), 
			in_block_count ); 

		/* Advance position */
		in_buffer += in_block_count;
		block_offset += drv->block_size;

		/* Update read size */
		*read_size = in_buffer;
	}

	/* Release the lock on this device */
	semaphore_up(drv->locks[minor]);

	return 0;
}

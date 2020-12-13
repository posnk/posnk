/**
 * @file kernel/chardev.c
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * @li 17-04-2014 - Created
 * @li 14-07-2014 - Documented
 */
#include "kernel/syscall.h"
#include "kernel/device.h"
#include "kernel/heapmm.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <poll.h>

/**
 * @brief Stores the driver descriptors for each character major device 
 */
char_dev_t **char_dev_table;

/**
 * @brief Initialize the character device interface
 */
void device_char_init()
{
	/* Allocate memory for the device table */
	char_dev_table = heapmm_alloc(sizeof(char_dev_t *) * 256);
	/* Clear the device table */
	memset(char_dev_table, 0, sizeof(char_dev_t *) * 256);
}

/**
 * @brief Register a new character device driver
 * @see char_dev for more information on character device drivers
 * @param driver The driver descriptor for the driver to register
 * @return When successful 1 is returned, otherwise 0 will be returned
 */
int device_char_register(char_dev_t *driver)
{
	/* Check for null pointers */
	if (!driver)
		return 0;
	/* Check if the device already exists */
	if (char_dev_table[driver->major])
		return 0;
	/* Register the driver */
	char_dev_table[driver->major] = driver;
	return 1;
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

int device_char_ioctl(dev_t device, int fd, int func, int arg)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENOTTY;
	}

	/* Call the driver */
	return drv->ops->ioctl(device, fd, func, arg);
}

/**
 * @brief Notifies the device of a dup(2) call
 * 
 * This function merely dispatches the call to the driver
 * @see _sys_dup for more information
 * @param device The device this call is to be performed on
 * @param fd The stream ptr used for the call
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_char_dup(dev_t device, stream_ptr_t *fd)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}

	/* Call the driver */
	if ( drv->ops->dup )
		return drv->ops->dup(device, fd);

	return 0;
}

/**
 * @brief Notifies the device of an open(2) call
 * 
 * This function merely dispatches the call to the driver
 * @see _sys_open for more information
 * @param device The device this call is to be performed on
 * @param fd The stream ptr used for the call
 * @param options The open flags
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_char_open(dev_t device, stream_ptr_t *fd, int options)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}

	/* Call the driver */
	if ( !drv->ops->open )
		return drv->ops->open_new(device, fd, options);
	else
		return drv->ops->open(device, fd->id, options);
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

int device_char_close(dev_t device, stream_ptr_t *fd)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}

	/* Call the driver */
	if ( !drv->ops->close )
		return drv->ops->close_new(device, fd);
	else
		return drv->ops->close(device, fd->id);
}

/**
 * @brief Write data to a character device
 *
 * This function merely dispatches the call to the driver
 * @see vfs_write for more information
 * @param device The device this call is to be performed on
 * @param file_offset The offset in the file to write to
 * @param buffer The buffer the data to be written resides in
 * @param count The number of bytes to write
 * @param write_size Output parameter for the number of bytes actually written
 * @param non_block If true, the call will not block when no room is available
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_char_write(dev_t device, __attribute__((__unused__)) aoff_t file_offset, const void * buffer, aoff_t count, aoff_t *write_size, int non_block)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}
	/* Call the driver */
	return drv->ops->write(device, buffer, count, write_size, non_block);
}

/**
 * @brief Read data from a character device
 *
 * This function merely dispatches the call to the driver
 * @see vfs_write for more information
 * @param device The device this call is to be performed on
 * @param file_offset The offset in the file to read from
 * @param buffer The buffer to put the data in
 * @param count The number of bytes to read
 * @param read_size Output parameter for the number of bytes actually read
 * @param non_block If true, the call will not block when no data is available
 * @return 0 on success, If an error occurs a valid error code will be returned
 * @exception ENXIO _device_ does not refer to a known device
 */

int device_char_read(dev_t device, __attribute__((__unused__)) aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}
	/* Call the driver */
	return drv->ops->read(device, buffer, count, read_size, non_block);
}

int device_char_mmap(dev_t device, int fd, int flags, void *addr, aoff_t offset, aoff_t size)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}
	/* Call the driver */
	return drv->ops->mmap(device, fd, flags, addr, offset, size);
}

int device_char_unmmap(dev_t device, int fd, int flags, void *addr, aoff_t offset, aoff_t size)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return ENXIO;
	}
	/* Call the driver */
	return drv->ops->unmmap(device, fd, flags, addr, offset, size);
}

short int device_char_poll(dev_t device, short int events)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	/* Check if the device exists */
	if (!drv) {
		return POLLERR;
	}
	if (!drv->ops->poll)
		return POLLNVAL;
	/* Call the driver */
	return drv->ops->poll(device, events);
}

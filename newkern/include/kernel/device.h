/**
 * @file kernel/device.h
 * @brief Device driver interface
 *
 * The device driver interface provides a way for device drivers to expose 
 * functionality to the OS and applications. It implements UNIX special 
 * files as the user API and provides a clean callback-based interface for 
 * the drivers. The driver interface also provides a transparent cache 
 * mechanism to allow fully random access to block devices.
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * @li 17-04-2014 - Created
 * @li 14-07-2014 - Documented
 */

#ifndef __KERNEL_DEVICE_H__
#define __KERNEL_DEVICE_H__
#include <sys/types.h>
#include "kernel/blkcache.h"
#include "kernel/synch.h"
#include "kernel/streams.h"

/**
 * @defgroup drvapi Device driver interface
 * @brief Device driver interface
 *
 * The device driver interface provides a way for device drivers to expose 
 * functionality to the OS and applications. It implements UNIX special 
 * files as the user API and provides a clean callback-based interface for 
 * the drivers. The driver interface also provides a transparent cache 
 * mechanism to allow fully random access to block devices.
 * @{
 */

/**
 * @brief Describes a character device driver instance *
 * @see char_dev
 */

typedef struct char_dev	char_dev_t;

/**
 * @brief Describes a block device driver instance *
 * @see blk_dev
 */

typedef struct blk_dev	blk_dev_t;

/** 
 * @brief Contains callbacks for all character device driver functions 
 * @see tty_ops
 */

typedef struct tty_ops	tty_ops_t;

/** 
 * @brief Contains callbacks for all block device driver functions 
 * @see blk_ops
 */

typedef struct blk_ops	blk_ops_t;

/**
 * @brief Describes a character device driver instance
 *
 * Character device drivers use an id that consists of two parts, the
 * major id which selects the driver instance from a table and the minor id 
 * which selects the device handled by the driver. The driver's interface is
 * exposed through a table of callbacks passed in tty_ops 
 */
 
struct char_dev {
	/** The driver name */
	char			 *name;
	/** The major part of the device id this instance handles */
	dev_t			  major;
	/** A pointer to the callback table containing the driver functions */
	tty_ops_t		 *ops;	
};

/** 
 * @brief Contains callbacks for all character device driver functions 
 *
 * This is the structure used to pass the callbacks for a character driver to
 * the kernel, it is referenced by char_dev, which describes an instance of a
 * character driver, implementation hints and requirements are given in the 
 * description of each callback, for more info on character driver 
 * implementation see char_dev
 */

struct tty_ops {

	/**
	 * @brief Hook function called on device open
	 *
	 * This function allows a block device to refuse certain open(2) calls
         * or to handle other implementation-specific behaviour on file open. @n
	 * @n
         * Minimal implementation: @n
	 * Stub returning 0
	 * @param device The device being opened
	 * @param fd The fd handle assigned to the new stream
	 * @param options The flags passed to open(2)
	 * @return An error code on failure, 0 on success
	 */

	int	(*open)			(dev_t, int, int);				//device, fd, options
	int	(*open_new)		(dev_t, stream_ptr_t *, int);	//device, fd, options

	/**
	 * @brief Hook function called on device close
	 *
	 * This function allows a to handle implementation-specific behaviour 
	 * on device close. @n
	 * @n
         * Minimal implementation: @n
	 * Stub returning 0
	 * @param device The device being closed
	 * @param fd The fd handle assigned to the stream being closed
	 * @return An error code on failure, 0 on success
	 */

	int	(*close)	  (dev_t, int);				//device, fd

	/**
	 * @brief Write data to device
 	 * 
	 * It is not required to implement non-blocking IO in the driver
	 * @n
         * Minimal implementation: @n
	 * Stub returning EINVAL
	 * @param device The device to write to
	 * @param buffer The buffer containing the data to write
	 * @oaram count The number of bytes to write
	 * @param write_size Output parameter for the number of bytes actually written
 	 * @param non_block If true, the call will not block when no room is available
	 * @return An error code on failure, 0 on success
	 * @exception EINVAL The device is not suitable for writing
	 */

	int	(*write)	  (dev_t, void *, aoff_t, aoff_t *, int); //device, buf, count, wr_size, non_block

	/**
	 * @brief Read data from device
 	 * 
	 * It is not required to implement non-blocking IO in the driver
	 * @n
         * Minimal implementation: @n
	 * Stub returning EINVAL
	 * @param device The device to read from
	 * @param buffer The buffer to read the data to
	 * @oaram count The number of bytes to read
	 * @param read_size Output parameter for the number of bytes actually read
 	 * @param non_block If true, the call will not block when no data is available
	 * @return An error code on failure, 0 on success
	 * @exception EINVAL The device is not suitable for reading
	 */

	int	(*read)		  (dev_t, void *, aoff_t, aoff_t *, int);	//device, buf, count, rd_size, non_block

	/**
	 * @brief Control device
	 *
	 * This function allows a driver to expose operations other than normal
         * file IO.
	 * @n
         * Minimal implementation: @n
	 * Stub returning EINVAL
	 * @param device The device to operate on
	 * @param fd The fd handle used for the call
	 * @param function The function being invoked
	 * @param arg An argument for the function
	 * @return An error code on failure, 0 on success
	 * @exception EINVAL Invalid value for function
	 */

	int	(*ioctl)	  (dev_t, int, int, int);			//device, fd, func, arg

	int	(*mmap)		  (dev_t, int, int, void *, aoff_t, aoff_t);

	int	(*unmmap)	  (dev_t, int, int, void *, aoff_t, aoff_t);

	short int	(*poll)	  (dev_t, short int);
};

/**
 * @brief Describes a block device driver instance
 *
 * Character device drivers use an id that consists of two parts, the
 * major id which selects the driver instance from a table and the minor id 
 * which selects the device handled by the driver. The driver's interface is
 * exposed through a table of callbacks passed in blk_ops. The main difference
 * between block and character drivers is that block drivers cache the data so
 * that IO only takes place in fixed-size chunks. This behaviour is implemented
 * by the kernel's block device layer and drivers only have to provide 
 * functionality to write whole blocks. 
 */

struct blk_dev {
	/** The driver name */
	char			 *name;
	/** The major part of the device id this instance handles */
	dev_t			  major;
	/** The amount of minor devices this instance handles */
	int			  minor_count;
	/** The size of the blocks handled by this instance */
	aoff_t			  block_size;
	/** The amount of blocks the cache should at most contain */
	int			  cache_size;
	/** Mutex locks for each minor device */
	semaphore_t		**locks;
	/** Block cache instances for each minor device */
	blkcache_cache_t	**caches;
	/** A pointer to the callback table containing the driver functions */
	blk_ops_t		 *ops;	
};

/** 
 * @brief Contains callbacks for all block device driver functions 
 *
 * This is the structure used to pass the callbacks for a block driver to
 * the kernel, it is referenced by blk_dev, which describes an instance of a
 * block driver, implementation hints and requirements are given in the 
 * description of each callback, for more info on block driver implementation 
 * see blk_dev
 */

struct blk_ops {

	/**
	 * @brief Hook function called on device open
	 *
	 * This function allows a block device to refuse certain open(2) calls
         * or to handle other implementation-specific behaviour on file open. @n
	 * @n
         * Minimal implementation: @n
	 * Stub returning 0
	 * @param device The device being opened
	 * @param fd The fd handle assigned to the new stream
	 * @param options The flags passed to open(2)
	 * @return An error code on failure, 0 on success
	 */
		
	int	(*open)		  (dev_t, int, int);			//device, fd, options

	/**
	 * @brief Hook function called on device close
	 *
	 * This function allows a to handle implementation-specific behaviour 
	 * on device close. @n
	 * @n
         * Minimal implementation: @n
	 * Stub returning 0
	 * @param device The device being closed
	 * @param fd The fd handle assigned to the stream being closed
	 * @return An error code on failure, 0 on success
	 */

	int	(*close)	  (dev_t, int);				//device, fd

	/**
	 * @brief Write block to storage
 	 * 
	 * Writes a single block to storage at the specified linear offset.@n
	 * @n
         * Minimal implementation: @n
	 * Stub returning EINVAL
	 * @param device The device to write to
	 * @param offset The start of the block to write
	 * @param buffer The buffer containing the data to write
	 * @return An error code on failure, 0 on success
	 * @exception EINVAL The device is not suitable for writing
	 */

	int	(*write)	  (dev_t, aoff_t, void *);		//device, offset, buf

	/**
	 * @brief Read block from storage
 	 * 
	 * Reads a single block from storage at the specified linear offset.@n
	 * @n
         * Full implementation required
	 * @param device The device to read from
	 * @param offset The start of the block to read
	 * @param buffer The buffer to read the block to
	 * @return An error code on failure, 0 on success
	 */

	int	(*read)		  (dev_t, aoff_t, void *); 		//device, offset, buf

	/**
	 * @brief Control device
	 *
	 * This function allows a driver to expose operations other than normal
         * file IO.
	 * @n
         * Minimal implementation: @n
	 * Stub returning EINVAL
	 * @param device The device to operate on
	 * @param fd The fd handle used for the call
	 * @param function The function being invoked
	 * @param arg An argument for the function
	 * @return An error code on failure, 0 on success
	 * @exception EINVAL Invalid value for function
	 */
	int	(*ioctl)	  (dev_t, int, int, int);		//device, fd, func, arg

};

void device_char_init();

void device_block_init();

int device_char_register(char_dev_t *driver);

int device_block_register(blk_dev_t *driver);

int device_block_ioctl(dev_t device, int fd, int func, int arg);

int device_block_open(dev_t device, int fd, int options);

int device_block_close(dev_t device, int fd);

int device_block_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size);

int device_block_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size);

int device_block_flush_all(dev_t device);

int device_block_flush_global();

int device_char_ioctl(dev_t device, int fd, int func, int arg);

int device_char_open(dev_t device, stream_ptr_t *fd, int options);

int device_char_close(dev_t device, int fd);

int device_char_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size, int non_block);

int device_char_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

int device_char_mmap(dev_t device, int fd, int flags, void *addr, aoff_t offset, aoff_t file_sz);

int device_char_unmmap(dev_t device, int fd, int flags, void *addr, aoff_t offset, aoff_t file_sz);

short int device_char_poll(dev_t device, short int events);

///@}

#endif

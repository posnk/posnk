/**
 * fs/partition.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-07-2014 - Created
 */

#ifndef __FS_PARTITION_H__
#define __FS_PARTITION_H__

#include <stdint.h>

typedef struct partition_info {
	uint8_t  type;
	uint32_t start;
	uint32_t size;
} partition_info_t;


#endif

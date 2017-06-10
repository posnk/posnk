/**
 * fs/mbr.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-07-2014 - Created
 */

#ifndef __FS_MBR_H__
#define __FS_MBR_H__

#include <stdint.h>
#include "fs/partition.h"

#define MBR_PARTITION_TABLE_START	(0x01BE)
#define MBR_PARTITION_TYPE		(4)
#define MBR_PARTITION_OFFSET		(8)
#define MBR_PARTITION_SIZE		(12)
#define MBR_PARTITION_ENTRY_SIZE	(16)

#define MBR_MAGIC_START			(0x01FE)
#define MBR_MAGIC			(0xAA55)

int mbr_parse(partition_info_t *partition_list, uint8_t *mbr);

#endif

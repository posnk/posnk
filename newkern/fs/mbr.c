/**
 * fs/mbr.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-07-2014 - Created
 */

#include "fs/mbr.h"
#include "kernel/earlycon.h"

int mbr_parse(partition_info_t *partition_list, uint8_t *mbr)
{
	uint16_t magic = *((uint16_t *)&mbr[MBR_MAGIC_START]);
	int part, po;
	if (MBR_MAGIC != magic) {
		debugcon_printf("mbr: invalid partition table magic %x!\n", magic);
		return 0;
	}
	for (part = 0; part < 4; part++) {
		po = part * MBR_PARTITION_ENTRY_SIZE + MBR_PARTITION_TABLE_START;
		partition_list[part].type = mbr[po + MBR_PARTITION_TYPE];
		partition_list[part].start = *((uint32_t *)&mbr[po + MBR_PARTITION_OFFSET]);
		partition_list[part].size = *((uint32_t *)&mbr[po + MBR_PARTITION_SIZE]);
		debugcon_printf("mbr: partition %i type %i starts at %i, has size %i!\n", part, partition_list[part].type,partition_list[part].start,partition_list[part].size);
	}

	return 1;
}

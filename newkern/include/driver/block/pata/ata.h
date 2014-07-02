/**
 * driver/block/pata/ata.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-07-2014 - Created
 */

#ifndef __DRIVER_BLOCK_PATA_ATA_H__
#define __DRIVER_BLOCK_PATA_ATA_H__

#define ATA_PRIMARY_PORT_BASE		(0x1F0)
#define ATA_PRIMARY_STATUS_PORT		(0x3F6)
#define ATA_SECONDARY_PORT_BASE		(0x170)
#define ATA_SECONDARY_STATUS_PORT	(0x376)

#define ATA_DATA_PORT			(0)
#define ATA_FEATURE_PORT		(1)
#define ATA_SECTOR_COUNT_PORT		(2)
#define ATA_SECTOR_NO_PORT		(3)
#define ATA_CYL_LOW_PORT		(4)
#define ATA_CYL_HIGH_PORT		(5)
#define ATA_DRIVE_HEAD_PORT		(6)
#define ATA_CMD_STATUS_PORT		(7)

#define ATA_STATUS_FLAG_ERR		(1<<0)
#define ATA_STATUS_FLAG_DRQ		(1<<3)
#define ATA_STATUS_FLAG_SRV		(1<<4)
#define ATA_STATUS_FLAG_DF		(1<<5)
#define ATA_STATUS_FLAG_RDY		(1<<6)
#define ATA_STATUS_FLAG_BSY		(1<<7)

#define ATA_DCR_FLAG_INT_DIS		(1<<1)
#define ATA_DCR_FLAG_SOFTRESET		(1<<2)
#define ATA_DCR_FLAG_HIGHORDER		(1<<7)


#endif

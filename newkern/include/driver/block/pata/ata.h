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

#include <stdint.h>
#include "kernel/interrupt.h"
#include "kernel/synch.h"
#include "fs/partition.h"

#define ATA_DATA_PORT			(0)

#define ATA_FEATURE_PORT		(1)
#define ATA_ERROR_PORT			(1)

#define ATA_SECTOR_NO_PORT		(3)
#define ATA_CYL_LOW_PORT		(4)
#define ATA_CYL_HIGH_PORT		(5)
#define ATA_DRIVE_HEAD_PORT		(6)

#define ATA_SECTOR_COUNT_LOW_PORT	(2)
#define ATA_SECTOR_COUNT_HIGH_PORT	(8)

#define ATA_LBA_0_PORT			(3)
#define ATA_LBA_1_PORT			(4)
#define ATA_LBA_2_PORT			(5)
#define ATA_LBA_3_PORT			(9)
#define ATA_LBA_4_PORT			(10)
#define ATA_LBA_5_PORT			(11)

#define ATA_COMMAND_PORT		(7)
#define ATA_STATUS_PORT			(7)
#define ATA_CONTROL_PORT		(12)
#define ATA_ALTSTATUS_PORT		(12)

#define ATA_DEVADDRESS_PORT		(13)

#define ATA_BUSMASTER_COMMAND_PORT	(14)
#define ATA_BUSMASTER_STATUS_PORT	(16)
#define ATA_BUSMASTER_PRDT_PTR_PORT	(18)

#define ATA_BM_CMD_FLAG_DMA_ENABLE	(1<<0)
#define ATA_BM_CMD_FLAG_READ		(1<<3)

#define ATA_BM_STATUS_FLAG_SIMPLEX	(1<<7)
#define ATA_BM_STATUS_FLAG_S_DMA	(1<<6)
#define ATA_BM_STATUS_FLAG_M_DMA	(1<<5)
#define ATA_BM_STATUS_FLAG_IREQ		(1<<2)
#define ATA_BM_STATUS_FLAG_ERR		(1<<1)
#define ATA_BM_STATUS_FLAG_DMAGO	(1<<0)

#define ATA_STATUS_FLAG_ERR		(1<<0)
#define ATA_STATUS_FLAG_DRQ		(1<<3)
#define ATA_STATUS_FLAG_SRV		(1<<4)
#define ATA_STATUS_FLAG_DF		(1<<5)
#define ATA_STATUS_FLAG_RDY		(1<<6)
#define ATA_STATUS_FLAG_BSY		(1<<7)

#define ATA_ERROR_FLAG_BBK		(1<<7)
#define ATA_ERROR_FLAG_UNC		(1<<6)
#define ATA_ERROR_FLAG_MC		(1<<5)
#define ATA_ERROR_FLAG_IDNF		(1<<4)
#define ATA_ERROR_FLAG_MCR		(1<<3)
#define ATA_ERROR_FLAG_ABRT		(1<<2)
#define ATA_ERROR_FLAG_TK0NF		(1<<1)
#define ATA_ERROR_FLAG_AMNF		(1<<0)

#define ATA_DCR_FLAG_INT_DIS		(1<<1)
#define ATA_DCR_FLAG_SOFTRESET		(1<<2)
#define ATA_DCR_FLAG_HIGHORDER		(1<<7)

#define ATA_CMD_READ_PIO		(0x20)
#define ATA_CMD_READ_PIO_EXT		(0x24)
#define ATA_CMD_READ_DMA		(0xC8)
#define ATA_CMD_READ_DMA_EXT		(0x25)
#define ATA_CMD_WRITE_PIO		(0x30)
#define ATA_CMD_WRITE_PIO_EXT		(0x34)
#define ATA_CMD_WRITE_DMA		(0xCA)
#define ATA_CMD_WRITE_DMA_EXT		(0x35)
#define ATA_CMD_CACHE_FLUSH		(0xE7)
#define ATA_CMD_CACHE_FLUSH_EXT		(0xEA)
#define ATA_CMD_PACKET			(0xA0)
#define ATA_CMD_IDENTIFY_PACKET		(0xA1)
#define ATA_CMD_IDENTIFY		(0xEC)

#define ATA_IDENT_DEVICETYPE		(0)
#define ATA_IDENT_CYLINDERS		(2)
#define ATA_IDENT_HEADS			(6)
#define ATA_IDENT_SECTORS		(12)
#define ATA_IDENT_SERIAL		(20)
#define ATA_IDENT_MODEL			(54)
#define ATA_IDENT_CAPABILITIES		(98)
#define ATA_IDENT_FIELDVALID		(106)
#define ATA_IDENT_MAX_LBA		(120)
#define ATA_IDENT_COMMANDSETS		(164)
#define ATA_IDENT_MAX_LBA_EXT		(200)

#define ATA_IDENT_SHORT(DV, DR, PT)	(* ( (uint16_t *) &(DV->drives[DR].ident_data[PT]) ) )
#define ATA_IDENT_LONG(DV, DR, PT)	(* ( (uint32_t *) &(DV->drives[DR].ident_data[PT]) ) )
#define ATA_IDENT_LLONG(DV, DR, PT)	(* ( (uint64_t *) &(DV->drives[DR].ident_data[PT]) ) )

#define ATA_IDENT_CMDSET_FLAG_LBA48	(1<<26)
#define ATA_IDENT_CAP_FLAG_DMA		(1<<8)
#define ATA_IDENT_CAP_FLAG_LBA		(1<<9)

#define ATA_DRVSEL_LBA(ndrv, buppr)		(0xE0 | ( ((ndrv)&1) << 4) | ((buppr)&0xF) )
#define ATA_DRVSEL_CHS(ndrv, nhead)		(0xA0 | ( ((ndrv)&1) << 4) | ((nhead)&0xF) )

#define ATA_PRD_END_OF_LIST		(0x8000)

#define ATA_DRIVE_NONE			(0)
#define ATA_DRIVE_ATA			(1)
#define ATA_DRIVE_ATAPI			(2)

#define ATA_MODE_CHS			(0)
#define ATA_MODE_LBA28			(1)
#define ATA_MODE_LBA48			(2)

#define ATA_PRD_LIST_SIZE		(16)

typedef struct ata_device ata_device_t;

typedef struct ata_drive ata_drive_t;

typedef struct ata_prd ata_prd_t;

typedef uint64_t ata_lba_t;

struct ata_drive {
	int		type;
/* Identity */
	uint8_t		ident_data[512];
	char		model[41];
	char		serial[21];
/* Features */
	uint16_t	capabilities;
	uint16_t	command_sets;
/* Geometry */
	uint16_t	cylinders;
	uint16_t	heads;
	uint16_t	sectors_per_track;
/* Size */
	ata_lba_t	max_lba;

/* Operation mode */
	int		lba_mode;

/* Partitions */
	partition_info_t partitions[31];
	
};

struct ata_device {
	int		 bus_number;
	uint16_t	 pio_base;
	uint16_t	 ctrl_base;
	uint16_t	 bmio_base;
	irq_id_t	 irq;
	uint8_t		 ctrl_reg;
	ata_drive_t	 drives[2];
	semaphore_t	*lock;
	semaphore_t	*int_wait;
	uint8_t		 int_status;
};

struct ata_prd {
	uint32_t	buffer_phys;
	uint16_t	byte_count;
	uint16_t	end_of_list;
}  __attribute__((packed));


uint8_t ata_read_port(ata_device_t *device, uint16_t port);

void ata_write_port(ata_device_t *device, uint16_t port, uint8_t value);

int ata_poll_wait(ata_device_t *device);

void ata_set_interrupts(ata_device_t *device, int enabled);

void ata_initialize(ata_device_t *device);
int ata_write(ata_device_t *device, int drive, ata_lba_t lba, uint8_t *buffer, uint16_t count);
int ata_read(ata_device_t *device, int drive, ata_lba_t lba, uint8_t *buffer, uint16_t count);
#endif

/**
 * @file driver/block/pata/ata.c
 *
 * Implements the functionality common to all ATA ports
 *
 * Part of P-OS driver library.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 02-07-2014 - Created
 * \li 20-01-2015 - Commented
 */

#include "config.h"
#include <sys/errno.h>

#include "arch/i386/x86.h"
#include "driver/block/pata/ata.h"
#define CON_SRC "ata"
#include "kernel/console.h"
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "fs/mbr.h"

#define ATA_READ_TIMEOUT  ( 10000000UL )
#define ATA_WRITE_TIMEOUT ( 10000000UL )

int ata_bus_number_counter = 0;
int ata_interrupt_enabled = 0;
int ata_global_inited = 0;

ata_device_t **ata_buses;

volatile ata_prd_t *ata_prd_list;

extern blk_ops_t ata_block_driver_ops;

void ata_do_wait(ata_device_t *device)
{
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
}

int ata_poll_status( ata_device_t *device )
{
	uint8_t status;

	ata_do_wait( device );

	if ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY)
		return 0;

	if (status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR))
		return 0;
	if (status & ATA_STATUS_FLAG_DRQ)
		return 1;
	else
		return 2;

}

/**
 * @brief Waits for the ATA device to become available
 * @param device ATA device to wait on
 * @return If successful and DRQ was set, the function will return 1, if DRQ
 * 	   was not set the function will return 2 unless an error occurred, in which case
 *         the function will return 0
 *
 */
int ata_poll_mtwait(ata_device_t *device)
{
	uint8_t status;
	int a = 0x10000000;//TODO: CONSTANT

	ata_do_wait( device );

	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY){
		if (--a <= 0)
			return 0;
	}

	return ata_poll_status( device );
}

int ata_poll_wait_resched(ata_device_t *device)
{
	uint8_t status;
	ktime_t timeout_end = system_time + 2;//TODO: CONSTANT

	ata_do_wait( device );

	if ( device->ctrl_reg & ATA_DCR_FLAG_INT_DIS )
		return ata_poll_mtwait( device );

	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY) {
		if (system_time > timeout_end)
			return 0;
		schedule();
	}

	return ata_poll_status( device );
}

/**
 * @brief Waits for the ATA device to become available
 * @param device ATA device to wait on
 * @return If successful and DRQ was set, the function will return 1, if DRQ
 * 	   was not set the function will return 2 unless an error occurred, in which case
 *         the function will return 0
 *
 */
int ata_poll_wait(ata_device_t *device)
{
	uint8_t status;
	ktime_t timeout_end = system_time + 2;//TODO: CONSTANT

	ata_do_wait( device );

	if ( device->ctrl_reg & ATA_DCR_FLAG_INT_DIS )
		return ata_poll_mtwait( device );

	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY){
		if (system_time > timeout_end)
			return 0;
	}

	return ata_poll_status( device );
}

/**
 * @brief Enables or disables interrupts for an ATA device
 * @param device The ATA device to operate on
 * @param enabled Whether to enable interrupts on the device
 */
void ata_set_interrupts(ata_device_t *device, int enabled)
{
	device->ctrl_reg = enabled ? 0 : ATA_DCR_FLAG_INT_DIS;
	ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
}

int ata_irq_handler(__attribute__((__unused__)) irq_id_t irq_id, void *context)
{
	int bstatus;
	ata_device_t *device = context;
	device->int_status = ata_read_port(device, ATA_STATUS_PORT);
	if (device->bmio_base) {
		bstatus = ata_read_port(device,ATA_BUSMASTER_STATUS_PORT);
//		debugcon_printf("ataduimoisr 0x%x",bstatus);
//		debugcon_printf("dstatus : 0x%x", device->int_status);
		if (!(bstatus & ATA_BM_STATUS_FLAG_IREQ)){
			return 0;//Forward interrupt to next handlers
		}
		ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_IREQ);
//		if (!(bstatus& ATA_BM_STATUS_FLAG_DMAGO))
//			ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0);

	}
	semaphore_up(device->int_wait);
	return 1;
}

void ata_global_initialize()
{
	ata_global_inited = 1;
	ata_buses = heapmm_alloc(sizeof(ata_device_t *) * 4);
}

void ata_load_partition_table(ata_device_t *device, int drive)
{
	uint8_t mbr[512];
	ata_read(device, drive, 0, mbr, 1);
	mbr_parse(device->drives[drive].partitions, mbr);
}

void ata_initialize(ata_device_t *device)
{
	int drive, _t, dmacap = 0;
	blk_dev_t *drv;

	if (!ata_global_inited)
		ata_global_initialize();

	ata_set_interrupts(device, 0); //Disable interrupts for this device_id
	interrupt_register_handler(device->irq, &ata_irq_handler, device);

	device->bus_number = ata_bus_number_counter++;
	device->lock = semaphore_alloc();
	device->int_wait = semaphore_alloc();

	ata_buses[device->bus_number] = device;

	for (drive = 0; drive < 2; drive++) {
		device->drives[drive].type = ATA_DRIVE_NONE;
		ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_CHS(drive, 0));//Select drive
		ata_do_wait(device);
		ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_IDENTIFY);//Execute #IDENTIFY
		ata_do_wait(device);
		if (!ata_read_port(device, ATA_STATUS_PORT))
			continue;//No drive found
		if (ata_poll_wait(device)) {
			//ATA device detected
			device->drives[drive].type = ATA_DRIVE_ATA;
			ata_read_data(device, ATA_DATA_PORT, device->drives[drive].ident_data, 512);
			device->drives[drive].capabilities	= ATA_IDENT_SHORT(device, drive, ATA_IDENT_CAPABILITIES );
			device->drives[drive].command_sets	= ATA_IDENT_SHORT(device, drive, ATA_IDENT_COMMANDSETS );

			device->drives[drive].cylinders		= ATA_IDENT_SHORT(device, drive, ATA_IDENT_CYLINDERS );
			device->drives[drive].heads		= ATA_IDENT_SHORT(device, drive, ATA_IDENT_HEADS);
			device->drives[drive].sectors_per_track	= ATA_IDENT_SHORT(device, drive, ATA_IDENT_SECTORS);

			if (device->drives[drive].command_sets & ATA_IDENT_CMDSET_FLAG_LBA48) {
				device->drives[drive].max_lba   = ATA_IDENT_LLONG(device, drive, ATA_IDENT_MAX_LBA_EXT);
				device->drives[drive].lba_mode  = ATA_MODE_LBA48;
			} else if (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_LBA) {
				device->drives[drive].max_lba   = ATA_IDENT_LONG(device, drive, ATA_IDENT_MAX_LBA);
				device->drives[drive].lba_mode  = ATA_MODE_LBA28;
			} else {
#ifndef HAVE_LIBGCC
				printf(CON_ERROR, "%i:%i doesn't support LBA and kernel was compiled w/o libgcc, disabling it", device->bus_number, drive);
				device->drives[drive].type = ATA_DRIVE_NONE;
#endif
				device->drives[drive].lba_mode  = ATA_MODE_CHS;
			}

			for (_t = 0; _t < 40; _t += 2) {
				device->drives[drive].model[_t] = (char) device->drives[drive].ident_data[ATA_IDENT_MODEL + _t + 1];
				device->drives[drive].model[_t + 1] = (char) device->drives[drive].ident_data[ATA_IDENT_MODEL + _t];
			}
			device->drives[drive].model[40] = '\0';
			for (_t = 0; _t < 20; _t += 2) {
				device->drives[drive].serial[_t] = (char) device->drives[drive].ident_data[ATA_IDENT_SERIAL + _t + 1];
				device->drives[drive].serial[_t + 1] = (char) device->drives[drive].ident_data[ATA_IDENT_SERIAL + _t];
			}
			device->drives[drive].serial[20] = '\0';
			printf(CON_INFO, "detected device: %i:%i which is a %s with serial %s", device->bus_number, drive, device->drives[drive].model, device->drives[drive].serial);
			semaphore_up(device->lock);
			ata_load_partition_table(device, drive);
			semaphore_down(device->lock);
			if (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA)
				dmacap |= 1;
		} else {
			//Possibly detected an ATAPI device
			//TODO: Handle ATAPI devices
			printf(CON_INFO, "detected device: %i:%i which is probably an ATAPI device", device->bus_number, drive);
			continue;//Ignoring this one
		}
	}
	if (dmacap) {
		ata_prd_list = heapmm_alloc_alligned(ATA_PRD_LIST_SIZE * sizeof(ata_prd_t), 8);
		if (ata_prd_list) {
			ata_write_port_long(device, ATA_BUSMASTER_PRDT_PTR_PORT, paging_get_physical_address( (void *) ata_prd_list));
			printf(CON_DEBUG, "allocated DMA prd list at %x", ata_prd_list);
		} else {
			device->drives[drive].capabilities &= ~ATA_IDENT_CAP_FLAG_DMA;
		}
	}
	ata_set_interrupts(device, 1);
	semaphore_up(device->lock);


	drv = heapmm_alloc(sizeof(blk_dev_t));
	drv->name = "ATA block driver";
	drv->major = 0x10 + device->bus_number;
	drv->minor_count = 64;//2*32 partitions
	drv->block_size = 512;
	drv->cache_size = 16;
	drv->ops = &ata_block_driver_ops;
	device_block_register(drv);
}

void ata_setup_lba_transfer(ata_device_t *device, int drive, ata_lba_t lba, uint16_t count)
{
#ifdef HAVE_LIBGCC
	uint32_t cylinder, temp, head, sector;
#endif
	switch (device->drives[drive].lba_mode) {
		case ATA_MODE_LBA48:
			ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_LBA(drive, 0));
			ata_do_wait( device );
			ata_write_port(device, ATA_SECTOR_COUNT_LOW_PORT, count & 0xFF);
			ata_write_port(device, ATA_LBA_0_PORT, lba & 0xFF);
			ata_write_port(device, ATA_LBA_1_PORT, (lba >> 8) & 0xFF);
			ata_write_port(device, ATA_LBA_2_PORT, (lba >> 16) & 0xFF);
			ata_write_port(device, ATA_SECTOR_COUNT_HIGH_PORT, (count >> 8) & 0xFF);
			ata_write_port(device, ATA_LBA_3_PORT, (lba >> 24) & 0xFF);
			ata_write_port(device, ATA_LBA_4_PORT, (lba >> 32) & 0xFF);
			ata_write_port(device, ATA_LBA_5_PORT, (lba >> 40) & 0xFF);
			break;
		case ATA_MODE_LBA28:
			ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_LBA(drive, (lba >> 24) & 0xF ));
			ata_do_wait( device );
			ata_write_port(device, ATA_SECTOR_COUNT_LOW_PORT, count & 0xFF);
			ata_write_port(device, ATA_LBA_0_PORT, lba & 0xFF);
			ata_write_port(device, ATA_LBA_1_PORT, (lba >> 8) & 0xFF);
			ata_write_port(device, ATA_LBA_2_PORT, (lba >> 16) & 0xFF);
			if (count == 0x100)
				count = 0;
			else if (count > 0x100)
				printf(CON_ERROR,"invalid sector count specified for LBA28 drive!");
			break;
		case ATA_MODE_CHS:
#ifdef HAVE_LIBGCC
			cylinder = lba / (device->drives[drive].heads * device->drives[drive].sectors_per_track);
			temp = lba % (device->drives[drive].heads * device->drives[drive].sectors_per_track);
			head = temp / device->drives[drive].sectors_per_track;
			sector = temp % device->drives[drive].sectors_per_track + 1;
			ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_CHS(drive, head & 0xF));
			ata_do_wait( device );
			ata_write_port(device, ATA_SECTOR_COUNT_LOW_PORT, count & 0xFF);
			ata_write_port(device, ATA_CYL_LOW_PORT, cylinder & 0xFF);
			ata_write_port(device, ATA_CYL_HIGH_PORT, (cylinder >> 8) & 0xFF);
			ata_write_port(device, ATA_SECTOR_NO_PORT, sector & 0xFF);
#endif
			break;
	}
}

int ata_read(ata_device_t *device, int drive, ata_lba_t lba, uint8_t *buffer, uint16_t count)
{
	size_t byte_count = count * 512;
	int status,dstatus,ws;
	int dma = (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) && ata_interrupt_enabled;
	semaphore_down(device->lock);

	if ( dma ) {

		if ( ata_interrupt_enabled )
			ata_poll_wait_resched( device );
		else
			ata_poll_wait( device );

		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0x00 );
		ata_prd_list[0].buffer_phys = paging_get_physical_address( (void *) buffer);
		ata_prd_list[0].byte_count = byte_count;
		ata_prd_list[0].end_of_list = ATA_PRD_END_OF_LIST;
//		debugcon_printf("bufferphys: %x\t bytecount: %i",ata_prd_list[0].buffer_phys,
//			ata_prd_list[0].byte_count);
		ata_write_port_long(device, ATA_BUSMASTER_PRDT_PTR_PORT, paging_get_physical_address( (void *) ata_prd_list));
		ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, 0x06 );
		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0x08 );

		if ( ata_interrupt_enabled )
			ata_poll_wait_resched( device );
		else
			ata_poll_wait( device );

	}

	ata_setup_lba_transfer(device, drive, lba, count);

	if ( ata_interrupt_enabled )
		ata_poll_wait_resched( device );
	else
		ata_poll_wait( device );

	*(device->int_wait) = 0;

#ifdef CONFIG_ATA_DEBUG
	printf(CON_TRACE, "read  %i:%i[%x]->M[%x] * %i blocks, DMA:%i lock:%i", device->bus_number, drive, (uint32_t)lba, buffer,(uint32_t) count, dma, *device->lock);
#endif

	if (dma) {
		*device->int_wait = 0;
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_DMA_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_DMA);
		ata_do_wait( device );
		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, ATA_BM_CMD_FLAG_DMA_ENABLE | ATA_BM_CMD_FLAG_READ);
		do {
		//TODO: Implement a work queue
			dstatus = ata_read_port(device, ATA_STATUS_PORT );
			if ( ~dstatus & ATA_STATUS_FLAG_BSY )
				break;
			ws = semaphore_ndown( device->int_wait, ATA_READ_TIMEOUT, SCHED_WAITF_TIMEOUT );
			if ( ws ) {
				printf(CON_ERROR,"device %i:%i DMA read timeout", device->bus_number, drive);
				status = ata_read_port(device, ATA_BUSMASTER_STATUS_PORT );
//				debugcon_printf("mstatus : 0x%x", status);
				dstatus = ata_read_port(device, ATA_STATUS_PORT );
//				debugcon_printf("dstatus : 0x%x", dstatus);
				semaphore_up(device->lock);
				return 0;
			}
			dstatus = ata_read_port(device, ATA_STATUS_PORT );
//			debugcon_printf("dstatus : 0x%x", dstatus);
			status = ata_read_port(device, ATA_BUSMASTER_STATUS_PORT );
//			debugcon_printf("bstatus : 0x%x", status);
		} while ( status & ATA_BM_STATUS_FLAG_DMAGO && dstatus & ATA_STATUS_FLAG_BSY );
		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0x00 );
			status = ata_read_port(device, ATA_BUSMASTER_STATUS_PORT );
		if ( status & ATA_BM_STATUS_FLAG_ERR) {
			ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_ERR);
			printf(CON_ERROR,"device %i:%i DMA read error", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		//scheduler_wait_micros(system_time_micros + 10);
		semaphore_up(device->lock);
		return 1;
	} else {
		*device->int_wait = 0;
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO);

		ata_do_wait( device );

		if (ata_interrupt_enabled) {

			if (semaphore_ndown( device->int_wait, ATA_READ_TIMEOUT, SCHED_WAITF_TIMEOUT )) {
				device->int_status = ata_read_port(device, ATA_STATUS_PORT);
				printf(CON_WARN, "device %i:%i PIO read int timeout", device->bus_number, drive);
				if ( device->int_status & ATA_STATUS_FLAG_BSY ) {
					printf(CON_ERROR,"device %i:%i PIO read timeout", device->bus_number, drive);
					semaphore_up(device->lock);
					return 0;
				}
			}
			if (device->int_status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR)){
				printf(CON_ERROR,"device %i:%i PIO read error", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (!(device->int_status & ATA_STATUS_FLAG_DRQ)){
				printf(CON_ERROR,"device %i:%i PIO read no DRQ!", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
		} else {
			if (ata_poll_wait_resched(device) != 1) {
				printf(CON_ERROR,"device %i:%i PIO read error %i", device->bus_number, drive, lba);
				semaphore_up(device->lock);
				return 0;
			}
		}
		ata_read_data(device, ATA_DATA_PORT, buffer, byte_count);
		semaphore_up(device->lock);
		return 1;
	}
}

int ata_write(ata_device_t *device, int drive, ata_lba_t lba, uint8_t *buffer, uint16_t count)
{
	size_t byte_count = count * 512;
	int dma = 0;//(device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) && ata_interrupt_enabled;
	semaphore_down(device->lock);

	if ( dma ) {

		if ( ata_interrupt_enabled )
			ata_poll_wait_resched( device );
		else
			ata_poll_wait( device );

		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0x00 );

		if ( ata_interrupt_enabled )
			ata_poll_wait_resched( device );
		else
			ata_poll_wait( device );

	}

	ata_setup_lba_transfer(device, drive, lba, count);

	if ( ata_interrupt_enabled )
		ata_poll_wait_resched( device );
	else
		ata_poll_wait( device );

	*(device->int_wait) = 0;
#ifdef CONFIG_ATA_DEBUG
	printf(CON_TRACE,"write %i:%i[%x]<-M[%x] * %i blocks, DMA:%i", device->bus_number, drive, (uint32_t)lba, buffer,(uint32_t) count, dma);
#endif
	if (dma) {
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_WRITE_DMA_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_WRITE_DMA);
		ata_do_wait(device);
		//TODO: Implement a work queue
		ata_prd_list[0].buffer_phys = paging_get_physical_address(buffer);
		ata_prd_list[0].byte_count = byte_count;
		ata_prd_list[0].end_of_list = ATA_PRD_END_OF_LIST;
		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, ATA_BM_CMD_FLAG_DMA_ENABLE);
		ata_do_wait(device);
		if ( semaphore_ndown( device->int_wait, ATA_WRITE_TIMEOUT, SCHED_WAITF_TIMEOUT ) ) {
			printf(CON_ERROR, "device %i:%i DMA write timeout", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		if (ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_ERR) {
			ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_ERR);
			ata_do_wait(device);
			printf(CON_ERROR, "device %i:%i DMA write error", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		//scheduler_wait_micros(system_time_micros + 10);
		semaphore_up(device->lock);
		return 1;
	} else {
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_WRITE_PIO_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_WRITE_PIO);

		if (ata_interrupt_enabled) {
			device->int_status = ata_read_port(device, ATA_STATUS_PORT);
			if ((device->int_status & ATA_STATUS_FLAG_BSY) && semaphore_ndown( device->int_wait, ATA_WRITE_TIMEOUT, SCHED_WAITF_TIMEOUT )  ) {
				device->int_status = ata_read_port(device, ATA_STATUS_PORT);
				if ( device->int_status & ATA_STATUS_FLAG_BSY ) {
					printf(CON_ERROR, "device %i:%i PIO write timeout", device->bus_number, drive);
					semaphore_up(device->lock);
					return 0;
				}
			}
			if (device->int_status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR)){
				printf(CON_ERROR,"device %i:%i PIO write error", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (!(device->int_status & ATA_STATUS_FLAG_DRQ)){
				printf(CON_ERROR,"device %i:%i DMA write no DRQ", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
		} else {
			if (ata_poll_wait_resched(device) != 1) {
				printf(CON_ERROR,"device %i:%i write error %i", device->bus_number, drive, lba);
				semaphore_up(device->lock);
				return 0;
			}
		}

		ata_write_data(device, ATA_DATA_PORT, buffer, byte_count);
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_CACHE_FLUSH_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_CACHE_FLUSH);

		if ( ata_interrupt_enabled )
			ata_poll_wait_resched( device );
		else
			ata_poll_wait( device );
		semaphore_up(device->lock);
		return 1;
	}
}

int ata_blk_open(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options) {return 0;}

int ata_blk_close(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd) {return 0;}

int ata_blk_write(dev_t device, aoff_t file_offset, const void * buffer )
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	ata_device_t *_dev = ata_buses[major - 0x10];
	ata_lba_t lba = ((ata_lba_t) file_offset) >> 9;
	int drive = (minor & 32) ? 1 : 0;

	if ((_dev == NULL) || (major >= 64))
		return ENODEV;

	minor &= 0x1F;

	if (minor) {
		if (_dev->drives[drive].partitions[minor - 1].type == 0)
			return ENODEV;
		if (lba > _dev->drives[drive].partitions[minor - 1].end)
			return ENOSPC;
		lba += _dev->drives[drive].partitions[minor - 1].start;
	}


	if(!ata_write(_dev, drive, lba, (uint8_t *) buffer, 1))
		return EIO;
	return 0;
}

int ata_blk_read(dev_t device, aoff_t file_offset, void * buffer )
{
	dev_t major = MAJOR(device);
	dev_t minor = MINOR(device);
	ata_device_t *_dev = ata_buses[major - 0x10];
	ata_lba_t lba = ((ata_lba_t) file_offset) >> 9;
	int drive = (minor & 32) ? 1 : 0;

	if ((_dev == NULL) || (major >= 64))
		return ENODEV;

	minor &= 0x1F;

	if (minor) {
		if (_dev->drives[drive].partitions[minor - 1].type == 0)
			return ENODEV;
		if (lba > _dev->drives[drive].partitions[minor - 1].end)
			return ENOSPC;
		lba += _dev->drives[drive].partitions[minor - 1].start;
	}

	if(!ata_read(_dev, drive, lba, (uint8_t *) buffer, 1))
		return EIO;
	return 0;
}

int ata_blk_ioctl(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int func, __attribute__((__unused__)) int arg)
{
	return 0;
}

blk_ops_t ata_block_driver_ops = {
		&ata_blk_open,
		&ata_blk_close,
		&ata_blk_write,
		&ata_blk_read,
		&ata_blk_ioctl
};

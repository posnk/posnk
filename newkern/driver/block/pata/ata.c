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
#include "kernel/earlycon.h"
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "fs/mbr.h"

int ata_bus_number_counter = 0;
int ata_interrupt_enabled = 0;
int ata_global_inited = 0;

ata_device_t **ata_buses;

ata_prd_t *ata_prd_list;

extern blk_ops_t ata_block_driver_ops;

void ata_do_wait(ata_device_t *device)
{
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
	ata_read_port(device, ATA_STATUS_PORT);
}

int ata_poll_wait_resched(ata_device_t *device)
{
	int _t;
	uint8_t status;
	ktime_t timeout_end = system_time + 2;//TODO: CONSTANT
	for (_t = 0; _t < 4; _t++)
		ata_read_port(device, ATA_ALTSTATUS_PORT);

	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY) {
		if (system_time > timeout_end)
			return 0;
		schedule();
	}

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
int ata_poll_wait(ata_device_t *device)
{
	int _t;
	uint8_t status;
	ktime_t timeout_end = system_time + 2;//TODO: CONSTANT
	for (_t = 0; _t < 4; _t++)
		ata_read_port(device, ATA_ALTSTATUS_PORT);
	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY){
		if (system_time > timeout_end)
			return 0;
	}
	if (status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR))
		return 0;
	if (status & ATA_STATUS_FLAG_DRQ)
		return 1;
	else	
		return 2;
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
	ata_device_t *device = context;
	if (device->bmio_base) {
		if (!(ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_IREQ)){
	debugcon_printf("ataduimoisr\n");
			return 0;//Forward interrupt to next handlers
}
		ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_IREQ);
		if (!(ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_DMAGO))
			ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0);
		
	}
	device->int_status = ata_read_port(device, ATA_STATUS_PORT);
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
	int drive, _t;	
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
				debugcon_printf("ata: %i:%i doesn't support LBA and kernel was compiled w/o libgcc, disabling it\n", device->bus_number, drive);
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
			debugcon_printf("ata: detected device: %i:%i which is a %s with serial %s\n", device->bus_number, drive, device->drives[drive].model, device->drives[drive].serial);
			ata_load_partition_table(device, drive);
		} else {
			//Possibly detected an ATAPI device
			//TODO: Handle ATAPI devices
			debugcon_printf("ata: detected device: %i:%i which is probably an ATAPI device\n", device->bus_number, drive);
			continue;//Ignoring this one
		}
	}	
	if (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) {
		ata_prd_list = heapmm_alloc_alligned(ATA_PRD_LIST_SIZE * sizeof(ata_prd_t), 8);
		if (ata_prd_list) {
			ata_write_port_long(device, ATA_BUSMASTER_PRDT_PTR_PORT, paging_get_physical_address(ata_prd_list));
			debugcon_printf("ata: allocated DMA prd list at %x\n", ata_prd_list);
		} else {
			device->drives[drive].capabilities &= ~ATA_IDENT_CAP_FLAG_DMA;
		}
	}
	ata_set_interrupts(device, 1);
	ata_interrupt_enabled = 1;//TODO: Hook this somewhere

	drv = heapmm_alloc(sizeof(blk_dev_t));
	drv->name = "ATA block driver";
	drv->major = 0x10 + device->bus_number;
	drv->minor_count = 64;//2*32 partitions
	drv->block_size = 512;
	drv->cache_size = 16;
	drv->ops = &ata_block_driver_ops;
	device_block_register(drv);

	semaphore_up(device->lock);
}

void ata_setup_lba_transfer(ata_device_t *device, int drive, ata_lba_t lba, uint16_t count)
{
#ifdef HAVE_LIBGCC
	uint32_t cylinder, temp, head, sector;
#endif
	switch (device->drives[drive].lba_mode) {
		case ATA_MODE_LBA48:
			ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_LBA(drive, 0));
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
			ata_write_port(device, ATA_SECTOR_COUNT_LOW_PORT, count & 0xFF);
			ata_write_port(device, ATA_LBA_0_PORT, lba & 0xFF);
			ata_write_port(device, ATA_LBA_1_PORT, (lba >> 8) & 0xFF);
			ata_write_port(device, ATA_LBA_2_PORT, (lba >> 16) & 0xFF);
			if (count == 0x100)
				count = 0;
			else if (count > 0x100)
				debugcon_printf("ata: invalid sector count specified for LBA28 drive!");
			break;
		case ATA_MODE_CHS:
#ifdef HAVE_LIBGCC
			cylinder = lba / (device->drives[drive].heads * device->drives[drive].sectors_per_track);
			temp = lba % (device->drives[drive].heads * device->drives[drive].sectors_per_track);
			head = temp / device->drives[drive].sectors_per_track;
			sector = temp % device->drives[drive].sectors_per_track + 1;
			ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_CHS(drive, head & 0xF));
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
	int st;
	size_t byte_count = count * 512;
	int dma =(device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) && ata_interrupt_enabled;
	semaphore_down(device->lock);
	ata_setup_lba_transfer(device, drive, lba, count);
	*(device->int_wait) = 0;
#ifdef CONFIG_ATA_DEBUG
	debugcon_printf("ata: read  %i:%i[%x]->M[%x] * %i blocks, DMA:%i\n", device->bus_number, drive, (uint32_t)lba, buffer,(uint32_t) count, dma);
#endif
	if (dma) {
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_DMA_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_DMA);
		//TODO: Implement a work queue
		ata_prd_list[0].buffer_phys = paging_get_physical_address(buffer);
		ata_prd_list[0].byte_count = byte_count;
		ata_prd_list[0].end_of_list = ATA_PRD_END_OF_LIST;
		ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, ATA_BM_CMD_FLAG_DMA_ENABLE | ATA_BM_CMD_FLAG_READ);
		if (semaphore_tdown(device->int_wait, 3)) {
			debugcon_printf("ata: device %i:%i DMA read timeout\n", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		if (ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_ERR) {
			ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_ERR);
			debugcon_printf("ata: device %i:%i DMA read error\n", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		//scheduler_wait_micros(system_time_micros + 10);
		semaphore_up(device->lock);
		return 1;
	} else {
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO);
		if (ata_interrupt_enabled) {
			if (semaphore_tdown(device->int_wait, 3)) {
				debugcon_printf("ata: device %i:%i PIO read timeout\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (device->int_status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR)){
				debugcon_printf("ata: device %i:%i PIO read error\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (!(device->int_status & ATA_STATUS_FLAG_DRQ)){
				debugcon_printf("ata: device %i:%i PIO read no DRQ!\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
		} else {
			if (ata_poll_wait_resched(device) != 1) {
				debugcon_printf("ata: device %i:%i PIO read error %i\n", device->bus_number, drive, lba);
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
	int st;
	size_t byte_count = count * 512;
	int dma =  (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) && ata_interrupt_enabled;
	semaphore_down(device->lock);
	*(device->int_wait) = 0;
	ata_setup_lba_transfer(device, drive, lba, count);
#ifdef CONFIG_ATA_DEBUG
	debugcon_printf("ata: write %i:%i[%x]<-M[%x] * %i blocks, DMA:%i\n", device->bus_number, drive, (uint32_t)lba, buffer,(uint32_t) count, dma);
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
		if (semaphore_tdown(device->int_wait, 3)) {
			debugcon_printf("ata: device %i:%i DMA write timeout\n", device->bus_number, drive);
			semaphore_up(device->lock);
			return 0;
		}
		if (ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_ERR) {
			ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_ERR);
			ata_do_wait(device);
			debugcon_printf("ata: device %i:%i DMA write error\n", device->bus_number, drive);
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
			if (semaphore_tdown(device->int_wait, 3)){
				debugcon_printf("ata: device %i:%i PIO write timeout\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (device->int_status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR)){
				debugcon_printf("ata: device %i:%i PIO write error\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
			if (!(device->int_status & ATA_STATUS_FLAG_DRQ)){
				debugcon_printf("ata: device %i:%i DMA write no DRQ\n", device->bus_number, drive);
				semaphore_up(device->lock);
				return 0;
			}
		} else {
			if (ata_poll_wait_resched(device) != 1) {
				debugcon_printf("ata: device %i:%i write error %i\n", device->bus_number, drive, lba);
				semaphore_up(device->lock);
				return 0;
			}
		}
		ata_write_data(device, ATA_DATA_PORT, buffer, byte_count);
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_CACHE_FLUSH_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_CACHE_FLUSH);
		ata_poll_wait(device);
		semaphore_up(device->lock);
		return 1;
	}
}

int ata_blk_open(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options) {return 0;}

int ata_blk_close(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd) {return 0;}

int ata_blk_write(dev_t device, aoff_t file_offset, void * buffer )
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

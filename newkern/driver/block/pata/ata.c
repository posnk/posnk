/**
 * driver/block/pata/ata.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-07-2014 - Created
 */

#include "arch/i386/x86.h"
#include "driver/block/pata/ata.h"
#include "kernel/earlycon.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "config.h"

int ata_bus_number_counter = 0;
int ata_interrupt_enabled = 0;

ata_prd_t *ata_prd_list;

uint8_t ata_read_port(ata_device_t *device, uint16_t port)
{
	uint8_t rv;
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	if (port < 0x08)
		rv = i386_inb(device->pio_base  + port);
	else if (port < 0x0C)
		rv = i386_inb(device->pio_base  + port - 6);
	else if (port < 0x0E)
		rv = i386_inb(device->ctrl_base + port - 10);
	else if (port < 0x16)
		rv = i386_inb(device->bmio_base + port - 14);
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
	return rv;
}

void ata_write_port(ata_device_t *device, uint16_t port, uint8_t value)
{
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	if (port < 0x08)
		i386_outb(device->pio_base  + port, value);
	else if (port < 0x0C)
		i386_outb(device->pio_base  + port - 6, value);
	else if (port < 0x0E)
		i386_outb(device->ctrl_base + port - 10, value);
	else if (port < 0x16)
		i386_outb(device->bmio_base + port - 14, value);
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
}

void ata_read_data(ata_device_t *device, uint16_t port, uint8_t *buffer, size_t count)
{
	uint16_t port_addr = 0;
	uint32_t *_buffer = (uint32_t *) buffer;
	size_t ptr;
	count >>= 2;
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	if (port < 0x08)
		port_addr = device->pio_base  + port;
	else if (port < 0x0C)
		port_addr = device->pio_base  + port - 6;
	else if (port < 0x0E)
		port_addr = device->ctrl_base + port - 10;
	else if (port < 0x16)
		port_addr = device->bmio_base + port - 14;

	for (ptr = 0; ptr < count; ptr ++)
		_buffer[ptr] = i386_inl(port_addr);

	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);

}

int ata_poll_wait(ata_device_t *device)
{
	int _t;
	uint8_t status;
	for (_t = 0; _t < 4; _t++)
		ata_read_port(device, ATA_ALTSTATUS_PORT);
	while ((status = ata_read_port(device, ATA_STATUS_PORT)) & ATA_STATUS_FLAG_BSY);//TODO: Timeout
	if (status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR))
		return 0;
	if (status & ATA_STATUS_FLAG_DRQ)
		return 1;
	else	
		return 2;
}

void ata_set_interrupts(ata_device_t *device, int enabled)
{
	device->ctrl_reg = enabled ? 0 : ATA_DCR_FLAG_INT_DIS;
	ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
}

int ata_irq_handler(irq_id_t irq_id, void *context)
{
	ata_device_t *device = context;
	if (device->bmio_base) {
		if (!(ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_IREQ))
			return 0;//Forward interrupt to next handlers
		ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_IREQ);
		if (!(ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_DMAGO))
			ata_write_port(device, ATA_BUSMASTER_COMMAND_PORT, 0);
		
	}
	device->int_status = ata_read_port(device, ATA_STATUS_PORT);
	semaphore_up(device->int_wait);
	return 1;
}

void ata_initialize(ata_device_t *device)
{
	int drive, _t;
	uint8_t firstsector[512];
	ata_set_interrupts(device, 0); //Disable interrupts for this device_id
	interrupt_register_handler(device->irq, &ata_irq_handler, device);
	device->bus_number = ata_bus_number_counter++;
	device->lock = semaphore_alloc();
	device->int_wait = semaphore_alloc();
	for (drive = 0; drive < 2; drive++) {
		device->drives[drive].type = ATA_DRIVE_NONE;
		ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_CHS(drive, 0));//Select drive
		//TODO: Wait here ? osdev wiki says so but does not specify why.(no support for kernel-land sleeps yet)	
		//NOTE: Seems to work fine on Qemu this way...
		ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_IDENTIFY);//Execute #IDENTIFY
		//TODO: ^^^^
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
				debugcon_printf("ata: %i:%i doesn''t support LBA and kernel was compiled w/o libgcc, disabling it\n", device->bus_number, drive);
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
			ata_read(device,drive,0,firstsector,1);
			debugcon_printf("ata: said devices first 4 chars are %c%c%c%c\n", firstsector[0], firstsector[1], firstsector[2], firstsector[3]);
		} else {
			//Possibly detected an ATAPI device
			//TODO: Handle ATAPI devices
			debugcon_printf("ata: detected device: %i:%i which is probably an ATAPI device!\n", device->bus_number, drive);
			continue;//Ignoring this one
		}
	}	
	if (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) {
		ata_prd_list = heapmm_alloc_alligned(ATA_PRD_LIST_SIZE, 8);
		if (ata_prd_list) {
			ata_write_port(device, ATA_BUSMASTER_PRDT_PTR_PORT, paging_get_physical_address(ata_prd_list));
		} else {
			device->drives[drive].capabilities &= ~ATA_IDENT_CAP_FLAG_DMA;
		}
	}
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
	size_t byte_count = count * 512;
	int dma = (device->drives[drive].capabilities & ATA_IDENT_CAP_FLAG_DMA) && ata_interrupt_enabled;
	ata_setup_lba_transfer(device, drive, lba, count);
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
		semaphore_down(device->int_wait);
		if (ata_read_port(device, ATA_BUSMASTER_STATUS_PORT) & ATA_BM_STATUS_FLAG_ERR) {
			ata_write_port(device, ATA_BUSMASTER_STATUS_PORT, ATA_BM_STATUS_FLAG_ERR);
			debugcon_printf("ata: device %i:%i DMA read error\n", device->bus_number, drive);
			return 0;
		}
		return 1;
	} else {
		if (device->drives[drive].lba_mode == ATA_MODE_LBA48)
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO_EXT);
		else
			ata_write_port(device, ATA_COMMAND_PORT, ATA_CMD_READ_PIO);
		if (ata_interrupt_enabled) {
			semaphore_down(device->int_wait);
			if (device->int_status & (ATA_STATUS_FLAG_DF | ATA_STATUS_FLAG_ERR))
				return 0;
			if (!(device->int_status & ATA_STATUS_FLAG_DRQ))
				return 0;
		} else {
			if (ata_poll_wait(device) != 1) {
				debugcon_printf("ata: device %i:%i read error\n", device->bus_number, drive);
				return 0;
			}
		}
		ata_read_data(device, ATA_DATA_PORT, buffer, byte_count);
		return 1;
	}
}

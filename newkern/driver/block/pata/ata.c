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

int ata_bus_number_counter = 0;

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

void ata_initialize(ata_device_t *device)
{
	int drive, _t;
	ata_set_interrupts(device, 0); //Disable interrupts for this device_id
	device->bus_number = ata_bus_number_counter++;
	for (drive = 0; drive < 2; drive++) {
		device->drives[drive].type = ATA_DRIVE_NONE;
		ata_write_port(device, ATA_DRIVE_HEAD_PORT, ATA_DRVSEL_CHS(drive, 0));//Select drive
		//TODO: Wait here ? osdev wiki says so but does not specify why.(no support for kernel-land sleeps yet)	
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

			if (device->drives[drive].command_sets & ATA_IDENT_CMDSET_FLAG_LBA48)
				device->drives[drive].max_lba   = ATA_IDENT_LLONG(device, drive, ATA_IDENT_MAX_LBA_EXT);
			else
				device->drives[drive].max_lba   = ATA_IDENT_LONG(device, drive, ATA_IDENT_MAX_LBA);
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
			debugcon_printf("ata: detected device: %i:%i which is a %s with serial %s\n",device->bus_number, drive, device->drives[drive].model, device->drives[drive].serial);
		} else {
			//Possibly detected an ATAPI device
			//TODO: Handle ATAPI devices
			continue;//Ignoring this one
		}
	}
	
}

/**
 * @file driver/block/pata/ata_pc.c
 *
 * Implements the ATA control port operations for an i386 PC
 *
 * Part of P-OS driver library.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 02-07-2014 - Initially written
 * \li 20-01-2015 - Commented
 * \li 20-01-2015 - Moved out of ata.c
 */

#include "config.h"
#include <sys/errno.h>
#include "kernel/earlycon.h"
#include "arch/i386/x86.h"
#include "driver/block/pata/ata.h"

/**
 * @brief 	Reads from an ATA control port
 *
 * @warning 	Port address is not a direct port offset, only the constants
 * 		defined in the ATA driver header
 *
 * @param	device The ATA device to operate on
 * @param	port   The ATA port number to read from
 * @return	The value of the register at the specified port
 */
uint8_t ata_read_port(ata_device_t *device, uint16_t port)
{
	/* Used to store the return value before clean-up operations are done*/
	uint8_t rv;
	/* Check for LBA48 port offsets */
	if ((port > 0x07) && (port < 0x0C))
		/* Offset is in the LBA48 range, we need to set the HIGHORDER *
		 * flag on the port's control port */
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	
	/* Handle the different port ranges */
	if (port < 0x08)
		rv = i386_inb(device->pio_base  + port);
	else if (port < 0x0C)
		rv = i386_inb(device->pio_base  + port - 6);
	else if (port < 0x0E)
		rv = i386_inb(device->ctrl_base + port - 10);
	else if (port < 0x16)
		rv = i386_inb(device->bmio_base + port - 14);

	/* If we accessed a LBA48 port, reset the HIGHORDER flag on the ATA *
	 * control port */
	if ((port > 0x07) && (port < 0x0C)) 
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
	//debugcon_printf("ata_read_port (%x) = %x\n",port,rv);

	return rv;
}


/**
 * @brief 	Writes an 8 bit value to an ATA control port
 *
 * @warning 	Port address is not a direct port offset, only the constants
 * 		defined in the ATA driver header
 *
 * @param	device The ATA device to operate on
 * @param	port   The ATA port number to write to
 * @param	value  The value to write to the register
 */
void ata_write_port(ata_device_t *device, uint16_t port, uint8_t value)
{

	/* Check for LBA48 port offsets */
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		/* Offset is in the LBA48 range, we need to set the HIGHORDER *
		 * flag on the port's control port */
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	
	/* Handle the different port ranges */
	if (port < 0x08)
		i386_outb(device->pio_base  + port, value);
	else if (port < 0x0C)
		i386_outb(device->pio_base  + port - 6, value);
	else if (port < 0x0E)
		i386_outb(device->ctrl_base + port - 10, value);
	else if (port < 0x16)
		i386_outb(device->bmio_base + port - 14, value);
	
	/* If we accessed a LBA48 port, reset the HIGHORDER flag on the ATA *
	 * control port */
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
	//debugcon_printf("ata_write_port(%x) = %x\n",port,value);
}


/**
 * @brief 	Writes an 32 bit value to an ATA control port
 *
 * @warning 	Port address is not a direct port offset, only the constants
 * 		defined in the ATA driver header
 *
 * @param	device The ATA device to operate on
 * @param	port   The ATA port number to write to
 * @param	value  The value to write to the register
 */
void ata_write_port_long(ata_device_t *device, uint16_t port, uint32_t value)
{
	/* Check for LBA48 port offsets */
	if ((port > 0x07) && (port < 0x0C)) 
		/* Offset is in the LBA48 range, we need to set the HIGHORDER *
		 * flag on the port's control port */
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);
	
	/* Handle the different port ranges */
	if (port < 0x08)
		i386_outl(device->pio_base  + port, value);
	else if (port < 0x0C)
		i386_outl(device->pio_base  + port - 6, value);
	else if (port < 0x0E)
		i386_outl(device->ctrl_base + port - 10, value);
	else if (port < 0x16)
		i386_outl(device->bmio_base + port - 14, value);
	
	/* If we accessed a LBA48 port, reset the HIGHORDER flag on the ATA *
	 * control port */
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);
}


/**
 * @brief 	Reads from an ATA control port
 *
 * @warning 	Port address is not a direct port offset, only the constants
 * 		defined in the ATA driver header
 *
 * @param	device The ATA device to operate on
 * @param	port   The ATA port number to read from
 * @param	buffer The buffer to put the data in.
 * @param	count  The number of bytes to read from a port
 */
void ata_read_data(ata_device_t *device, uint16_t port, uint8_t *buffer, size_t count)
{
	uint16_t port_addr = 0;
	uint32_t *_buffer = (uint32_t *) buffer;
	size_t ptr;

	/* We use the long in instruction so we reduce the count to units *
	 * of four bytes */
	count >>= 2;
	
	/* Check for LBA48 port offsets */
	if ((port > 0x07) && (port < 0x0C)) 
		/* Offset is in the LBA48 range, we need to set the HIGHORDER *
		 * flag on the port's control port */
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);

	/* Calculate the address */
	if (port < 0x08)
		port_addr = device->pio_base  + port;
	else if (port < 0x0C)
		port_addr = device->pio_base  + port - 6;
	else if (port < 0x0E)
		port_addr = device->ctrl_base + port - 10;
	else if (port < 0x16)
		port_addr = device->bmio_base + port - 14;

	/* Actually read the data */
	for (ptr = 0; ptr < count; ptr ++)
		_buffer[ptr] = i386_inl(port_addr);

	/* If we accessed a LBA48 port, reset the HIGHORDER flag on the ATA *
	 * control port */
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);

}


/**
 * @brief 	Writes data to an ATA control port
 *
 * @warning 	Port address is not a direct port offset, only the constants
 * 		defined in the ATA driver header
 *
 * @param	device The ATA device to operate on
 * @param	port   The ATA port number to read from
 * @param	buffer The buffer to get the data from.
 * @param	count  The number of bytes to write to a port
 */
void ata_write_data(ata_device_t *device, uint16_t port, uint8_t *buffer, size_t count)
{
	uint16_t port_addr = 0;
	uint32_t *_buffer = (uint32_t *) buffer;
	size_t ptr;


	/* We use the long out instruction so we reduce the count to units *
	 * of four bytes */
	count >>= 2;

	/* Check for LBA48 port offsets */
	if ((port > 0x07) && (port < 0x0C)) 
		/* Offset is in the LBA48 range, we need to set the HIGHORDER *
		 * flag on the port's control port */
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg | ATA_DCR_FLAG_HIGHORDER);

	/* Calculate the address */
	if (port < 0x08)
		port_addr = device->pio_base  + port;
	else if (port < 0x0C)
		port_addr = device->pio_base  + port - 6;
	else if (port < 0x0E)
		port_addr = device->ctrl_base + port - 10;
	else if (port < 0x16)
		port_addr = device->bmio_base + port - 14;

	/* Actually write the data */
	for (ptr = 0; ptr < count; ptr ++)
		i386_outl(port_addr, _buffer[ptr]);

	/* If we accessed a LBA48 port, reset the HIGHORDER flag on the ATA *
	 * control port */
	if ((port > 0x07) && (port < 0x0C)) //LBA48
		ata_write_port(device, ATA_CONTROL_PORT, device->ctrl_reg);

}

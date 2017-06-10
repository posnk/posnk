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

char *ataport_names[] = {
	"ATA_DATA      ",// 0
	"ATA_FEATURE   ",// 1
	"ATA_SCOUNT_L  ",// 2
	"ATA_SECTOR_NO ",// 3
	"ATA_CYL_L     ",// 4
	"ATA_CYL_H     ",// 5
	"ATA_DRIVE_HEAD",// 6
	"ATA_CMD_STATUS",// 7
	"ATA_SCOUNT_H  ",// 8
	"ATA_LBA3      ",// 9
	"ATA_LBA4      ",//10
	"ATA_LBA5      ",//11
	"ATA_CTRL_ASTAT",//12
	"ATA_DEVADDR   ",//13
	"ATA_BM_COMMAND",//14
	"ATA_PORT15    ",//15
	"ATA_BM_STATUS ",//16
	"ATA_PORT16    ",//17
	"ATA_BM_PRDT_PT"//18
};

void ata_dump_port(char *func, uint16_t port, uint32_t val)
{
	debugcon_printf("%s(%s) = ",func,ataport_names[port]);
	if ( port == ATA_BUSMASTER_STATUS_PORT ) {
		if ( val & ATA_BM_STATUS_FLAG_DMAGO )
			debugcon_printf("DGO ");
		else
			debugcon_printf("    ");
		if ( val & ATA_BM_STATUS_FLAG_ERR )
			debugcon_printf("ERR ");
		else
			debugcon_printf("    ");
		if ( val & ATA_BM_STATUS_FLAG_IREQ )
			debugcon_printf("IRQ ");
		else
			debugcon_printf("    ");
		if ( val & ATA_BM_STATUS_FLAG_SIMPLEX )
			debugcon_printf("SPX\n");
		else
			debugcon_printf("   \n");
	} else if ( port == ATA_STATUS_PORT ) {
		if ( val & ATA_STATUS_FLAG_ERR )
			debugcon_printf("ERR ");
		else
			debugcon_printf("    ");
		if ( val & ATA_STATUS_FLAG_DRQ )
			debugcon_printf("DRQ ");
		else
			debugcon_printf("    ");
		if ( val & ATA_STATUS_FLAG_SRV )
			debugcon_printf("SRV ");
		else
			debugcon_printf("    ");
		if ( val & ATA_STATUS_FLAG_DF )
			debugcon_printf("DF ");
		else
			debugcon_printf("   ");
		if ( val & ATA_STATUS_FLAG_RDY )
			debugcon_printf("RDY ");
		else
			debugcon_printf("    ");
		if ( val & ATA_STATUS_FLAG_BSY )
			debugcon_printf("BSY\n");
		else
			debugcon_printf("   \n");
	} else
		debugcon_printf("0x%x\n",val);
}

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
	//ata_dump_port("ata_read_port",port,rv);

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
	//ata_dump_port("ata_write_prt",port,value);
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
	//ata_dump_port("ata_write_prl",port,value);
	
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

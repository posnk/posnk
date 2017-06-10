/**
 * driver/bus/pci_intel_host.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-07-2014 - Created
 */
#include "arch/i386/x86.h"
#include "driver/bus/pci_intel_host.h"
#include "kernel/earlycon.h"

uint32_t pci_config_read_long( uint8_t bus, uint8_t device, uint8_t function, uint8_t address )
{
	uint32_t value;
	uint32_t _address = (((uint32_t) bus) << 16) | (((uint32_t)device) << 11) | (((uint32_t)function) << 8) | (((uint32_t)address) & 0xFC);

	_address |= CONFIG_ENABLE;
	i386_outl(CONFIG_ADDRESS, _address);
	value = i386_inl (CONFIG_DATA);

	return value;
}

void pci_config_write_long( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint32_t value )
{
	uint32_t _address = (((uint32_t) bus) << 16) | (((uint32_t)device) << 11) | (((uint32_t)function) << 8) | (((uint32_t)address) & 0xFC);

	_address |= CONFIG_ENABLE;
	
	i386_outl(CONFIG_ADDRESS, _address);
	i386_outl(CONFIG_DATA	, value);
}

uint16_t pci_config_read_short( uint8_t bus, uint8_t device, uint8_t function, uint8_t address )
{
	uint32_t lval = pci_config_read_long( bus, device, function, address );

	return (lval >> ((address & 2) * 8)) & 0xFFFF;
}

void pci_config_write_short( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint16_t value )
{
	uint32_t lval = pci_config_read_long( bus, device, function, address );

	lval &=          ~(0xFFFF  << ((address & 2) * 8));
	lval |=  ((value & 0xFFFF) << ((address & 2) * 8));

	pci_config_write_long( bus, device, function, address, lval );
}

uint8_t pci_config_read_byte( uint8_t bus, uint8_t device, uint8_t function, uint8_t address )
{
	uint32_t lval = pci_config_read_long( bus, device, function, address );

	return (uint8_t) ((lval >> ((address & 3) * 8)) & 0xFF);
}

void pci_config_write_byte( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint16_t value )
{
	uint32_t lval = pci_config_read_long( bus, device, function, address );

	lval &=          ~(0xFF  << ((address & 3) * 8));
	lval |=  ((value & 0xFF) << ((address & 3) * 8));

	pci_config_write_long( bus, device, function, address, lval );
}

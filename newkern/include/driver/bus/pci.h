/**
 * driver/bus/pci.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-07-2014 - Created
 */

#ifndef __DRIVER_BUS_PCI_H__
#define __DRIVER_BUS_PCI_H__

#include <stdint.h>

/* Common header fields */
#define PCI_CONFIG_VENDOR_ID			(0x00)
#define PCI_CONFIG_DEVICE_ID			(0x02)
#define PCI_CONFIG_COMMAND				(0x04)
#define PCI_CONFIG_STATUS				(0x06)
#define PCI_CONFIG_REVISION_ID			(0x08)
#define PCI_CONFIG_API_ID				(0x09)
#define PCI_CONFIG_SUBCLASS				(0x0A)
#define PCI_CONFIG_CLASS				(0x0B)
#define PCI_CONFIG_CACHE_SIZE			(0x0C)
#define PCI_CONFIG_LATENCY_TMR			(0x0D)
#define PCI_CONFIG_HEADER_TYPE			(0x0E)
#define PCI_CONFIG_SELF_TEST			(0x0F)

/* Header type flags */
#define PCI_CONFIG_MULTIFUNCTION		(0x80)
#define PCI_CONFIG_HEADER_TYPE_MASK		(0x7F)

/* Base address registers */
#define PCI_CONFIG_BAR0					(0x10)
#define PCI_CONFIG_BAR1					(0x14)
#define PCI_CONFIG_BAR2					(0x18)
#define PCI_CONFIG_BAR3					(0x1C)
#define PCI_CONFIG_BAR4					(0x20)
#define PCI_CONFIG_BAR5					(0x24)

/* Header type 0 fields */
#define PCI_CONFIG_CIS_POINTER			(0x28)
#define PCI_CONFIG_SUBSYS_VENDOR_ID		(0x2C)
#define PCI_CONFIG_SUBSYS_ID			(0x2E)
#define PCI_CONFIG_OPTION_ROM_PTR		(0x30)
#define PCI_CONFIG_CAPABILITY_PTR		(0x34)
#define PCI_CONFIG_INTERRUPT_LINE		(0x3C)//ALSO ON 1
#define PCI_CONFIG_INTERRUPT_PIN		(0x3D)
#define PCI_CONFIG_MIN_GRANT			(0x3E)
#define PCI_CONFIG_MAX_LATENCY			(0x3F)

/* Header type 1 fields (PCI-PCI bridge) */
#define PCI_CONFIG_PRIMARY_BUS			(0x18)
#define PCI_CONFIG_SECONDARY_BUS		(0x19)
#define PCI_CONFIG_SUBORD_BUS_NUMBER	(0x1A)
#define PCI_CONFIG_SEC_LATENCY_TIMER	(0x1B)
#define PCI_CONFIG_IO_BASE				(0x1C)
#define PCI_CONFIG_IO_LIMIT				(0x1D)
#define PCI_CONFIG_SECONDARY_STATUS		(0x1E)
#define PCI_CONFIG_MEMORY_BASE			(0x20)
#define PCI_CONFIG_MEMORY_LIMIT			(0x22)
#define PCI_CONFIG_PREFETCH_MEM_BASE	(0x24)
#define PCI_CONFIG_PREFETCH_MEM_LIMIT	(0x26)
#define PCI_CONFIG_PREFETCH_MEM_BASE_H	(0x28)
#define PCI_CONFIG_PREFETCH_MEM_LIMIT_H	(0x2C)
#define PCI_CONFIG_IO_BASE_H			(0x30)
#define PCI_CONFIG_IO_LIMIT_H			(0x32)

/* Necessary definitions */
#define PCI_CLASS_BRIDGE				(0x06)
#define PCI_SUBCLASS_PCI2PCI			(0x04)

uint32_t pci_config_read_long ( uint8_t bus, uint8_t device, uint8_t function, uint8_t address );
uint16_t pci_config_read_short( uint8_t bus, uint8_t device, uint8_t function, uint8_t address );
uint8_t  pci_config_read_byte ( uint8_t bus, uint8_t device, uint8_t function, uint8_t address );

void pci_config_write_long ( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint32_t value );
void pci_config_write_short( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint16_t value );
void pci_config_write_byte ( uint8_t bus, uint8_t device, uint8_t function, uint8_t address, uint16_t value );

void pci_enumerate_all();

void pci_enumerate_bus(uint8_t bus);

void pci_enumerate_function ( uint8_t bus, uint8_t device, uint8_t function );

#endif

/**
 * driver/bus/pci.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-07-2014 - Created
 */
#include "kernel/drivermgr.h"
#include "kernel/earlycon.h"

#include "driver/bus/pci.h"
#include "driver/bus/pci_list.h"

const char * pci_vendor_lookup(unsigned short vendor_id) {
	unsigned int i;
	for (i = 0; i < PCI_VENTABLE_LEN; ++i) {
		if (PciVenTable[i].VenId == vendor_id) {
			return PciVenTable[i].VenFull;
		}
	}
	return "";
}

const char * pci_device_lookup(unsigned short vendor_id, unsigned short device_id) {
	unsigned int i;
	for (i = 0; i < PCI_DEVTABLE_LEN; ++i) {
		if (PciDevTable[i].VenId == vendor_id && PciDevTable[i].DevId == device_id) {
			return PciDevTable[i].ChipDesc;
		}
	}
	return "";
}

void pci_enumerate_all()
{
	uint8_t function, function_count, hdr_type;
	uint16_t vid;
	hdr_type = pci_config_read_byte( 0, 0, 0, PCI_CONFIG_HEADER_TYPE );
	function_count = ( hdr_type & PCI_CONFIG_MULTIFUNCTION ) ? 8 : 1;

	debugcon_printf("Enumerating PCI...\n");

	for ( function = 0; function < function_count; function++ ) {
		vid = pci_config_read_short( 0, 0, function, PCI_CONFIG_VENDOR_ID );
		if (vid == 0xFFFF)
			continue;
		pci_enumerate_bus( function );
	}
		
}

void pci_enumerate_bus(uint8_t bus)
{
	uint8_t device, function, function_count, hdr_type;
	uint16_t vid;
	for ( device = 0; device < 32; device++ ) {
		vid = pci_config_read_short( bus, device, 0, PCI_CONFIG_VENDOR_ID );
		if (vid == 0xFFFF)
			continue;

		hdr_type = pci_config_read_byte( bus, device, 0, PCI_CONFIG_HEADER_TYPE );
		function_count = ( hdr_type & PCI_CONFIG_MULTIFUNCTION ) ? 8 : 1;

		for ( function = 0; function < function_count; function++ ) {
			vid = pci_config_read_short( bus, device, function, PCI_CONFIG_VENDOR_ID );
			if (vid == 0xFFFF)
				continue;
			pci_enumerate_function( bus, device, function );
		}
		
	}
}

void pci_enumerate_function ( uint8_t bus, uint8_t device, uint8_t function )
{
	uint8_t class, subclass, api, secbus;
	uint16_t vid, pid;
	uint32_t busaddr, interface;
	vid = pci_config_read_short( bus, device, function, PCI_CONFIG_VENDOR_ID );
	pid = pci_config_read_short( bus, device, function, PCI_CONFIG_DEVICE_ID );
	class 	 = pci_config_read_byte( bus, device, function, PCI_CONFIG_CLASS );
	subclass = pci_config_read_byte( bus, device, function, PCI_CONFIG_SUBCLASS );
	api = pci_config_read_short( bus, device, function, PCI_CONFIG_API_ID );

	if ((class == PCI_CLASS_BRIDGE) && (subclass == PCI_SUBCLASS_PCI2PCI)) {
		secbus = pci_config_read_byte( bus, device, function, PCI_CONFIG_SECONDARY_BUS);
		pci_enumerate_bus( secbus );	
		return;	
	}
	
	busaddr = ((bus << 16) & 0xFF0000) | ((device << 8) & 0xFF00) | (function & 0xFF);

	if (drivermgr_enumerate_device(DEVICE_TYPE_PCI, busaddr, vid, pid))
		return;

	interface = ((class << 16) & 0xFF0000) | ((subclass << 8) & 0xFF00) | (api & 0xFF);
	//debugcon_printf("Enumerating interface ifid %x\n", interface);

	if (drivermgr_enumerate_interface(DEVICE_TYPE_PCI, busaddr, interface))
		return;

	debugcon_printf("%i:%i.%i - %x:%x %s %s NO DRIVER FOUND!!!\n", bus, device, function,
			vid, pid,
			pci_vendor_lookup(vid),
			pci_device_lookup(vid, pid));
}

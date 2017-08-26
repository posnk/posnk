/**
 * driver/net/rtl8139/pci.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-08-2017 - Created
 */

#include "kernel/earlycon.h"
#include "kernel/heapmm.h"
#include "kernel/drivermgr.h"
#include "driver/bus/pci.h"
#include "driver/net/rtl8139/hwdefs.h"

int rtl8139_pci_probe(uint32_t bus_addr) {
	uint8_t bus	 = (uint8_t) ((bus_addr >> 16) & 0xFF);
	uint8_t device	 = (uint8_t) ((bus_addr >>  8) & 0xFF);
	uint8_t function = (uint8_t) ((bus_addr      ) & 0xFF);
	uint32_t bar0 = pci_config_read_long(	bus, 
						device,
						function,
						PCI_CONFIG_BAR0 );
	uint32_t bar1 = pci_config_read_long(	bus,
						device,
						function,
						PCI_CONFIG_BAR1 );
	uint32_t irq  = pci_config_read_byte(	bus,
						device,
						function,
						PCI_CONFIG_INTERRUPT_LINE ) ;
	uint32_t cr   = pci_config_read_short(	bus,
						device,
						function,
						PCI_CONFIG_COMMAND ) ;
/*
	rtl8139_t *ifd = heapmm_alloc(sizeof(rtl8139_t));
	if (!ifd) {
		debugcon_printf("rtl8139: error initializing, out of memory!\n");
		return 0;
	}
*/
	debugcon_printf("rtl8139: found controller %i:%i.%i (%x,%x,CR=%x)->IRQ%i\n",
			bus, 
			device, 
			function, 
			bar0, 
			bar1, 
			cr,
			irq);
/*
	ifd->iobase = bar0 & 0xFFFFFF00;
*/
/*	if ( irq == 255 ) {
		debugcon_printf("ata_pci: allocating irq 14 to controller %i.%i.%i!\n",
							bus,
							device,
							function);
		pci_config_write_byte( bus, device, function, PCI_CONFIG_INTERRUPT_LINE,
								14 );
		irq = 14;
	}
	pci_config_write_byte( bus, device, function, PCI_CONFIG_COMMAND,0x5); 
*/
//	rtl8139_initialize(ifd);
	return 1;
}

drivermgr_device_driver_t rtl8139_pci_descriptor = {
	{0,0},
	DEVICE_TYPE_PCI,
	RTL8139_PCI_VID, 
	RTL8139_PCI_PID,
	&rtl8139_pci_probe
};

void rtl8139_register()
{
	drivermgr_register_device_driver(&rtl8139_pci_descriptor);
}

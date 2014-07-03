/**
 * driver/block/pata/ata_pci.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-07-2014 - Created
 */

#include "kernel/earlycon.h"
#include "kernel/heapmm.h"
#include "kernel/drivermgr.h"
#include "driver/bus/pci.h"
#include "driver/block/pata/ata.h"

int ata_pci_probe(uint32_t bus_addr) {
	uint8_t bus	 = (uint8_t) ((bus_addr >> 16) & 0xFF);
	uint8_t device	 = (uint8_t) ((bus_addr >>  8) & 0xFF);
	uint8_t function = (uint8_t) ((bus_addr      ) & 0xFF);
	uint32_t bar0 = pci_config_read_long( bus, device, function, PCI_CONFIG_BAR0 ) & 0xFFFFFFFC;
	uint32_t bar1 = pci_config_read_long( bus, device, function, PCI_CONFIG_BAR1 ) & 0xFFFFFFFC;
	uint32_t bar2 = pci_config_read_long( bus, device, function, PCI_CONFIG_BAR2 ) & 0xFFFFFFFC;
	uint32_t bar3 = pci_config_read_long( bus, device, function, PCI_CONFIG_BAR3 ) & 0xFFFFFFFC;
	uint32_t bar4 = pci_config_read_long( bus, device, function, PCI_CONFIG_BAR4 ) & 0xFFFFFFFC;
	uint32_t irq  = pci_config_read_byte( bus, device, function, PCI_CONFIG_INTERRUPT_LINE ) ;
	ata_device_t *dev_p = heapmm_alloc(sizeof(ata_device_t));
	ata_device_t *dev_s = heapmm_alloc(sizeof(ata_device_t));
	if ((!dev_s) && dev_p) {
		heapmm_free(dev_s, sizeof(ata_device_t));
		dev_s = NULL;
	}
	if (!dev_p) {
		debugcon_printf("ata_pci: error initializing, out of memory!\n");
		return 0;
	}
	debugcon_printf("ata_pci: initializing controller %i:%i.%i (%x, %x, %x, %x, %x)->IRQ%i\n", bus, device, function, bar0, bar1, bar2, bar3, bar4, irq);

	dev_p->pio_base = (bar0 > 1) ? bar0 : 0x1F0;
	dev_p->ctrl_base = (bar1 > 1) ? bar1 : 0x3F4;
	dev_p->bmio_base = bar4;
	dev_p->irq = irq ? irq : 14;

	dev_s->pio_base = (bar2 > 1) ? bar2 : 0x170;
	dev_s->ctrl_base = (bar3 > 1) ? bar3 : 0x374;
	dev_s->bmio_base = bar4 + 8;
	dev_s->irq = irq ? irq : 15;

	ata_initialize(dev_p);
	ata_initialize(dev_s);
	return 1;
}

drivermgr_interface_driver_t ata_pci_descriptor = {
	{NULL, NULL},
	DEVICE_TYPE_PCI,
	0x010100,
	0xFFFF00,
	&ata_pci_probe
};

void ata_pci_register()
{
	drivermgr_register_interface_driver(&ata_pci_descriptor);
}

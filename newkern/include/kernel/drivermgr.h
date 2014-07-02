/**
 * kernel/drivermgr.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-07-2014 - Created
 */

#ifndef __KERNEL_DRIVERMGR_H__
#define __KERNEL_DRIVERMGR_H__

#include <stdint.h>
#include "util/llist.h"

#define DEVICE_TYPE_PCI		(0)

typedef int dev_type_t ;

struct drivermgr_device_driver {
	llist_t		link;
	dev_type_t	type;
	int		vendor;
	int		device;
	int		(*probe)	(uint32_t bus_addr);
};

struct drivermgr_interface_driver {
	llist_t		link;
	dev_type_t	type;
	int		interface;
	int		(*probe)	(uint32_t bus_addr);
};

typedef struct drivermgr_device_driver drivermgr_device_driver_t;

typedef struct drivermgr_interface_driver drivermgr_interface_driver_t;

int drivermgr_enumerate_device(dev_type_t type,uint32_t bus_addr, int vendor, int device);

int drivermgr_enumerate_interface(dev_type_t type, uint32_t bus_addr, uint32_t interface);

void drivermgr_register_device_driver( drivermgr_device_driver_t *driver );

void drivermgr_register_interface_driver( drivermgr_interface_driver_t *driver );

#endif

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

#define DEVICE_TYPE_PCI		(0)

typedef dev_type_t int;

int drivermgr_enumerate_device(dev_type_t type, uint32_t bus_addr, int vendor, int device);

int drivermgr_enumerate_iface(dev_type_t type, uint32_t bus_addr, int interface);

#endif

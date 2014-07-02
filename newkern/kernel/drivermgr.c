/**
 * kernel/drivermgr.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-07-2014 - Created
 */

#include "kernel/drivermgr.h"
#include "kernel/heapmm.h"

llist_t drivermgr_device_driver_list;
llist_t drivermgr_interface_driver_list;

void drivermgr_init()
{
	llist_create(&(drivermgr_device_driver_list));
	llist_create(&(drivermgr_interface_driver_list));
}

/** 
 * Iterator function that looks up the driver for it
 */
int drivermgr_enumerate_device_iterator (llist_t *node, void *param)
{
	drivermgr_device_driver_t *drv = (drivermgr_device_driver_t *) node;
	drivermgr_device_driver_t *cmp = (drivermgr_device_driver_t *) param;
	return (drv->type == cmp->type) && (drv->vendor == cmp->vendor) && (drv->device == cmp->device);		
}

int drivermgr_enumerate_device(dev_type_t type, uint32_t bus_addr, int vendor, int device)
{
	drivermgr_device_driver_t cmp;
	drivermgr_device_driver_t *drv;
	cmp.type = type;
	cmp.vendor = vendor;
	cmp.device = device;
	drv = (drivermgr_device_driver_t *) llist_iterate_select(&drivermgr_device_driver_list, &drivermgr_enumerate_device_iterator, (void *) &cmp);
	if (!drv)
		return 0;
	drv->probe(bus_addr);
	return 1;
}

/** 
 * Iterator function that looks up the driver for it
 */
int drivermgr_enumerate_interface_iterator (llist_t *node, void *param)
{
	drivermgr_interface_driver_t *drv = (drivermgr_interface_driver_t *) node;
	drivermgr_interface_driver_t *cmp = (drivermgr_interface_driver_t *) param;
	return (drv->type == cmp->type) && (drv->interface == cmp->interface);		
}

int drivermgr_enumerate_interface(dev_type_t type, uint32_t bus_addr, uint32_t interface)
{
	drivermgr_interface_driver_t cmp;
	drivermgr_interface_driver_t *drv;
	cmp.type = type;
	cmp.interface = interface;
	drv = (drivermgr_interface_driver_t *) llist_iterate_select(&drivermgr_interface_driver_list, &drivermgr_enumerate_interface_iterator, (void *) &cmp);
	if (!drv)
		return 0;
	drv->probe(bus_addr);
	return 1;
}

void drivermgr_register_device_driver( drivermgr_device_driver_t *driver )
{
	llist_add_end( &drivermgr_device_driver_list, (llist_t *) driver );
}

void drivermgr_register_interface_driver( drivermgr_interface_driver_t *driver )
{
	llist_add_end( &drivermgr_interface_driver_list, (llist_t *) driver );
}

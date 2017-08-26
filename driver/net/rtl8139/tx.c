/**
 * driver/net/rtl8139/tx.c
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
#include "driver/net/rtl8139/swdefs.h"

void rtl8139_tx_packet( rtl8139_t *ifd, netpkt_t p )
{
	int slot;
	//TODO: Make 
	rtl8139_wregl(	RTL8139_REG_TSAD( ifd->tx_slot ), p->buffer_phys );
	rtl8139_wregl(	RTL8139_REG_TSD( ifd->tx_slot ),
			RTL8139_TSD_SIZE_W( p->size ),	
}

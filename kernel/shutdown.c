/**
 * kernel/shutdown.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-09-2014 - Created
 */
#include <string.h>
#include <sys/errno.h>
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"


void shutdown()
{
	earlycon_printf("Flushing inode cache...");
	vfs_cache_flush();
	earlycon_printf("OK\nSynchronizing filesystems...");
	vfs_sync_filesystems();
	earlycon_printf("OK\nFlushing block cache...");
	if (device_block_flush_global())
		earlycon_printf("FAILED\n");
	else
		earlycon_printf("OK\n");
	//earlycon_printf("Bringing down system using a panic...");
	//*((uint32_t *)0xF0000000) = 0;
}

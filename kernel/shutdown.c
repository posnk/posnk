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
#define CON_SRC "shutdown"
#include "kernel/console.h"


void shutdown()
{
	printf(CON_INFO, "Flushing inode cache...");
	vfs_cache_flush();
	printf(CON_INFO, "Synchronizing filesystems...");
	vfs_sync_filesystems();
	printf(CON_INFO, "Flushing block cache...");
	if (device_block_flush_global())
		printf(CON_ERROR, "Block cache flush FAILED\n");
	printf(CON_INFO, "Halting...");
	halt();
	//earlycon_printf("Bringing down system using a panic...");
	//*((uint32_t *)0xF0000000) = 0;
}

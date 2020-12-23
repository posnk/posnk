/**
 * kernel/init/kinit.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include "kernel/physmm.h"
#define CON_SRC ("kinit")
#include "kernel/console.h"
#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "driver/bus/pci.h"
#include "driver/platform/platform.h"
#include "kernel/syscall.h"
#include "kernel/streams.h"
#include "kernel/vfs.h"
#include "kernel/tar.h"
#include "kernel/ipc.h"
#include "kernel/device.h"
#include "kernel/tty.h"
#include "kernel/drivermgr.h"
#include "kernel/interrupt.h"
#include "kernel/version.h"
#include "kernel/process.h"
#include "kernel/init.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>

static void print_banner()
{
	printf(CON_INFO, "P-OS Release %s Version %s. %i MB free",
		posnk_release, posnk_version, physmm_count_free()/0x100000);
	printf(CON_INFO, "kernel commandline:");
	printf(CON_INFO, "%s",kernel_cmdline);
}

void register_dev_drivers();

extern int ata_interrupt_enabled;

void kmain()
{
	con_register_src( "kinit" );
	print_banner();

	cmdline_parse();

	printf(CON_INFO, "initializing initial task");
	scheduler_init();

	arch_init_early();

	process_init();

	printf(CON_INFO, "initializing driver infrastructure");
	drivermgr_init();
	device_char_init();
	device_block_init();
	tty_init();
	interrupt_init();

	printf(CON_INFO, "initializing builtin drivers");
	register_dev_drivers();
#ifdef ARCH_I386
#ifndef CONFIG_i386_NO_PCI
	puts(CON_INFO, "enumerating PCI devices");
	pci_enumerate_all();
#endif
#endif
	arch_init_late();

	printf(CON_INFO, "initializing system calls");
	syscall_init();
	ipc_init();

	kinit_start_idle_task();

	printf(CON_INFO, "mounting root device %X as %s",
		root_dev, root_fs);

	if (!vfs_initialize( root_dev, root_fs )) {
		puts(CON_PANIC, "failed to mount root filesystem, halting...");
		halt();
	}

#ifdef ARCH_I386
	ata_interrupt_enabled = 1;
#endif

	kinit_start_uinit();

	printf(CON_PANIC, "\n\nkernel main exited... halting!");

}

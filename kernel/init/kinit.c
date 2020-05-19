/**
 * kernel/init/kinit.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include "kernel/physmm.h"
#include "kernel/earlycon.h"
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
	earlycon_printf("P-OS Release %s Version %s. %i MB free\n",
		posnk_release, posnk_version, physmm_count_free()/0x100000);
	earlycon_printf("kernel commandline:\n%s\n",kernel_cmdline);
}

void register_dev_drivers();

extern int ata_interrupt_enabled;

void kmain()
{

	print_banner();

	cmdline_parse();

	arch_init_early();

	earlycon_printf("kinit: initializing initial task\n");
	scheduler_init();
	process_init();

	earlycon_printf("kinit: initializing driver infrastructure\n");
	drivermgr_init();
	device_char_init();
	device_block_init();
	tty_init();
	interrupt_init();


	earlycon_printf("kinit: initializing builtin drivers\n");
	register_dev_drivers();

#ifndef CONFIG_i386_NO_PCI
	earlycon_puts("kinit: enumerating PCI devices\n");
	pci_enumerate_all();
#endif

	arch_init_late();

	earlycon_printf("kinit: initializing system calls\n");
	syscall_init();
	ipc_init();

	kinit_start_idle_task();

	earlycon_printf("kinit: mounting root device %X as %s\n", 
		root_dev, root_fs);
	if (!vfs_initialize( root_dev, root_fs )) {
		earlycon_puts("kinit: failed to mount root filesystem, halting...\n");
		halt();
	}

	ata_interrupt_enabled = 1;

	kinit_start_uinit();

	earlycon_puts("\n\nkernel main exited... halting!");

}

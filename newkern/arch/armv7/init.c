/**
 * arch/armv7/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2014 - Created
 */
#include <fcntl.h>
#include <sys/errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include "kernel/interrupt.h"
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/physmm.h"
#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/vfs.h"
#include "kernel/tar.h"
#include "kernel/ipc.h"
#include "kernel/device.h"
#include "kernel/tty.h"
#include "kernel/drivermgr.h"
#include "kdbg/dbgapi.h"
#include "arch/armv7/bootargs.h"
#include "arch/armv7/exception.h"
#include "arch/armv7/cpu.h"
void sercon_init();

char *armv7_cmdline[128];
uint32_t armv7_initrd_start_pa;
uint32_t armv7_initrd_end_pa;

void halt()
{
	for(;;);
}

void armv7_init_stub()
{
        int fd = _sys_open("/faketty", O_RDWR, 0);
        if (fd < 0) {
                earlycon_printf("Error opening tty : %i\n",syscall_errno);
        } else {
                fd = _sys_dup(fd);
                if (fd < 0) {
                        earlycon_printf("Error dupping stdin : %i\n",syscall_errno);
                } else {
                        fd = _sys_dup(fd);
                        if (fd < 0) {
                                earlycon_printf("Error dupping stderr : %i\n",syscall_errno);
                        }
                }
        }
        char *ptr = NULL;
        void *nullaa = &ptr;
        //vgacon_clear_video();
        int status = process_exec("/sbin/init", nullaa, nullaa);
        if (status) {
                earlycon_printf("Error executing init : %i\n",status);
        }
	armv7_enable_ints();
	for (;;);
}

void armv7_idle_task()
{
        scheduler_set_as_idle();
        strcpy(scheduler_current_task->name, "idle task");
       	armv7_enable_ints();

        for (;;)
                ;
}


void tasks_init()
{
	int init_status, rv;
	pid_t pid_init, pid_idle;
        uint32_t wp_params[4];

	earlycon_printf("Forking init...\n");
	pid_init = scheduler_fork();
	if (!pid_init)
		armv7_init_stub();
	pid_idle = scheduler_fork();
	if (!pid_idle)
		armv7_idle_task();
	armv7_enable_ints();
	
	wp_params[0] = (uint32_t) pid_init;
        wp_params[1] = (uint32_t) &init_status;
        wp_params[2] = 0;
        rv = sys_waitpid(wp_params,wp_params);

        earlycon_printf("PANIC! Init exited with status: %i %i\n",init_status,rv);

//        earlycon_puts("\n\nkernel main exited... halting!");

	for (;;);

	
}

void armv7_initrd_extr(physaddr_t start, physaddr_t end)
{
	physaddr_t pa, pac;
	uintptr_t va, vap;
	
	pa = start & ~0xFFF;
	va = 0x80000000;
	vap = va;
	for (pac = pa; pac < end; pac+=4096) {
		paging_map ((void *)va, pac, PAGING_PAGE_FLAG_RW );
		vap+=4096;
	}

	vap = va + (pa & 0xFFF);

	tar_extract_mem((void *)vap);

	for (pac = pa; pac< end; pac+=4096)  {
		paging_unmap((void *)va);
		physmm_free_frame(pac);
		va += 4096;
	}

}

void armv7_init( armv7_bootargs_t *bootargs )
{
	size_t initial_heap = 4096;
	sercon_init();

	earlycon_printf("posnk kernel built on %s %s\n", __DATE__,__TIME__);

	earlycon_printf("bootargs: loading boot arguments from 0x%x\n", bootargs);

	strcpy(armv7_cmdline, bootargs->ba_cmd);
	
	armv7_initrd_start_pa = bootargs->ba_initrd_pa;	
	armv7_initrd_end_pa = bootargs->ba_initrd_pa + bootargs->ba_initrd_sz;

	earlycon_printf("bootargs: command line = %s\n", armv7_cmdline);
	earlycon_printf("bootargs: initrd [0x%x-0x%x]\n", armv7_initrd_start_pa,
							armv7_initrd_end_pa);

	memcpy(physmm_bitmap, bootargs->ba_pm_bitmap, 32768 * sizeof(uint32_t));
	armv7_exception_init();
	earlycon_printf("physmm: initializing...\n");

	physmm_free_range((physaddr_t)bootargs->ba_pm_bitmap,
			 ((physaddr_t)bootargs->ba_pm_bitmap)
				 + 32768 * sizeof(uint32_t));

	earlycon_printf("physmm: %i MB available\n",
			physmm_count_free() / 0x100000);

	armv7_paging_init(bootargs);
	
	earlycon_printf("paging online\n");
	kdbg_initialize();
	earlycon_printf("heap debugger online\n");
	initial_heap = heapmm_request_core((void *)0xd0000000, initial_heap);
	if (initial_heap == 0) {
		earlycon_printf("could not alloc first page of heap!\n");
		halt();
	}
	heapmm_init((void *) 0xd0000000, initial_heap);
	earlycon_printf("heeapmmtest: 0x%x\n", heapmm_alloc(123));
	earlycon_printf("initializing sched\n");
	scheduler_init();
	earlycon_printf("initializing drivers\n");
	drivermgr_init();
	device_char_init();
	device_block_init();
	tty_init();
	earlycon_printf("loading builtin drivers\n");
	register_dev_drivers();
	earlycon_printf("loaded drivers\n");
	earlycon_printf("initializing vfs...");
	if (vfs_initialize(3, "ramfs"))
		earlycon_puts("OK\n");
	else
		earlycon_puts("FAIL\n");
	
	earlycon_puts("Creating tty stub dev...");
	if (!vfs_mknod("/faketty", S_IFCHR | 0777, 0x0200))
		earlycon_printf("OK\n");
	else
		earlycon_printf("FAIL\n");
	earlycon_printf("Extracting initrd...\n");
	armv7_initrd_extr(armv7_initrd_start_pa, armv7_initrd_end_pa);
	earlycon_printf("Initializing ipc...\n");
	ipc_init();
	earlycon_printf("Initializing system calls...\n");
	syscall_init();
	interrupt_init();
	platform_initialize();
	//armv7_enable_ints();
	earlycon_printf("Initializing task stacks...\n", heapmm_alloc(123));
	paging_map((void *) 0xBFFFF000, physmm_alloc_frame(), 
			PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
	paging_map((void *) 0xBFFFE000, physmm_alloc_frame(), 
			PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
	
	stack_switch_call(tasks_init, 0xBFFFFFF0);
	halt();
}



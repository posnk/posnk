/**
 * kernel/init/idle.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "kernel/streams.h"
#include "kernel/syscall.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>

static void process1_entry( process_info_t *proc )
{

	scheduler_reown_task( scheduler_current_task, proc );

	int fd = _sys_open( console_path, O_RDWR, 0 );

	if (fd < 0) {
		printf(CON_ERROR, "Error opening tty : %i\n",syscall_errno);
	} else {
		fd = _sys_dup(fd);
		if (fd < 0) {
			printf(CON_ERROR, "Error dupping stdin : %i\n",syscall_errno);
		}
		fd = _sys_dup(fd);
		if (fd < 0) {
			printf(CON_ERROR, "Error dupping stderr : %i\n",syscall_errno);
		}
	}

	char *ptr = NULL;
	void *nullaa = &ptr;

	int status = process_exec( init_path, nullaa, nullaa );

	if (status) {
		printf(CON_PANIC, "Error executing init : %i\n",status);
	}

	/* Reenable interrupts so the timer can wake us up.
	 * This is needed because we are still in privileged mode and this kernel
	 * masks interrupts while in it */
	enable();

	/* Wait for interrupts, forever. */
	for (;;)
		wait_int();
}

void kinit_start_uinit( void ) {
	pid_t pid_init;
	int init_status, rv;

	pid_init = scheduler_spawn( process1_entry, fork_process(), NULL );

	if ( pid_init < 0 )
		puts(CON_ERROR, "Failed to spawn init process\n");

	rv = sys_waitpid((uint32_t) 1,(uint32_t) &init_status,0,0,0,0);

	printf(CON_PANIC, "Init exited with status: %i %i\n",init_status,rv);

}

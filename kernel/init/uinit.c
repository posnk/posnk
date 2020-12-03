/**
 * kernel/init/uinit.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 * Handles starting the usermode init process.
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

/**
 * Main function for PID 1
 * Opens the console device and execs init.
 */
static void process1_entry( process_info_t *proc )
{
	int status, fd;
	char *ptr;
	void *nullaa;

	/* Attach the current task to the init process instead of the kernel */
	scheduler_reown_task( scheduler_current_task, proc );

	/* Open the console device */
	fd = _sys_open( console_path, O_RDWR, 0 );

	/* If the open was succesful, duplicate the fd twice. There are no open
	 * fds on this process yet so these will bind to 0,1,2 */
	if (fd < 0) {
		printf(CON_ERROR, "Error opening tty : %i", syscall_errno);
		goto error;
	}

	fd = _sys_dup(fd);
	if (fd < 0) {
		printf(CON_ERROR, "Error dupping stdin : %i", syscall_errno);
		goto error;
	}

	fd = _sys_dup(fd);
	if (fd < 0) {
		printf(CON_ERROR, "Error dupping stderr : %i", syscall_errno);
		goto error;
	}

	/* Prepare empty argument and environment lists */
	ptr = NULL;
	nullaa = &ptr;

	/* Load the init image and initialize the usermode state */
	status = process_exec( init_path, nullaa, nullaa );

	if ( status ) {
		printf(CON_PANIC, "Error executing init : %i", status);
		goto error;
	}

	/* Returning from a spawn entrypoint drops to user mode */
	return;

error:
	sys_exit( 255, 0, 0, 0, 0, 0 );

}

/**
 * @brief Start the usermode init process
 * This function spawns PID 1 and waits for it to exit.
 */
void kinit_start_uinit( void ) {
	pid_t pid_init;
	int init_status, rv;

	/* Spawn PID 1 as a fork of the kernel */
	pid_init = scheduler_spawn( process1_entry, fork_process(), NULL );

	/* Handle errors */
	if ( pid_init < 0 ) {
		printf( CON_ERROR,
		      "Failed to spawn init process: %i",
		      syscall_errno );
		return;
	}

	/* Wait for init to exit */
	rv = sys_waitpid( (uint32_t) 1, (uint32_t) &init_status, 0, 0, 0, 0 );

	printf(CON_PANIC, "Init exited with status: %i %i\n",init_status,rv);

}

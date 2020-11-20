/**
 * kernel/init/idle.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#define CON_SRC ("kinit")
#include "kernel/console.h"
#include "kernel/system.h"
#include "kernel/scheduler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>


/**
 * This function implements the idle task that runs when the scheduler had
 * no real processes to run.
 */
static void idle_task( process_info_t *proc )
{
	/* Assign the idle task to the kernel process (pid 0) */
	scheduler_reown_task( scheduler_current_task, proc );

	/* Tell the scheduler to register us as the idle task.
	 * This causes the scheduler to remove this task from the scheduling queue
	 * and store it as the task it will run when no other processes are ready*/
	scheduler_set_as_idle();

	/* Reenable interrupts so the timer can wake us up.
	 * This is needed because we are still in privileged mode and this kernel
	 * masks interrupts while in it */
	enable();

	/* Wait for interrupts, forever. */
	for (;;)
		wait_int();
}

void kinit_start_idle_task( void ) {
	pid_t pid_idle;

	printf(CON_INFO, "spinning up the idle task");
	pid_idle = scheduler_spawn( idle_task, current_process, NULL );

	if ( pid_idle < 0 )
		puts(CON_ERROR, "Failed to start idle task");
}

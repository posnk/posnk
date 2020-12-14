/**
 * kernel/synch.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */
/* XXX:
 * If this kernel is ever made to support SMP, replace critical sections with
 * atomic operations.
 */

#include "kernel/synch.h"
#include "kernel/heapmm.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#include <assert.h>

int spinlock_enter( spinlock_t *lock )
{
	int s;

	s = disable();

	while ( *lock )  {
#ifdef CONFIG_SERIAL_DEBUGGER_TRIG
		if (debugcon_have_data())
			dbgapi_invoke_kdbg(0);
#endif
		restore(s);
		//TODO: Wait for interrupts
		s = disable();
	};

	*lock = 1;

	return s;

}

void spinlock_exit( spinlock_t *lock, int s )
{

	*lock = 0;

	restore( s );

}
void semaphore_up(semaphore_t *semaphore)
{
	int s;
	s = disable();
	(*semaphore)++;
	restore( s );
	//Scheduler will check semaphore waits every time it is run.
}

void semaphore_add(semaphore_t *semaphore, unsigned int n)
{
	int s;
	s = disable();
	(*semaphore)+=n;
	restore( s );
	//Scheduler will check semaphore waits every time it is run.
}

/**
 * Tries to decrement a semaphore with a fixed timeout
 * If the timeout is reached before the semaphore became available this
 * function will return SCHED_WAIT_TIMEOUT. If the wait was interrupted for
 * any reason, this function will return SCHED_WAIT_INTR.
 */
int semaphore_ndown( semaphore_t *semaphore, utime_t timeout, int flags )
{
	/* Try to decrement the semaphore */
	if ( semaphore_try_down( semaphore ) )
		return SCHED_WAIT_OK;

	return scheduler_wait( semaphore, timeout, flags );
}

int semaphore_down(semaphore_t *semaphore)
{
	/* Decrement the semaphore */
	return semaphore_ndown(
		/* semaphore */ semaphore,
		/* timeout   */ 0,
		/* flags     */ 0 );

}

/**
 * Tries to decrement the semaphore, fails if it is already zero.
 * @param semaphore The semaphore to decrement
 * @return True if successful.
 */
int semaphore_try_down(semaphore_t *semaphore)
{
	int r,s;

	s = disable();
	if ((*semaphore) == 0) {
		r = 0;
	} else {
		(*semaphore)--;
		r = 1;
	}
	restore(s);
	return r;
}

/**
 * Initializes a semaphore
 */
void semaphore_init(semaphore_t *semaphore) {
	*(semaphore) = 0;
}


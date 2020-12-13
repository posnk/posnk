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
/**
 * If this kernel is ever made preeemptible, these functions should be made atomic!
 * This only works because there is NO way the kernel can be interrupted and resume
 * again during a system call except for a page fault, which is handled without accesing
 * user-mode semaphores
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
	
	//TODO: Make this SMP friendly ( atomic )
	
	while ( *lock )  {
#ifdef CONFIG_SERIAL_DEBUGGER_TRIG	
	if (debugcon_have_data())
		dbgapi_invoke_kdbg(0);
#endif
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
	(*semaphore)++;
	//Scheduler will check semaphore waits every time it is run.
}

void semaphore_add(semaphore_t *semaphore, unsigned int n)
{
	(*semaphore)+=n;
	//Scheduler will check semaphore waits every time it is run.
}

int semaphore_down(semaphore_t *semaphore)
{

	/* Try to decrement the semaphore */
	if ( semaphore_try_down( semaphore ) )
		return SCHED_WAIT_OK;
	
	/* Wait uninterruptably */
	//TODO: Should we handle process kills here?
	while ( scheduler_wait_on( semaphore ) != SCHED_WAIT_OK );
	
	return SCHED_WAIT_OK;
	
}

int semaphore_idown(semaphore_t *semaphore)
{
 
	/* Try to decrement the semaphore */
	if ( semaphore_try_down( semaphore ) )
		return SCHED_WAIT_OK;
		
	/* Ask the scheduler to block the thread */
	return scheduler_wait_on(semaphore );
	
}

/**
 * Tries to decrement a semaphore with a fixed timeout
 * If the timeout is reached before the semaphore became available this 
 * function will return SCHED_WAIT_TIMEOUT. If the wait was interrupted for
 * any reason, this function will return SCHED_WAIT_INTR.
 */
int semaphore_tdown(semaphore_t *semaphore, ktime_t seconds)
{

	/* Try to decrement the semaphore */
	if ( semaphore_try_down( semaphore ) )
		return SCHED_WAIT_OK;
	
	/* Ask the scheduler to block the thread */
	return scheduler_wait_on_timeout(semaphore, seconds);
	
}

/**
 * Tries to decrement a semaphore with a fixed timeout
 * If the timeout is reached before the semaphore became available this 
 * function will return SCHED_WAIT_TIMEOUT. If the wait was interrupted for
 * any reason, this function will return SCHED_WAIT_INTR.
 */
int semaphore_mdown(semaphore_t *semaphore, ktime_t micros)
{
	
	/* Try to decrement the semaphore */
	if ( semaphore_try_down( semaphore ) )
		return SCHED_WAIT_OK;
	
	/* Ask the scheduler to block the thread */
	return scheduler_wait_on_to_ms(semaphore, micros);
	
}

/**
 * Tries to decrement the semaphore, fails if it is already zero.
 * @param semaphore The semaphore to decrement
 * @return True if successful.
 */
int semaphore_try_down(semaphore_t *semaphore)
{
	if ((*semaphore) == 0)
		return 0;
	(*semaphore)--;
	return 1;	
}

/**
 * Allocates a semaphore
 */
semaphore_t *semaphore_alloc()
{
	semaphore_t *semaphore = (semaphore_t *) heapmm_alloc(sizeof(semaphore_t));
	*(semaphore) = 0;
	return semaphore;
}

/**
 * Frees a semaphore
 */
void semaphore_free(semaphore_t *semaphore)
{
	heapmm_free(semaphore, sizeof(semaphore_t));
}


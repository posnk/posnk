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
	if ((*semaphore) == 0) {
rw:
		scheduler_wait_on(semaphore);
		if (scheduler_current_task->state == PROCESS_INTERRUPTED){
			scheduler_current_task->state = PROCESS_RUNNING;
			goto rw;
			return -1;
		}
	} else {
		(*semaphore)--;
	}
	return 0;
}

int semaphore_idown(semaphore_t *semaphore)
{
	if ((*semaphore) == 0) {
		scheduler_wait_on(semaphore);
		if (scheduler_current_task->state == PROCESS_INTERRUPTED){
			scheduler_current_task->state = PROCESS_RUNNING;
			return -1;
		}
	} else {
		(*semaphore)--;
	}
	return 0;
}

int semaphore_try_down(semaphore_t *semaphore)
{
	if ((*semaphore) == 0)
		return 0;
	(*semaphore)--;
	return 1;	
}

semaphore_t *semaphore_alloc()
{
	semaphore_t *semaphore = (semaphore_t *) heapmm_alloc(sizeof(semaphore_t));
	*(semaphore) = 0;
	return semaphore;
}

void semaphore_free(semaphore_t *semaphore)
{
	heapmm_free(semaphore, sizeof(semaphore_t));
}


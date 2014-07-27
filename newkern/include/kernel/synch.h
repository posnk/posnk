/**
 * kernel/synch.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

#ifndef __KERNEL_SYNCH_H__
#define __KERNEL_SYNCH_H__

#include <stddef.h>
#include <stdint.h>

typedef uint32_t semaphore_t;

void semaphore_up(semaphore_t *semaphore);

void semaphore_add(semaphore_t *semaphore, unsigned int n);

int semaphore_down(semaphore_t *semaphore);

int semaphore_idown(semaphore_t *semaphore);

/* FOR INTERNAL USE BY SCHEDULER ONLY */
int  semaphore_try_down(semaphore_t *semaphore);

semaphore_t *semaphore_alloc();

void semaphore_free(semaphore_t *semaphore);

#endif

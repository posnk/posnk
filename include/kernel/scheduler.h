/**
 * kernel/scheduler.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 */

#ifndef __KERNEL_SCHEDULER_H__
#define __KERNEL_SCHEDULER_H__

#include <stddef.h>
#include <stdint.h>
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/time.h"
#include "util/llist.h"

typedef struct process_info scheduler_task_t;

extern scheduler_task_t *scheduler_current_task;

int scheduler_init_task(scheduler_task_t *new_task);

void scheduler_switch_task(scheduler_task_t *new_task);

void scheduler_fork_main( void * arg );

int scheduler_do_spawn( scheduler_task_t *new_task, void *callee, void *arg );

int scheduler_spawn( void *callee, void *arg );

void scheduler_init();

void scheduler_wait_on(semaphore_t *semaphore);

void scheduler_wait_on_timeout(semaphore_t *semaphore, ktime_t seconds);

void scheduler_wait_on_to_ms(semaphore_t *semaphore, ktime_t micros);

int scheduler_wait_time(ktime_t time);

int scheduler_wait_micros(ktime_t microtime);

int scheduler_fork();

void schedule();

void scheduler_set_as_idle();

#endif

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

int scheduler_invoke_signal_handler(int signal);

void scheduler_exit_signal_handler();

void scheduler_switch_task(scheduler_task_t *new_task);

int scheduler_fork_to(scheduler_task_t *new_task);

void scheduler_init();

void scheduler_wait_on(semaphore_t *semaphore);

int scheduler_fork();

void schedule();

void scheduler_set_as_idle();

#endif

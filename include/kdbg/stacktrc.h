/**
 * kdbg/stacktrc.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#ifndef __KDBG_STACKTRC_H__
#define __KDBG_STACKTRC_H__

#include "util/llist.h"
#include <stdint.h>
#include "kernel/scheduler.h"

typedef struct kdbg_calltrace {
	llist_t		link;
	uintptr_t	func_addr;
	uintptr_t	frame_addr;
} kdbg_calltrace_t;

llist_t *kdbg_do_calltrace();

void kdbg_pt_calltrace(scheduler_task_t *t);

void kdbg_free_calltrace(llist_t *st);

void kdbg_print_calltrace(llist_t *st);

#endif

/**
 * kdbg/taskdbg.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-07-2014 - Created
 */
#include "kdbg/stacktrc.h"
#include "kdbg/kdbgio.h"
#include "kdbg/kdbgmm.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"

extern char *syscall_names[];
#define PROCESS_RUNNING 	0
#define PROCESS_WAITING 	1
#define PROCESS_READY		2
#define PROCESS_NO_SCHED	3
#define PROCESS_KILLED		4
#define PROCESS_INTERRUPTED	5

extern llist_t *scheduler_task_list;

char * kdbg_procstates[] = {
	"RUNNING",
	"WAITING",
	"READY",
	"NO_SCHED",
	"KILLED",
	"INTERRUPTED",
	"TIMED_OUT",
	"STOPPED"
};

void kdbg_dump_processes()
{
	process_info_t *p;
	scheduler_task_t *t;
	llist_t *_p,*_t;
	for (_p = process_list->next; _p != process_list; _p = _p->next) {
		p = (process_info_t *) _p;
		kdbg_printf("[%i] %s : %s\n", p->pid, p->name, kdbg_procstates[p->state]);
		for (_t  = p->tasks.next;
			 _t != &p->tasks;
			 _t  = _t->next) {
			 t = (scheduler_task_t *)_t;
			 kdbg_printf("    [%i] %s", t->tid, kdbg_procstates[t->state]);
		if (t->in_syscall != 0xFFFFFFFF)
			kdbg_printf(" sc: %s\n", syscall_names[t->in_syscall]);
		else
			kdbg_printf("\n");
			}
	}
}
void debug_attach_task(process_info_t *new_task);
void kdbg_attach_process(int pid)
{
	debug_attach_task(process_get(pid));
}

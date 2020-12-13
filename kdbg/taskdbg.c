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
#include <string.h>
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

char * kdbg_taskstates[] = {
	"RUNNING ",
	"READY ",
	"STOPPED ",
	"TIMEDWAIT_US ",
	"TIMEDWAIT_S ",
	"BLOCKED ",
	"6 ",
	"7 ",
	"INTERRUPTED ",
	"TIMED_OUT "
};
#define TASK_STATE_RUNNING	(1 << 0)
#define TASK_STATE_READY	(1 << 1)
#define TASK_STATE_STOPPED	(1 << 2)
#define TASK_STATE_TIMEDWAIT_US	(1 << 3)
#define TASK_STATE_TIMEDWAIT_S	(1 << 4)
#define TASK_STATE_TIMEDWAIT	(3 << 3)
#define TASK_STATE_BLOCKED	(1 << 5)
#define TASK_STATE_INTERRUPTED	(1 << 8)
#define TASK_STATE_TIMED_OUT	(1 << 9)


char *kdbg_taskstatestr(int s) {
	static char state[80];
	int i;
	state[0] = 0;
	for ( i = 0; i < 10; i++ )
		if ( s & (1 <<i) )
			strcat( state, kdbg_taskstates[i] );
	return state;
}

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
			 kdbg_printf("    [%i] %s\n", t->tid, kdbg_taskstatestr(t->state));
			 if(t->state != 0)
			 kdbg_pt_calltrace(t);
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

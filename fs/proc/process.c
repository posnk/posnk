#include "fs/proc/proc.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include <stdio.h>
#include <string.h>

static const char * procstates[] = {
	"RUNNING",
	"WAITING",
	"READY",
	"NO_SCHED",
	"KILLED",
	"INTERRUPTED",
	"TIMED_OUT",
	"STOPPED"
};

errno_t proc_process_open ( snap_t *snap, ino_t inode ) {
	pid_t pid = PROC_INODE_PID( inode );
	snap_appenddir( snap, "."    , inode );
	snap_appenddir( snap, ".."   , PROC_INODE(   0, PROC_INO_ROOT ) );
	snap_appenddir( snap, "tasks", PROC_INODE( pid, PROC_INO_P_TASKS ));
	snap_appenddir( snap, "name" , PROC_INODE( pid, PROC_INO_P_NAME ));
	snap_appenddir( snap, "state", PROC_INODE( pid, PROC_INO_P_STATE ));
	return 0;
}

errno_t proc_proc_tasks_open ( snap_t *snap, ino_t inode ) {
	process_info_t *p = process_get( PROC_INODE_PID( inode ) );
	scheduler_task_t *t;
	llist_t *_t;
	char name[16];

	for (_t  = p->tasks.next; _t != &p->tasks; _t  = _t->next) {
		t = (scheduler_task_t *)_t;
		snprintf( name, sizeof name, "%i", t->tid );
		snap_appenddir( snap, name, PROC_INODE(t->tid, PROC_INO_T_DIR));
	}
	return 0;
}

errno_t proc_proc_name_open ( snap_t *snap, ino_t inode ) {
	process_info_t *p = process_get( PROC_INODE_PID( inode ) );

	if ( !p || !p->name)
		return EINVAL;

	snap_setline( snap, p->name );

	return 0;
}

errno_t proc_proc_state_open ( snap_t *snap, ino_t inode ) {
	process_info_t *p = process_get( PROC_INODE_PID( inode ) );

	if ( !p || p->state < 0 || p->state >= 8 )
		return EINVAL;

	snap_setline( snap, procstates[p->state] );

	return 0;
}

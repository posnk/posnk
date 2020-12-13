#include "fs/proc/proc.h"
#include "kernel/process.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include <stdio.h>
#include <string.h>
extern char *syscall_names[];

static char * taskstates[] = {
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

errno_t proc_task_open ( snap_t *snap, ino_t inode ) {
	tid_t tid = PROC_INODE_TID( inode );
	scheduler_task_t *t = scheduler_get_task( PROC_INODE_TID( inode ) );

	if ( !t )
		return EINVAL;

	snap_appenddir( snap, "."     , inode );
	snap_appenddir( snap, ".."    , PROC_INODE( t->process->pid, 0x001 ) );
	snap_appenddir( snap, "state" , PROC_INODE( tid, 0x006 ) );
	snap_appenddir( snap, "syscall", PROC_INODE( tid, 0x007 ) );
	snap_appenddir( snap, "cpustate", PROC_INODE( tid, 0x008 ) );
	return 0;
}

errno_t proc_task_state_open ( snap_t *snap, ino_t inode ) {
	scheduler_task_t *t = scheduler_get_task( PROC_INODE_TID( inode ) );
	char state[80];
	int i,s;

	if ( !t )
		return EINVAL;
	s = t->state;
	state[0] = 0;
	for ( i = 0; i < 10; i++ )
		if ( s & (1 <<i) )
			strcat( state, taskstates[i] );

	snap_setline( snap, state );

	return 0;
}

errno_t proc_task_syscall_open ( snap_t *snap, ino_t inode ) {
	scheduler_task_t *t = scheduler_get_task( PROC_INODE_TID( inode ) );

	if ( !t )
		return EINVAL;
	if ( t->in_syscall == 0xFFFFFFFF )
		snap_setline( snap, "(running)" );
	else
		snap_setline( snap, syscall_names[t->in_syscall] );

	return 0;
}

errno_t proc_task_cpustate_open ( snap_t *snap, ino_t inode ) {
	scheduler_task_t *t = scheduler_get_task( PROC_INODE_TID( inode ) );
	aoff_t ws;
	if ( !t || !t->arch_state)
		return EINVAL;

	snap_write( snap, 0, t->arch_state, scheduler_get_state_size(), &ws );

	return 0;
}

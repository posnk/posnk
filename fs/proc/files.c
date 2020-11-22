#include "fs/proc/proc.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include <stdio.h>
#include <string.h>

errno_t proc_root_open ( proc_snap_t *snap, ino_t inode ) {
	process_info_t *p;
	llist_t *_p;
	char name[16];

	proc_snap_appenddir( snap, "."    , inode);
	proc_snap_appenddir( snap, ".."   , 0x000 );

	for (_p = process_list->next; _p != process_list; _p = _p->next) {
		p = (process_info_t *) _p;
		snprintf( name, sizeof name, "%i", p->pid );
		proc_snap_appenddir( snap, name, PROC_INODE(p->pid, 0x001) );
	}

	return 0;
}

errno_t proc_snap_setstring( proc_snap_t *snap, const char *str )
{
	aoff_t wr;
	return proc_snap_write( snap, 0, str, strlen(str), &wr );
}

errno_t proc_snap_setline( proc_snap_t *snap, const char *str )
{
	errno_t status;
	aoff_t wr;
	char nl='\n';

	status = proc_snap_write( snap, 0, str, strlen(str), &wr );
	if (status)
		return status;

	status = proc_snap_write( snap, strlen(str), &nl, 1, &wr );
	if (status)
		return status;

	return 0;
}

proc_file_t proc_file_list[] = {
	{    // 0
		.name="<root>",
		.mode=S_IFDIR | 0555,
		.size=4096,
		.open=proc_root_open,
		.flags=0
	}, { // 1
		.name="<process>",
		.mode=S_IFDIR | 0555,
		.size=4096,
		.open=proc_process_open,
		.flags=0
	}, { // 2
		.name="name",
		.mode=S_IFREG | 0444,
		.size=4096,
		.open=proc_proc_name_open,
		.flags=0
	}, { // 3
		.name="state",
		.mode=S_IFREG | 0444,
		.size=4096,
		.open=proc_proc_state_open,
		.flags=0
	}, { // 4
		.name="<tasks>",
		.mode=S_IFDIR | 0555,
		.size=4096,
		.open=proc_proc_tasks_open,
		.flags=0
	}, { // 5
		.name="<task>",
		.mode=S_IFDIR | 0555,
		.size=4096,
		.open=proc_task_open,
		.flags=0
	}, { // 6
		.name="state",
		.mode=S_IFREG | 0555,
		.size=4096,
		.open=proc_task_state_open,
		.flags=0
	}, { // 7
		.name="syscall",
		.mode=S_IFREG | 0555,
		.size=4096,
		.open=proc_task_syscall_open,
		.flags=0
	}
};

proc_file_t *proc_get_file( ino_t id ) {
	int file_id;

	file_id = PROC_INODE_FILE(id);

	if ( file_id < 0 || file_id >= (sizeof proc_file_list / sizeof(proc_file_t)) )
		return NULL;

	return proc_file_list + file_id;

}

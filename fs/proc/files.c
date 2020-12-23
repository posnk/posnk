#include "fs/proc/proc.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include <stdio.h>
#include <string.h>

errno_t proc_root_open ( snap_t *snap, ino_t inode ) {
	process_info_t *p;
	llist_t *_p;
	char name[16];

	snap_appenddir( snap, "."    , inode );
	snap_appenddir( snap, ".."   , PROC_INO_ROOT );

	for (_p = process_list->next; _p != process_list; _p = _p->next) {
		p = (process_info_t *) _p;
		snprintf( name, sizeof name, "%i", p->pid );
		snap_appenddir( snap, name, PROC_INODE(p->pid, PROC_INO_P_DIR));
	}

	return 0;
}

errno_t snap_setstring( snap_t *snap, const char *str )
{
	aoff_t wr;
	return snap_write( snap, 0, str, strlen(str), &wr );
}

errno_t snap_setline( snap_t *snap, const char *str )
{
	errno_t status;
	aoff_t wr;
	char nl='\n';

	status = snap_write( snap, 0, str, strlen(str), &wr );
	if (status)
		return status;

	status = snap_write( snap, strlen(str), &nl, 1, &wr );
	if (status)
		return status;

	return 0;
}

#define PROC_DEF_FILE_INT( Id, Name, Open, Size, Mode, Flags ) \
	proc_file_list[Id].name = Name; \
	proc_file_list[Id].mode = Mode; \
	proc_file_list[Id].size = Size; \
	proc_file_list[Id].open = Open; \
	proc_file_list[Id].flags = Flags

#define PROC_MODE_FR_OWN  (S_IFREG | 0400)
#define PROC_MODE_FRX_OWN (S_IFREG | 0500)
#define PROC_MODE_DRX_OWN (S_IFDIR | 0500)

#define DEF_FILE( Id, Name, Open, Size ) \
	PROC_DEF_FILE_INT( Id, Name, Open, Size, PROC_MODE_FR_OWN, 0 )

#define DEF_DIR( Id, Name, Open, Size ) \
	PROC_DEF_FILE_INT( Id, Name, Open, Size, PROC_MODE_DRX_OWN, 0 )

proc_file_t proc_file_list[64];

proc_file_t *proc_get_file( ino_t id ) {
	int file_id;

	file_id = PROC_INODE_FILE(id);

	if ( file_id < 0 || file_id >= (int) (sizeof proc_file_list / sizeof(proc_file_t)) )
		return NULL;

	if ( proc_file_list[ file_id ].open == NULL &&
	     proc_file_list[ file_id ].custopen == NULL )
		return NULL;

	return proc_file_list + file_id;

}

void proc_init_files( void ) {
	DEF_DIR(  PROC_INO_ROOT, "<root>", proc_root_open, 4096 );
	DEF_DIR(  PROC_INO_P_DIR, "<pdir>", proc_process_open, 4096 );
	DEF_FILE( PROC_INO_P_NAME, "name", proc_proc_name_open, 4096 );
	DEF_FILE( PROC_INO_P_STATE, "state", proc_proc_state_open, 4096 );
	DEF_DIR(  PROC_INO_P_TASKS, "tasks", proc_proc_tasks_open, 4096 );
	DEF_DIR(  PROC_INO_T_DIR, "<tdir>", proc_task_open, 4096 );
	DEF_FILE( PROC_INO_T_STATE, "state", proc_task_state_open, 4096 );
	DEF_FILE( PROC_INO_T_SYSCALL, "syscall", proc_task_syscall_open, 4096 );
	DEF_FILE( PROC_INO_T_MCONTEXT, "mcontext", proc_task_mcontext_open, 4096 );
	proc_file_list[ PROC_INO_P_MEM ].custopen = proc_open_mem_inode;
    proc_file_list[ PROC_INO_P_MEM ].size = 0xC0000000; /* so we can't map kernel mem */
    proc_file_list[ PROC_INO_P_MEM ].flags = PROC_FLAG_NOT_SNAP;
    proc_file_list[ PROC_INO_P_MEM ].mode = PROC_MODE_FR_OWN;
    proc_file_list[ PROC_INO_P_MEM ].name = "mem";

}

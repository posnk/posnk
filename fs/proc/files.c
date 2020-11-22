#include "fs/proc/proc.h"



errno_t proc_root_open ( proc_snap_t *snap, ino_t inode ) {
	proc_snap_appenddir( snap, "file0", 0x001 );
	proc_snap_appenddir( snap, "file1", 0x002 );
	proc_snap_appenddir( snap, "file2", 0x003 );
	return 0;
}


proc_file_t proc_file_list[] = {
	{
		.name="<root>",
		.mode=S_IFDIR | 0555,
		.size=4096,
		.open=proc_root_open,
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

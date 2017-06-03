/**
 * kernel/process.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */
#include <string.h>
#include <stddef.h>
#include <sys/errno.h>
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/scheduler.h"
#include "kernel/syscall.h"
#include "kernel/paging.h"
#include "kernel/permissions.h"
#include "kernel/earlycon.h"
#include "kernel/streams.h"
#include "kernel/elf.h"
#include "kernel/elfloader.h"
#include "kernel/signals.h"
#include "kernel/heapmm.h"
#include "config.h"

int strlistlen(char **list)
{
	int len = 0;
	while (list[len] != 0)
		len++;
	return len;
}

int process_exec( char *path, char **args, char **envs )
{
	char *sb_sp, *sb_nl, *sb_path, *sb_arg, *name;
	char *header;
	aoff_t rd_count;
	inode_t *inode;
	int status;
	size_t arg_size, env_size, argl_size, envl_size, args_size, envs_size, c, d;
	uintptr_t ptr;
	char **n_args;
	char **n_envs;

	struct siginfo sigi;

	memset( &sigi, 0, sizeof( struct siginfo ) );

exec_start:

	header = heapmm_alloc( 128 * sizeof( char ) );
	//TODO: Figure out how to free this
	if ( header == NULL ) {
		return ENOMEM;
	}

	/* Get file inode */
	status = vfs_find_inode(path, &inode);

	if (status) {
		return status;
	}

	/* We can't execute special files */
	if (!S_ISREG(inode->mode)) {
		vfs_inode_release(inode);
		return EACCES;
	}

	/* Check for execute permission */
	if (!vfs_have_permissions(inode, MODE_EXEC)) {
		vfs_inode_release(inode);
		return EACCES;
	}

	/* Read file header */
	memset( header, 0, 128 );
	status = vfs_read( inode, 0, header, 127, &rd_count, 0 );

	if ( status ) {
		vfs_inode_release(inode);
		return status;
	}

	/* Check for shebang */
	if ( rd_count >= 2	&& header[0] == '#' 
						&& header[1] == '!' ) {
		
		/* Close shell file */
		vfs_inode_release( inode );

		/* Find first newline, if present */
		sb_nl = strchr( header + 2, '\n' );

		/* If not, it is not a valid executable */
		if ( !sb_nl )
			return ENOEXEC;

		/* Terminate the string */
		*sb_nl = 0;
	
		/* Check if we have an argument */
		sb_sp = strchr( header + 3, ' ' );

		/* Get path */
		sb_path = header + 2;

		if ( *sb_path == ' ' )
			sb_path++;

		/* Get argument if we have it */
		if ( sb_sp ) {

			/* Argument starts after the space character */
			sb_arg = sb_sp + 1;

			/* Set the space character to terminate the path */
			*sb_sp = 0;

		} else
			sb_arg = NULL;

		argl_size = strlistlen(args);
		
		if ( sb_arg )
			d = 2;
		else
			d = 1;
	
		n_args = heapmm_alloc( sizeof( char * ) * ( argl_size + d + 1 ) );

		for ( c = 0; c < argl_size; c++ )
			n_args[ c + d ] = args[ c ];

		n_args[0] = sb_path;
		n_args[argl_size+d] = NULL;

		if ( sb_arg )
			n_args[1] = sb_arg;

		args = n_args;

		if ( strcmp( path, sb_path ) == 0 ) {
			
			return ELOOP;

		}

		path = sb_path;

		//TODO: Move exec syscall in here to prevent leaking the kernel
		//		copy of argv
		
		goto exec_start;

	}

	/* We should now be looking at an ELF file */
	if ( !elf_verify( header, ( size_t ) header ) ) {
		vfs_inode_release( inode );
		return ENOEXEC;
	}

	//TODO: Should this be the script or interpreter
	status = vfs_get_filename( path, &name );

	if (status) {
		vfs_inode_release(inode);
		return status;
	}

	strcpy(scheduler_current_task->name, name);

	heapmm_free(name, strlen(name) + 1);

	if (scheduler_current_task->image_inode) {
		scheduler_current_task->image_inode->open_count--;
		vfs_inode_release(scheduler_current_task->image_inode);
	}

	procvmm_clear_mmaps();

	signal_init_task( scheduler_current_task );

	status = elf_load( inode );

	vfs_inode_release( inode );

	if (status) {
		debugcon_printf("error loading elf %s\n",path);
		process_send_signal(scheduler_current_task, SIGSYS, sigi);
		do_signals();
		schedule();
		return status;	//NEVER REACHED
	}

	/* Pre exec bookkeeping */
	stream_do_close_on_exec();

	/* Prepare environment */
	status = procvmm_do_exec_mmaps();
	if (status) {
		debugcon_printf("error mmapping stuff\n");
		process_send_signal(scheduler_current_task, SIGSYS, sigi);
		do_signals();
		schedule();
		return status;	//NEVER REACHED
	}
	//TODO: Verify arg and env list sizes

	argl_size = strlistlen(args);
	
	args_size = 0;

	for (c = 0; c < argl_size; c++)
		args_size += strlen(args[c]) + 1;

	arg_size = (argl_size + 1) * sizeof(char *) + args_size;

	procvmm_mmap_anon((void *) 0x4000000, arg_size, PROCESS_MMAP_FLAG_WRITE, "(args)");

	ptr = 0x4000000 + sizeof(char *) * (argl_size + 1);
	n_args = (char **) 0x4000000;

	for (c = 0; c < argl_size; c++) {
		strcpy((char *)ptr, args[c]);
		n_args[c] = (char *) ptr;		
		ptr += strlen(args[c]) + 1;
	}
	n_args[argl_size] = (char *) 0;

	envl_size = strlistlen(envs);
	
	envs_size = 0;

	for (c = 0; c < envl_size; c++)
		envs_size += strlen(envs[c]) + 1;

	env_size = sizeof(char *) * (envl_size + 1) + envs_size;

	procvmm_mmap_anon((void *) 0x4020000, env_size, PROCESS_MMAP_FLAG_WRITE, "(envs)");

	ptr = 0x4020000 + sizeof(char *) * (envl_size + 1);
	n_envs = (char **) 0x4020000;

	for (c = 0; c < envl_size; c++) {
		strcpy((char *)ptr, envs[c]);
		n_envs[c] = (char *) ptr;		
		ptr += strlen(envs[c]) + 1;
	}
	n_envs[envl_size] = (char *) 0;
	//debugcon_printf("calling program\n");

	process_user_call(scheduler_current_task->entry_point, scheduler_current_task->stack_bottom);
	return 0;//NEVER REACHED

}


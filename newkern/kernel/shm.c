/**
 * kernel/shm.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */
#include <string.h>
#include <sys/errno.h>
#include "kernel/shm.h"
#include "kernel/ipc.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/heapmm.h"
#include "kernel/time.h"
#include "kernel/paging.h"

llist_t	shm_list;
int shm_id_ctr = 0;

void shm_init()
{
	llist_create(&shm_list);
}

/**
 * Iterator to test if shm id matches the id
 */
int shm_id_search_iterator (llist_t *node, void *param)
{
	int id = (int) param;
	shm_info_t *ptr = (shm_info_t *) node;
	return ptr->id == id;
}

/**
 * Iterator to test if shm key matches the key
 */
int shm_key_search_iterator (llist_t *node, void *param)
{
	key_t key = (key_t) param;
	shm_info_t *ptr = (shm_info_t *) node;
	return ptr->key == key;
}

shm_info_t *shm_get_by_key(key_t key)
{
	return (shm_info_t *) llist_iterate_select(&shm_list, &shm_key_search_iterator, (void *) key);
}

shm_info_t *shm_get_by_id(int id)
{
	return (shm_info_t *) llist_iterate_select(&shm_list, &shm_id_search_iterator, (void *) id);
}

int shm_alloc_id()
{
	int id = shm_id_ctr++;
	if(shm_id_ctr <= 0) {
		shm_id_ctr--;
		syscall_errno = ENFILE;
		return -1;
	}

	return id;
}

void *_sys_shmat(int shmid, void *shmaddr, int shmflg)
{
	int flags = 0;
	void *st;
	shm_info_t *info;

	info = shm_get_by_id(shmid);
	if (!info) {
		syscall_errno = EINVAL;
		return (void *) -1;
	}
	if (info->del) {
		syscall_errno = EIDRM;
		return (void *) -1;
	}
	if (shmflg & SHM_RDONLY) {
		if (!ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_READ)) {
			syscall_errno = EACCES;
			return (void *) -1;
		}
	} else {
		if (!ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_READ | IPC_PERM_WRITE)) {
			syscall_errno = EACCES;
			return (void *) -1;
		}
		flags |= PROCESS_MMAP_FLAG_WRITE;
	}
	st = procvmm_attach_shm(shmaddr, info, flags);
	if ((!info->zero) && (st != (void *) -1) && (flags & PROCESS_MMAP_FLAG_WRITE)) {
		memset(shmaddr, 0, info->info.shm_segsz);
		info->zero = 1;		
	}
	return st;
}

int _sys_shmdt(void *shmaddr)
{
	return procvmm_detach_shm(shmaddr);
	
}


int _sys_shmctl(int id, int cmd, struct shmid_ds *buf)
{
	struct shmid_ds ubuf;
	shm_info_t *info;

	info = shm_get_by_id(id);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}

	
	switch (cmd) {
		case IPC_STAT:
			if (!ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			if (!copy_kern_to_user(&(info->info), buf, sizeof(struct shmid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;
		case IPC_SET:
			if ((!ipc_is_creator(&(info->info.shm_perm))) && !ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			if (!copy_user_to_kern(buf, &ubuf, sizeof(struct shmid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			info->info.shm_perm.uid = ubuf.shm_perm.uid;
			info->info.shm_perm.gid = ubuf.shm_perm.gid;
			info->info.shm_perm.mode &= ~0x1FF;
			info->info.shm_perm.mode |= ubuf.shm_perm.mode & 0x1FF;
			return 0;
		case IPC_RMID:
			if ((!ipc_is_creator(&(info->info.shm_perm))) && !ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			info->del = 1;
			if (!info->info.shm_nattch) {
				shm_do_delete(info);
				
			}
			return 0;
		default: 
			syscall_errno = EINVAL;
			return -1;
	}
}

void shm_do_delete(shm_info_t *info) {
	int nframes,n;
	llist_unlink((llist_t *) info);
	nframes = info->info.shm_segsz / PHYSMM_PAGE_SIZE;
	for (n = 0; n < nframes; n++)
		physmm_free_frame(info->frames[n]);
	heapmm_free(info->frames, sizeof(physaddr_t) * nframes);
	heapmm_free(info, sizeof(shm_info_t));
}

int _sys_shmget(key_t key, size_t size, int flags)
{
	int nframes, n;
	shm_info_t *info = NULL;
	if (key != IPC_PRIVATE)
		info = shm_get_by_key(key);	
	if (info && (flags & IPC_CREAT) && (flags & IPC_EXCL)) {
		syscall_errno = EEXIST;
		return -1;
	} else if (!info) {
		if ((key != IPC_PRIVATE) && !(flags & IPC_CREAT)) {
			syscall_errno = ENOENT;
			return -1;
		}
		size = (size + PHYSMM_PAGE_ADDRESS_MASK) & ~PHYSMM_PAGE_ADDRESS_MASK;
		info = heapmm_alloc(sizeof(shm_info_t));
		if (!info) {
			syscall_errno = ENOMEM;
			return -1;
		}

		/* Allocate frames */
		nframes = size / PHYSMM_PAGE_SIZE;
		info->frames = heapmm_alloc(sizeof(physaddr_t) * nframes);
		if (!info->frames) {
			heapmm_free(info, sizeof(shm_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		for (n = 0; n < nframes; n++) {
			info->frames[n] = physmm_alloc_frame();
			if (info->frames[n] == PHYSMM_NO_FRAME) {
				nframes = n;
				for (n = 0; n < nframes; n++)
					physmm_free_frame(info->frames[n]);
				heapmm_free(info->frames, sizeof(physaddr_t) * nframes);
				heapmm_free(info, sizeof(shm_info_t));
				syscall_errno = ENOMEM;
				return -1;
				
			}
		}

		/* Initialize info */
		info->id = shm_alloc_id();
		if (info->id == -1) {
			for (n = 0; n < nframes; n++)
				physmm_free_frame(info->frames[n]);
			heapmm_free(info->frames, sizeof(physaddr_t) * nframes);
			heapmm_free(info, sizeof(shm_info_t));
			return -1;
		}

		info->zero = 0;
		info->del = 0;
		info->key = key;

		/* Initialize shmid_ds */
		info->info.shm_perm.cuid = info->info.shm_perm.uid = get_effective_uid();
		info->info.shm_perm.cgid = info->info.shm_perm.gid = get_effective_gid();
		info->info.shm_perm.mode = flags & 0x1FF;
		info->info.shm_segsz = size;
		info->info.shm_cpid = scheduler_current_task->pid;
		info->info.shm_lpid = 0;
		info->info.shm_nattch = 0;
		info->info.shm_atime = 0;
		info->info.shm_dtime = 0;
		info->info.shm_ctime = (time_t) system_time;

		llist_add_end(&shm_list, (llist_t *) info);
	} else if (!ipc_have_permissions(&(info->info.shm_perm), IPC_PERM_READ)) {
		syscall_errno = EACCES;
		return -1;
	} else if (size && (size < info->info.shm_segsz)) {
		syscall_errno = EINVAL;
		return -1;
	} else if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}	
	return info->id;
}

/**
 * kernel/sem.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */
#include <string.h>
#include <assert.h>
#include <sys/errno.h>
#include "kernel/sem.h"
#include "kernel/ipc.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/heapmm.h"
#include "kernel/time.h"

llist_t	sem_list;
int sem_id_ctr = 0;

void sem_init()
{
	llist_create(&sem_list);
}

/**
 * Iterator to test if sem id matches the id
 */
int sem_id_search_iterator (llist_t *node, void *param)
{
	int id = (int) param;
	sem_info_t *ptr = (sem_info_t *) node;
	return ptr->id == id;
}

/**
 * Iterator to test if sem key matches the key
 */
int sem_key_search_iterator (llist_t *node, void *param)
{
	key_t key = (key_t) param;
	sem_info_t *ptr = (sem_info_t *) node;
	return ptr->key == key;
}

static inline sem_info_t *sem_get_by_key(key_t key)
{
	return (sem_info_t *) llist_iterate_select(&sem_list, &sem_key_search_iterator, (void *) key);
}

static inline sem_info_t *sem_get_by_id(int id)
{
	return (sem_info_t *) llist_iterate_select(&sem_list, &sem_id_search_iterator, (void *) id);
}

int sem_alloc_id()
{
	int id = sem_id_ctr++;
	if(sem_id_ctr <= 0) {
		sem_id_ctr--;
		syscall_errno = ENFILE;
		return -1;
	}

	return id;
}

static inline int sysv_sem_checkblock_int(sem_info_t *info, short semnum, short semop, __attribute__((__unused__)) short semflg)
{
	assert(info != NULL);
	assert(semnum >= 0);
	assert(semnum < info->info.sem_nsems);
	if (semop == 0) 
		return (info->sems[semnum].semval != 0);
	if (semop > 0)
		return 0;
	if (semop < 0)
		return (info->sems[semnum].semval < (-semop));
	assert(0);
	return -1;
}

static inline int _sys_semop_int(sem_info_t *info, short semnum, short semop, __attribute__((__unused__)) short semflg)
{
	assert(info != NULL);
	if ((semnum < 0) || (semnum >= info->info.sem_nsems)) {
		syscall_errno = EFBIG;
		return -1;
	}
	if (semop == 0) {
		assert (info->sems[semnum].semval == 0);
		return 0;
	} else if (semop > 0) {
		info->sems[semnum].semval += semop;
		semaphore_add(info->nwaitsem, info->sems[semnum].semncnt);
		info->sems[semnum].sempid = current_process->pid;
		return 0;
	} else if (semop < 0) {
		assert (info->sems[semnum].semval >= (-semop));
		info->sems[semnum].semval -= (-semop);
		if (!info->sems[semnum].semval) 
			semaphore_add(info->zwaitsem, info->sems[semnum].semzcnt);
		info->sems[semnum].sempid = current_process->pid;
		return 0;
	}
	assert(0);
	return -1;
}

int _sys_semop(int semid, struct sembuf *sops, size_t nsops)
{
	int block = 1, again = 0, waitz = 0, nblock;
	size_t n;
	sem_info_t *info;

	assert (sops != NULL);

	info = sem_get_by_id(semid);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}

	info->refs++;
	while (block) {
		block = 0;
		for (n = 0; n < nsops; n++) {
			if (sysv_sem_checkblock_int(info, sops[n].sem_num, sops[n].sem_op, sops[n].sem_flg)) {	
				block = 1;
				again = sops[n].sem_flg & IPC_NOWAIT;
				waitz = sops[n].sem_op == 0;
				nblock = n;
				break;
			}
		}
		if (block) {
			if (again) {
				info->refs--;
				syscall_errno = EAGAIN;
				return -1;
			}
			if (waitz) {
				info->sems[nblock].semzcnt++;
				if (semaphore_idown(info->zwaitsem)) {
					info->sems[nblock].semzcnt--;
					info->refs--;
					syscall_errno = EINTR;
					if (info->del && !info->refs)
						sem_do_delete(info);
					return -1;
				}
				info->sems[nblock].semzcnt--;
			} else { 
				info->sems[nblock].semncnt++;
				if (semaphore_idown(info->nwaitsem)) {
					info->sems[nblock].semncnt--;
					info->refs--;
					syscall_errno = EINTR;
					if (info->del && !info->refs)
						sem_do_delete(info);
					return -1;
				}
				info->sems[nblock].semncnt--;
			}
			if (info->del) {
				info->refs--;
				if (!info->refs)
					sem_do_delete(info);
				syscall_errno = EIDRM;
				return -1;
			}
		}
	}

	for (n = 0; n < nsops; n++) {
		if (_sys_semop_int(info, sops[n].sem_num, sops[n].sem_op, sops[n].sem_flg))
			return -1;
	}

	info->info.sem_otime = (time_t) system_time;
	info->refs--;
	return 0;
}



int _sys_semctl(int id, int semnum, int cmd, void *buf)
{
	int n;
	struct semid_ds ubuf;
	unsigned short *allbuf;
	sem_info_t *info;

	info = sem_get_by_id(id);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	if ((semnum < 0) || (semnum > info->info.sem_nsems)) {
		syscall_errno = EINVAL;
		return -1;
	}
	 
	switch (cmd) {
		case GETVAL:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			return (int) info->sems[semnum].semval;
		case SETVAL:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_WRITE)) {
				syscall_errno = EACCES;
				return -1;
			}
			info->sems[semnum].semval = (int) buf;
			return 0;

		case GETPID:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			return (int) info->sems[semnum].sempid;
		case GETNCNT:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			return (int) info->sems[semnum].semncnt;
		case GETZCNT:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			return (int) info->sems[semnum].semzcnt;

		case GETALL:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			allbuf = heapmm_alloc(sizeof(unsigned short) * info->info.sem_nsems);
			if (!allbuf) {
				syscall_errno = ENOMEM;
				return -1;
			}
			for (n = 0; n < info->info.sem_nsems; n++)
				allbuf[n] = info->sems[n].semval;
			if (!copy_kern_to_user(allbuf, buf, sizeof(unsigned short) * info->info.sem_nsems)){
				syscall_errno = EFAULT;
				heapmm_free(allbuf, sizeof(unsigned short) * info->info.sem_nsems);
				return -1;
			}	
			heapmm_free(allbuf, sizeof(unsigned short) * info->info.sem_nsems);
			return 0;
		case SETALL:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_WRITE)) {
				syscall_errno = EACCES;
				return -1;
			}
			allbuf = heapmm_alloc(sizeof(unsigned short) * info->info.sem_nsems);
			if (!allbuf) {
				syscall_errno = ENOMEM;
				return -1;
			}
			if (!copy_user_to_kern(buf, allbuf, sizeof(unsigned short) * info->info.sem_nsems)){
				syscall_errno = EFAULT;
				heapmm_free(allbuf, sizeof(unsigned short) * info->info.sem_nsems);
				return -1;
			}	
			for (n = 0; n < info->info.sem_nsems; n++)
				info->sems[n].semval = allbuf[n];
			heapmm_free(allbuf, sizeof(unsigned short) * info->info.sem_nsems);
			return 0;

		case IPC_STAT:
			if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			if (!copy_kern_to_user(&(info->info), buf, sizeof(struct semid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;
		case IPC_SET:
			if ((!ipc_is_creator(&(info->info.sem_perm))) && !ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			if (!copy_user_to_kern(buf, &ubuf, sizeof(struct semid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			info->info.sem_perm.uid = ubuf.sem_perm.uid;
			info->info.sem_perm.gid = ubuf.sem_perm.gid;
			info->info.sem_perm.mode &= ~0x1FF;
			info->info.sem_perm.mode |= ubuf.sem_perm.mode & 0x1FF;
			return 0;
		case IPC_RMID:
			if ((!ipc_is_creator(&(info->info.sem_perm))) && !ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			info->del = 1;
			if (!info->refs) {
				sem_do_delete(info);				
			} else {
				semaphore_add(info->zwaitsem, 999999);
				semaphore_add(info->nwaitsem, 999999);
			}
			return 0;
		default: 
			syscall_errno = EINVAL;
			return -1;
	}
}

void sem_do_delete(sem_info_t *info) {
	llist_unlink((llist_t *) info);
	semaphore_free(info->zwaitsem);
	semaphore_free(info->nwaitsem);
	heapmm_free(info->sems, sizeof(sysv_sem_t) * info->info.sem_nsems);
	heapmm_free(info, sizeof(sem_info_t));
}

int _sys_semget(key_t key, int nsems, int flags)
{
	int n;
	sem_info_t *info = NULL;
	if (key != IPC_PRIVATE)
		info = sem_get_by_key(key);	
	if (info && (flags & IPC_CREAT) && (flags & IPC_EXCL)) {
		syscall_errno = EEXIST;
		return -1;
	} else if (!info) {

		if ((key != IPC_PRIVATE) && !(flags & IPC_CREAT)) {
			syscall_errno = ENOENT;
			return -1;
		}

		info = heapmm_alloc(sizeof(sem_info_t));
		if (!info) {
			syscall_errno = ENOMEM;
			return -1;
		}

		/* Allocate frames */
		info->sems = heapmm_alloc(sizeof(sysv_sem_t) * nsems);
		if (!info->sems) {
			heapmm_free(info, sizeof(sem_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		info->nwaitsem = semaphore_alloc();
		if (!info->nwaitsem) {
			heapmm_free(info->sems, sizeof(sysv_sem_t) * nsems);
			heapmm_free(info, sizeof(sem_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		info->zwaitsem = semaphore_alloc();
		if (!info->zwaitsem) {
			semaphore_free(info->nwaitsem);
			heapmm_free(info->sems, sizeof(sysv_sem_t) * nsems);
			heapmm_free(info, sizeof(sem_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		/* Initialize info */
		info->id = sem_alloc_id();
		if (info->id == -1) {
			semaphore_free(info->zwaitsem);
			semaphore_free(info->nwaitsem);
			heapmm_free(info->sems, sizeof(sysv_sem_t) * nsems);
			heapmm_free(info, sizeof(sem_info_t));
			return -1;
		}

		info->del = 0;
		info->refs = 0;
		info->key = key;
		/* Initialize semid_ds */
		info->info.sem_perm.cuid = info->info.sem_perm.uid = get_effective_uid();
		info->info.sem_perm.cgid = info->info.sem_perm.gid = get_effective_gid();
		info->info.sem_perm.mode = flags & 0x1FF;
		info->info.sem_nsems = nsems;
		info->info.sem_otime = 0;
		info->info.sem_ctime = (time_t) system_time;
		for (n = 0; n < nsems; n++) {
			info->sems[n].semval = 0;
			info->sems[n].sempid = 0;
			info->sems[n].semncnt = 0;
			info->sems[n].semzcnt = 0;
		}
		llist_add_end(&sem_list, (llist_t *) info);
	} else if (!ipc_have_permissions(&(info->info.sem_perm), IPC_PERM_READ)) {
		syscall_errno = EACCES;
		return -1;
	} else if (nsems && (nsems < info->info.sem_nsems)) {
		syscall_errno = EINVAL;
		return -1;
	} else if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}	
	return info->id;
}

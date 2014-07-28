/**
 * kernel/msg.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-07-2014 - Created
 */
#include <string.h>
#include <assert.h>
#include <sys/errno.h>
#include "kernel/msg.h"
#include "kernel/ipc.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/heapmm.h"
#include "kernel/time.h"
#include "config.h"

llist_t	msg_list;
int msg_id_ctr = 0;

void msg_init()
{
	llist_create(&msg_list);
}

/**
 * Iterator to test if msg id matches the id
 */
int msg_id_search_iterator (llist_t *node, void *param)
{
	int id = (int) param;
	msg_info_t *ptr = (msg_info_t *) node;
	return ptr->id == id;
}

/**
 * Iterator to test if msg key matches the key
 */
int msg_key_search_iterator (llist_t *node, void *param)
{
	key_t key = (key_t) param;
	msg_info_t *ptr = (msg_info_t *) node;
	return ptr->key == key;
}

inline msg_info_t *msg_get_by_key(key_t key)
{
	return (msg_info_t *) llist_iterate_select(&msg_list, &msg_key_search_iterator, (void *) key);
}

inline msg_info_t *msg_get_by_id(int id)
{
	return (msg_info_t *) llist_iterate_select(&msg_list, &msg_id_search_iterator, (void *) id);
}

int msg_alloc_id()
{
	int id = msg_id_ctr++;
	if(msg_id_ctr <= 0) {
		msg_id_ctr--;
		syscall_errno = ENFILE;
		return -1;
	}

	return id;
}

int msg_recv_iterator (llist_t *node, void *param)
{
	long int rtype = (long int) param;
	sysv_msg_t *msg = (sysv_msg_t *) node;
	return	(rtype == 0) || 
		((rtype > 0) && (msg->mtype == rtype)) || 
		((rtype < 0) && (msg->mtype <= (-rtype)));
}

ssize_t _sys_msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg)
{
	size_t readsz;
	msg_info_t *info;
	sysv_msg_t *msg;
	assert(msgp != NULL);

	msgsz += sizeof(long int);

	info = msg_get_by_id(msqid);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	
	if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_READ)) {
		syscall_errno = EACCES;
		return -1;
	}

	if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}

	msg = (sysv_msg_t *) llist_iterate_select(&(info->msgs), &msg_recv_iterator, (void *) msgtyp);

	if (!msg) {
		if (msgflg & IPC_NOWAIT) {
			syscall_errno = ENOMSG;
			return -1;
		}
		info->refs++;
		while (!msg) {
			if (semaphore_idown(info->rwaitsem)) {
				info->refs--;
				if (info->del && !info->refs)
					msg_do_delete(info);
				syscall_errno = EINTR;
				return -1;
			}
			if (info->del) {
				info->refs--;
				if (!info->refs)
					msg_do_delete(info);
				syscall_errno = EIDRM;
				return -1;
			}
			msg = (sysv_msg_t *) llist_iterate_select(&(info->msgs), &msg_recv_iterator, (void *) msgtyp);
		}
	}	
	if (msg->msize > msgsz) {
		if (msgflg & MSG_NOERROR)
			readsz = msgsz;
		else {
			info->refs--;
			syscall_errno = E2BIG;
			return -1;
		}
	} else 
		readsz = msg->msize;

	llist_unlink((llist_t *) msg);
	info->info.msg_qnum--;
	info->used_bytes -= msg->msize;
	info->refs--;

	memcpy(msgp, msg->mtext, readsz);

	heapmm_free(msg->mtext, msg->msize);
	heapmm_free(msg, sizeof(sysv_msg_t));

	info->info.msg_lrpid = scheduler_current_task->pid;
	info->info.msg_rtime = (time_t) system_time;

	semaphore_up(info->swaitsem);

	return (ssize_t) (readsz - sizeof(long int));
}

int _sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
	size_t sendsz = sizeof(long int) + msgsz;
	msg_info_t *info;
	sysv_msg_t *msg;
	const long int *mtype = (const long int *) msgp;

	assert(msgp != NULL);

	info = msg_get_by_id(msqid);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	
	if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_WRITE)) {
		syscall_errno = EACCES;
		return -1;
	}
	
	if ((*mtype) < 1) {
		syscall_errno = EINVAL;
		return -1;
	}

	if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}
	if (info->used_bytes >= info->info.msg_qbytes) {
		if (msgflg & IPC_NOWAIT) {
			syscall_errno = EAGAIN;
			return -1;
		}
		info->refs++;
		while (info->used_bytes >= info->info.msg_qbytes) {
			if (semaphore_idown(info->swaitsem)) {
				info->refs--;
				if (info->del && !info->refs)
					msg_do_delete(info);
				syscall_errno = EINTR;
				return -1;
			}
			if (info->del) {
				info->refs--;
				if (!info->refs)
					msg_do_delete(info);
				syscall_errno = EIDRM;
				return -1;
			}
		}
		info->refs--;
	}
	
	msg = heapmm_alloc(sizeof(sysv_msg_t));	
	if (!msg) {
		syscall_errno = ENOMEM;
		return -1;
	}

	msg->msize = sendsz;
	msg->mtype = *mtype;
	msg->mtext = heapmm_alloc(msg->msize);
	if (!msg->mtext) {
		heapmm_free(msg, sizeof(sysv_msg_t));
		syscall_errno = ENOMEM;
		return -1;
	}
	
	memcpy(msg->mtext, msgp, sendsz);

	info->info.msg_qnum++;
	info->used_bytes += sendsz;
	info->info.msg_lspid = scheduler_current_task->pid;
	info->info.msg_stime = (time_t) system_time;
	llist_add_end(&(info->msgs), (llist_t *) msg);
	
	semaphore_up(info->rwaitsem);

	return 0;

}

int _sys_msgctl(int id, int cmd, void *buf)
{
	struct msqid_ds ubuf;
	msg_info_t *info;

	info = msg_get_by_id(id);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	 
	switch (cmd) {
		case IPC_STAT:
			if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			if (!copy_kern_to_user(&(info->info), buf, sizeof(struct msqid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;
		case IPC_SET:
			if ((!ipc_is_creator(&(info->info.msg_perm))) && !ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			if (!copy_user_to_kern(buf, &ubuf, sizeof(struct msqid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			info->info.msg_perm.uid = ubuf.msg_perm.uid;
			info->info.msg_perm.gid = ubuf.msg_perm.gid;
			info->info.msg_perm.mode &= ~0x1FF;
			info->info.msg_perm.mode |= ubuf.msg_perm.mode & 0x1FF;
			return 0;
		case IPC_RMID:
			if ((!ipc_is_creator(&(info->info.msg_perm))) && !ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_WRITE)) {
				syscall_errno = EPERM;
				return -1;
			}
			info->del = 1;
			if (!info->refs) {
				msg_do_delete(info);				
			} else {
				semaphore_add(info->rwaitsem, 999999);
				semaphore_add(info->swaitsem, 999999);
			}
			return 0;
		default: 
			syscall_errno = EINVAL;
			return -1;
	}
}

void msg_do_delete(msg_info_t *info) {
	llist_t *_m;
	sysv_msg_t *m;
	llist_unlink((llist_t *) info);
	for (_m = llist_remove_last(&(info->msgs)); _m != NULL; _m = llist_remove_last(&(info->msgs))) {
		m = (sysv_msg_t *) _m;
		heapmm_free(m->mtext, m->msize);
		heapmm_free(m, sizeof(sysv_msg_t));
	}
	semaphore_free(info->rwaitsem);
	semaphore_free(info->swaitsem);
	heapmm_free(info, sizeof(msg_info_t));
}

int _sys_msgget(key_t key, int flags)
{
	msg_info_t *info = NULL;
	if (key != IPC_PRIVATE)
		info = msg_get_by_key(key);	
	if (info && (flags & IPC_CREAT) && (flags & IPC_EXCL)) {
		syscall_errno = EEXIST;
		return -1;
	} else if (!info) {

		if ((key != IPC_PRIVATE) && !(flags & IPC_CREAT)) {
			syscall_errno = ENOENT;
			return -1;
		}

		info = heapmm_alloc(sizeof(msg_info_t));
		if (!info) {
			syscall_errno = ENOMEM;
			return -1;
		}

		info->rwaitsem = semaphore_alloc();
		if (!info->rwaitsem) {			
			heapmm_free(info, sizeof(msg_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		info->swaitsem = semaphore_alloc();
		if (!info->swaitsem) {	
			semaphore_free(info->rwaitsem);		
			heapmm_free(info, sizeof(msg_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		/* Initialize info */
		info->id = msg_alloc_id();
		if (info->id == -1) {
			//semaphore_free(info->zwaitsem);
			//semaphore_free(info->nwaitsem);
			//heapmm_free(info->sems, sizeof(sysv_msg_t) * nsems);
			heapmm_free(info, sizeof(msg_info_t));
			return -1;
		}

		info->del = 0;
		info->refs = 0;
		info->key = key;
		info->used_bytes = 0;
		llist_create(&(info->msgs));

		/* Initialize semid_ds */
		info->info.msg_perm.cuid = info->info.msg_perm.uid = get_effective_uid();
		info->info.msg_perm.cgid = info->info.msg_perm.gid = get_effective_gid();
		info->info.msg_perm.mode = flags & 0x1FF;
		info->info.msg_qnum = 0;
		info->info.msg_lspid = 0;
		info->info.msg_lrpid = 0;
		info->info.msg_stime = 0;
		info->info.msg_rtime = 0;
		info->info.msg_ctime = (time_t) system_time;
		info->info.msg_qbytes = CONFIG_SYSV_MSG_SIZE_LIMIT;
		
		llist_add_end(&msg_list, (llist_t *) info);
	} else if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_READ)) {
		syscall_errno = EACCES;
		return -1;
	} else if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}	
	return info->id;
}

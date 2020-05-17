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

/**
 * Gets a message queue by its key
 * @return The message queue info block, or NULL if not found
 */
static inline msg_info_t *msg_get_by_key(key_t key)
{
	return (msg_info_t *) 
		llist_iterate_select(&msg_list, &msg_key_search_iterator, (void *) key);
}

/**
 * Gets a message queue by its identifier
 * @return The message queue info block, or NULL if not found
 */
static inline msg_info_t *msg_get_by_id(int id)
{
	return (msg_info_t *)
		llist_iterate_select(&msg_list, &msg_id_search_iterator, (void *) id);
}

/**
 * Allocate message queue id
 * @return A message queue ID or MSG_BAD_ID
 */
int msg_alloc_id()
{
	int id = msg_id_ctr++;
	
	/* If the message queue ID counter wrapped around we cannot allocate 
	 * any more MQs */
	if(msg_id_ctr <= 0) {
		msg_id_ctr--;
		syscall_errno = ENFILE;
		return MSG_BAD_ID;
	}

	return id;
}

/* Find any messages with a type that matches the rtype */
int msg_recv_iterator (llist_t *node, void *param)
{
	long int rtype = (long int) param;
	sysv_msg_t *msg = (sysv_msg_t *) node;
	
	return	(rtype == 0) || 
		((rtype > 0) && (msg->mtype == rtype)) || 
		((rtype < 0) && (msg->mtype <= (-rtype)));
}

ssize_t _sys_msgrcv(
	int msqid, 
	void *msgp, 
	size_t msgsz, 
	long int msgtyp, 
	int msgflg )
{
	size_t readsz;
	msg_info_t *info;
	sysv_msg_t *msg;
	assert(msgp != NULL);

	msgsz += sizeof(long int);

	/* Resolve the message queue */
	info = msg_get_by_id(msqid);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	
	/* Check if we may access the MQ... */
	if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_READ)) {
		syscall_errno = EACCES;
		return -1;
	}

	/* ...and if it hasn't been deleted */
	if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}

	/* Try to find a message eligible for msgrcv */
	msg = (sysv_msg_t *)
		llist_iterate_select(
			&(info->msgs), 
			&msg_recv_iterator, 
			(void *) msgtyp);

	/* If none was found */
	if (!msg) {
		/* and we are not to wait, return ENOMSG error */
		if (msgflg & IPC_NOWAIT) {
			syscall_errno = ENOMSG;
			return -1;
		}
		
		/* Otherwise, wait until a message comes in. */
		
		/* Because we are going to be yielding control, we need to mark 
		 * the MQ as referenced so other processes won't delete it and leave
		 * us with a stale pointer. */
		info->refs++;
		
		/* Loop until we receive a message or an error condition */
		while (!msg) {
		
			/* Wait on the receive semaphore */
			if ( semaphore_idown(info->rwaitsem) != SCHED_WAIT_OK ) {
				/* We were interrupted before receiving a message */
				
				/* Release the reference to the MQ */
				info->refs--;
				
				/* If we had the last ref and the MQ was marked for deletion,
				 * perform the actual delete operation */
				if (info->del && !info->refs) //TODO: Consolidate into func?
					msg_do_delete(info);
				
				/* Finally, report EINTR to the caller */
				syscall_errno = EINTR;
				return -1;
			}
			
			/* We got notified on the semaphore */
			
			/* Check whether it was because someone deleted the queue */
			if (info->del) {
				/* It was. */
				
				/* First, get rid of our reference to the MQ */
				info->refs--;
				if (!info->refs)
					msg_do_delete(info); //TODO: Consolidate into func?
					
				/* The MQ being deleted during msgrcv is an error, report it */
				syscall_errno = EIDRM;
				return -1;
			}
			
			/* Try to find a message that matches the mtype */ 
			msg = (sysv_msg_t *) llist_iterate_select(
				&(info->msgs), 
				&msg_recv_iterator, 
				(void *) msgtyp);
				
		}
	}	
	
	/* Check if the message would fit the output buffer */
	if (msg->msize > msgsz) {
	
		/* It does not fit, check if the caller accepts errors for this */
		if (msgflg & MSG_NOERROR) {
		
			/* they do not, truncate the data and proceed */
			readsz = msgsz;
			
		} else {
		
			/* they do, clean up and report the error */			
			info->refs--;
			
			syscall_errno = E2BIG;
			return -1;
			
		}
	} else { 
		/* It fits, read the whole message */
		readsz = msg->msize;
	}
	
	/* Remove the message from the queue's message list */
	llist_unlink((llist_t *) msg);
	
	/* Update the message and data counters on the queue */
	info->info.msg_qnum--;
	info->used_bytes -= msg->msize;
	
	/* Release the queue reference */
	info->refs--;

	/* Copy over the data to the caller */
	memcpy( msgp, msg->mtext, readsz );

	/* Release the memory for the message info block */
	heapmm_free(msg->mtext, msg->msize);
	heapmm_free(msg, sizeof(sysv_msg_t));

	/* Set MQ last receive information */
	info->info.msg_lrpid = current_process->pid;
	info->info.msg_rtime = (time_t) system_time;

	/* Bump the send wait semaphore.
	 * This will wake up any processes that blocked becaue the MQ was full,
	 * the count of this semaphore does not really matter, as the sender will
	 * check for availability before proceeding. If the count is too high it 
	 * will simply be decremented until there is actually room to store another
	 * message */
	semaphore_up(info->swaitsem);

	/* Return the number of bytes read, minus the message id. */
	return (ssize_t) (readsz - sizeof(long int));
}

int _sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
	size_t sendsz;
	msg_info_t *info;
	sysv_msg_t *msg;
	const long int *mtype;
	
	/* The first long int in the message buffer is the message type */
	mtype = (const long int *) msgp;
	
	/* the mtype is not included in the msgsz count, so add it here */
	sendsz = sizeof(long int) + msgsz;

	assert(msgp != NULL);

	/* Resolve the MQ info block */
	info = msg_get_by_id(msqid);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	
	/* Check if we may use it */
	if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_WRITE)) {
		syscall_errno = EACCES;
		return -1;
	}
	
	/* Check if the message type is valid, 0 and negative have special 
	 * meanings for msgrcv so cannot be used as an actual mtype. */
	if ((*mtype) < 1) {
		syscall_errno = EINVAL;
		return -1;
	}

	/* Check whether the MQ was deleted */	 
	if (info->del) {
		syscall_errno = EIDRM;
		return -1;
	}
	
	/* Check whether the MQ has room for a message */
	if (info->used_bytes >= info->info.msg_qbytes) {
		/* It does not, check if we are supposed to wait for room to appear */
		if (msgflg & IPC_NOWAIT) {
			/* we are not, report the error and stop */
			syscall_errno = EAGAIN;
			return -1;
		}
		
		/* Because we are going to be yielding control, we need to mark 
		 * the MQ as referenced so other processes won't delete it and leave
		 * us with a stale pointer. */
		info->refs++;
		
		/* Loop until space has become available */
		while (info->used_bytes >= info->info.msg_qbytes) {
		
			/* Wait on the send semaphore, which is raised whenever a message
			 * is received from the queue. This may be non-zero even if there
			 * is no room, hence the loop around this part of the logic. */
			if ( semaphore_idown(info->swaitsem) != SCHED_WAIT_OK ) {
				/* The wait was interrupted, return the error to the caller */
				
				/* Release the reference to the MQ */
				info->refs--;
				
				/* If we had the last ref and the MQ was marked for deletion,
				 * perform the actual delete operation */
				if (info->del && !info->refs) //TODO: Consolidate into func?
					msg_do_delete(info);
					
				/* Report the error */
				syscall_errno = EINTR;
				return -1;
				
			}
			
			/* We got notified on the semaphore */
			
			/* Check whether it was because someone deleted the queue */
			if (info->del) {
				/* It was. */
				
				/* First, get rid of our reference to the MQ */
				info->refs--;
				if (!info->refs)
					msg_do_delete(info); //TODO: Consolidate into func?
					
				/* The MQ being deleted during msgrcv is an error, report it */
				syscall_errno = EIDRM;
				return -1;
			}

		}
		
		/* Space has become available in the MQ, release the reference to it */
		info->refs--;
	}
	/* There is space available in the MQ */
	
	/* Allocate a header for the message */
	msg = heapmm_alloc(sizeof(sysv_msg_t));	
	if (!msg) {
		syscall_errno = ENOMEM;
		return -1;
	}

	/* Fill the message metadata */
	msg->msize = sendsz;
	msg->mtype = *mtype;
	
	/* Allocate a buffer for the message body */
	msg->mtext = heapmm_alloc(msg->msize);
	if (!msg->mtext) {
		/* Allocation failed, but we already got memory for the header */
		/* Free the header and report the error */
		heapmm_free(msg, sizeof(sysv_msg_t));
		syscall_errno = ENOMEM;
		return -1;
	}
	
	/* Copy the message body */
	memcpy(msg->mtext, msgp, sendsz); 
	//TODO: Check if this should be copy_kern_to_user

	/* Update the data and message counters for the MQ */
	info->info.msg_qnum++;
	info->used_bytes += sendsz;
	
	/* Update the last sender info for the MQ */
	info->info.msg_lspid = current_process->pid;
	info->info.msg_stime = (time_t) system_time;
	
	/* Enqueue the message */
	llist_add_end(&(info->msgs), (llist_t *) msg);
	
	/* Wake up any processes blocking on the queue */
	semaphore_up(info->rwaitsem);

	return 0;

}

int _sys_msgctl(int id, int cmd, void *buf)
{
	struct msqid_ds ubuf;
	msg_info_t *info;

	/* Resolve the message queue */
	info = msg_get_by_id(id);
	if (!info) {
		syscall_errno = EINVAL;
		return -1;
	}
	
	/* Handle the various commands exposed through this syscall */
	switch (cmd) {
		/* IPC_STAT queries the status of a MQ */
		case IPC_STAT:
			/* Check whether we are permitted to read the MQ info */
			if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_READ)) {
				syscall_errno = EACCES;
				return -1;
			}
			
			/* The info structure is matched to the result for IPC_STAT, so
			 * a simple copy to userspace is sufficient. */
			if (!copy_kern_to_user(
					&(info->info), 
					buf, 
					sizeof(struct msqid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			
			/* Report succesful completion */
			return 0;
			
		/* IPC_SET allows setting the ownership of a MQ */
		case IPC_SET:
			/* Check if we are permitted to set the ownership and flags,
			 * this is the case either when the process is the owner, creator
			 * or, if the process is privileged */
			if ( 
				(!ipc_is_creator(&(info->info.msg_perm))) && 
				 !ipc_is_owner(&(info->info.msg_perm)) && 
				 !ipc_is_privileged() ) {
				syscall_errno = EPERM;
				return -1;
			}
			
			/* Copy over the input buffer from the process to the kernel */
			if (!copy_user_to_kern(buf, &ubuf, sizeof(struct msqid_ds))){
				syscall_errno = EFAULT;
				return -1;
			}
			
			/* Copy the values into the structure */
			info->info.msg_perm.uid = ubuf.msg_perm.uid;
			info->info.msg_perm.gid = ubuf.msg_perm.gid;
			info->info.msg_perm.mode &= ~0x1FF;
			info->info.msg_perm.mode |= ubuf.msg_perm.mode & 0x1FF;
			
			/* Report succesful completion */
			return 0;
		case IPC_RMID:
			/* Check if we are permitted to delete the MQ,
			 * this is the case either when the process is the owner, creator
			 * or, if the process is privileged */
			if ( 
				(!ipc_is_creator(&(info->info.msg_perm))) && 
				 !ipc_is_owner(&(info->info.msg_perm)) && 
				 !ipc_is_privileged() ) {
				syscall_errno = EPERM;
				return -1;
			}
			
			/* Mark the MQ as deleted. 
			 * Because other processes might still hold references to the MQ we
			 * need to check the reference count. */
			info->del = 1;
			
			if (!info->refs) {		
				/* If no other processes hold references, delete the MQ now */
				msg_do_delete(info);				
			} else {
				/* If other processes do hold a reference, we wake them by
				 * providing a near infinite amount of counts to the send and
				 * receive semaphores. The last process to hold a reference 
				 * deletes the MQ. */
				semaphore_add(info->rwaitsem, 999999);
				semaphore_add(info->swaitsem, 999999);
			}
			
			/* Report succesful completion */
			return 0;
		default: 
			syscall_errno = EINVAL;
			return -1;
	}
}

void msg_do_delete(msg_info_t *info) {
	llist_t *_m;
	sysv_msg_t *m;
	
	/* Remove the MQ from the global MQ table */
	llist_unlink((llist_t *) info);
	
	/* Iterate over all messages and delete them */
	for ( _m = llist_remove_last(&(info->msgs));
		  _m != NULL; 
		  _m = llist_remove_last(&(info->msgs))) {
		m = (sysv_msg_t *) _m;
		
		/* Free the associated memory */
		heapmm_free(m->mtext, m->msize);
		heapmm_free(m, sizeof(sysv_msg_t));
	}
	
	/* Free the semaphores and MQ metadata */
	semaphore_free(info->rwaitsem);
	semaphore_free(info->swaitsem);
	heapmm_free(info, sizeof(msg_info_t));
}

int _sys_msgget(key_t key, int flags)
{
	msg_info_t *info = NULL;
	
	/* If the key is not IPC_PRIVATE, we are trying to either create or query
	 * an MQ by key. If we are creating one by name we need to know whether one
	 * exists by that key, if we are querying we need the MQ. Both are solved
	 * by looking up the MQ. */
	if ( key != IPC_PRIVATE )
		info = msg_get_by_key(key);	
	
	/* If we are to create the MQ and the IPC_EXCL flag is set, we are not 
	 * allowed to use an existing MQ. If it does exist, signal EEXIST */
	if (info && (flags & IPC_CREAT) && (flags & IPC_EXCL)) {
		syscall_errno = EEXIST;
		return -1;
	} else if (!info) {
	
		/* If we did not find a MQ and we are not supposed to create a MQ,
		 * we need to signal ENOENT. If key is IPC_PRIVATE, we are always
		 * supposed to create the MQ */
		if ((key != IPC_PRIVATE) && !(flags & IPC_CREAT)) {
			syscall_errno = ENOENT;
			return -1;
		}

		/* Try to allocate the MQ info block,... */
		info = heapmm_alloc(sizeof(msg_info_t));
		if (!info) {
			syscall_errno = ENOMEM;
			return -1;
		}
		
		/* ...the receive semaphore,... */
		info->rwaitsem = semaphore_alloc();
		if (!info->rwaitsem) {			
			heapmm_free(info, sizeof(msg_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		/* ...and, the sender semaphore. */
		info->swaitsem = semaphore_alloc();
		if (!info->swaitsem) {	
			semaphore_free(info->rwaitsem);		
			heapmm_free(info, sizeof(msg_info_t));
			syscall_errno = ENOMEM;
			return -1;
		}

		/* Try to allocate a MQ ID */
		info->id = msg_alloc_id();
		if (info->id == -1) {
			semaphore_free(info->rwaitsem);
			semaphore_free(info->swaitsem);
			heapmm_free(info, sizeof(msg_info_t));
			return -1;
		}

		/* Initialize the MQ fields */
		info->del = 0;
		info->refs = 0;
		info->key = key;
		info->used_bytes = 0;
		llist_create(&(info->msgs));

		/* Initialize the permission */
		info->info.msg_perm.cuid = 
			info->info.msg_perm.uid = get_effective_uid();
		info->info.msg_perm.cgid = 
			info->info.msg_perm.gid = get_effective_gid();
		info->info.msg_perm.mode = flags & 0x1FF;
		info->info.msg_qnum   = 0;
		info->info.msg_lspid  = 0;
		info->info.msg_lrpid  = 0;
		info->info.msg_stime  = 0;
		info->info.msg_rtime  = 0;
		info->info.msg_ctime  = (time_t) system_time;
		info->info.msg_qbytes = CONFIG_SYSV_MSG_SIZE_LIMIT;
		
		/* Add the MQ to the MQ list */
		llist_add_end(&msg_list, (llist_t *) info);
		
	} else if (!ipc_have_permissions(&(info->info.msg_perm), IPC_PERM_OPER) ) {
		/* The MQ was found, but we did not have OPER permission on it, that 
		 * means we signal EACCES */
		syscall_errno = EACCES;
		return -1;
	} else if (info->del) {
		/* The MQ was found but was already marked for deletion, signal EIDRM */
		syscall_errno = EIDRM;
		return -1;
	}
	
	/* Return the MQ ID */
	return info->id;
}

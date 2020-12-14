/**
 * kernel/msg.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-07-2014 - Created
 */

#ifndef __KERNEL_MSG_H__
#define __KERNEL_MSG_H__

#include <stdint.h>
#include <sys/types.h>
#include <sys/msg.h>
#include "kernel/synch.h"
#include "util/llist.h"

#define MSG_BAD_ID	(-1)

typedef struct {
	llist_t		 link;
	long int	 mtype;
	size_t		 msize;
	void		*mtext;
} sysv_msg_t;

typedef struct {
	llist_t      link;
	key_t        key;
	int          id;
	int          del;
	int          refs;
	semaphore_t  swaitsem;
	semaphore_t  rwaitsem;
	struct msqid_ds	 info;
	llist_t      msgs;
	msglen_t     used_bytes;
} msg_info_t;

void msg_init(void);
void msg_do_delete(msg_info_t *info);

ssize_t _sys_msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg);
int _sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
int _sys_msgctl(int id, int cmd, void *buf);
int _sys_msgget(key_t key, int flags);

#endif

/**
 * sys/msg.h
 *
 * Part of P-OS.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-07-2014 - Created
 */

#ifndef __SYS_MSG_H__
#define __SYS_MSG_H__

#include <sys/types.h>
#include <sys/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_NOERROR	(1<<0)

typedef unsigned long	msgqnum_t;
typedef size_t		msglen_t;

struct msqid_ds {
	struct ipc_perm	msg_perm;
	msgqnum_t	msg_qnum;
	msglen_t	msg_qbytes;
	pid_t		msg_lspid;
	pid_t		msg_lrpid;
	time_t		msg_stime;
	time_t		msg_rtime;
	time_t		msg_ctime;
};

int       msgctl(int, int, struct msqid_ds *);
int       msgget(key_t, int);
ssize_t   msgrcv(int, void *, size_t, long int, int);
int       msgsnd(int, const void *, size_t, int);


#ifdef __cplusplus
}
#endif

#endif


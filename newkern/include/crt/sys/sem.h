/**
 * sys/sem.h
 *
 * Part of P-OS.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 24-07-2014 - Created
 */

#ifndef __SYS_SEM_H__
#define __SYS_SEM_H__

#include <sys/types.h>
#include <sys/ipc.h>

#define SEM_UNDO	(1<<0)

#define GETNCNT		(0)
#define GETPID		(1)
#define GETVAL		(2)
#define GETALL		(3)
#define GETZCNT		(4)
#define SETVAL		(5)
#define SETALL		(6)

struct semid_ds {
	struct ipc_perm	sem_perm;
	unsigned short	sem_nsems;
	time_t		sem_otime;
	time_t		sem_ctime;
};

struct sembuf {
	short		sem_num;
	short		sem_op;
	short		sem_flg;
};

int	 semctl(int semid, int semnum, int cmd, void *buf);
int	 semget(key_t key, int nsems, int semflg);
int	 semop(int, struct sembuf *, size_t);

#endif

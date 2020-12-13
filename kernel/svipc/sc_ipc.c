/**
 * kernel/sc_ipc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-07-2014 - Created
 */

#include <assert.h>
#include <stdint.h>
#include <sys/errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "kernel/shm.h"
#include "kernel/sem.h"
#include "kernel/msg.h"
#include "kernel/ipc.h"
#include "kernel/heapmm.h"
#include "kernel/syscall.h"

//void *shmat(int shmid, void *shmaddr, int shmflg);
SYSCALL_DEF3(shmat)
{
	return (uint32_t) _sys_shmat((int) a, (void *) b, (int) c);
}

//int shmdt(void *shmaddr);
SYSCALL_DEF1(shmdt)
{
	return (uint32_t) _sys_shmdt((void *) a);
}

//int shmctl(int id, int cmd, struct shmid_ds *buf);
SYSCALL_DEF3(shmctl)
{
	return (uint32_t) _sys_shmctl((int) a, (int) b, (struct shmid_ds *) c);
}

//int shmget(key_t key, size_t size, int flags);
SYSCALL_DEF3(shmget)
{
	return (uint32_t) _sys_shmget((key_t) a, (size_t) b, (int) c);
}

//int semop(int semid, struct sembuf *sops, size_t nsops);
SYSCALL_DEF3(semop)
{
	struct sembuf * buf;
	int status;
	buf = heapmm_alloc(c * sizeof(struct sembuf));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *) b, buf, c * sizeof(struct sembuf))) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c * sizeof(struct sembuf));
		return (uint32_t) -1;
	}
	status = _sys_semop((int) a, buf, (size_t) c);
	heapmm_free(buf, c * sizeof(struct sembuf));
	return (uint32_t) status;
}


//int semctl(int id, int semnum, int cmd, void *buf);
SYSCALL_DEF4(semctl)
{
	return (uint32_t) _sys_semctl((int) a, (int) b, (int) c, (void *) d);
}

//int semget(key_t key, int nsems, int flags);
SYSCALL_DEF3(semget)
{
	return (uint32_t) _sys_semget((key_t) a, (int) b, (int) c);
}

//ssize_t   msgrcv(int, void *, size_t, long int, int);
SYSCALL_DEF5(msgrcv)
{
	void *buf;
	ssize_t status;
	buf = heapmm_alloc(c + sizeof(long int));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_msgrcv((int) a, buf, (size_t) c, (long int) d, (int) e);
	if ((status != -1) && !copy_kern_to_user(buf, (void *) b, status + sizeof(long int)) ) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c + sizeof(long int));
		return (uint32_t) -1;
	}
	heapmm_free(buf, c  + sizeof(long int));
	return (uint32_t) status;
}

//int       msgsnd(int, const void *, size_t, int);
SYSCALL_DEF4(msgsnd)
{
	void *buf;
	int status;
	buf = heapmm_alloc(c + sizeof(long int));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *) b, buf, c + sizeof(long int))) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c  + sizeof(long int));
		return (uint32_t) -1;
	}
	status = _sys_msgsnd((int) a, buf, (size_t) c, (int) d);
	heapmm_free(buf, c  + sizeof(long int));
	return (uint32_t) status;
}

//int       msgctl(int, int, struct msqid_ds *);
SYSCALL_DEF3(msgctl)
{
	return (uint32_t) _sys_msgctl((int) a, (int) b, (void *) c);
}

//int       msgget(key_t, int);
SYSCALL_DEF2(msgget)
{
	return (uint32_t) _sys_msgget((key_t) a, (int) b);
}



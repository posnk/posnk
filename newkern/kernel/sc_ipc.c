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
#include "kernel/ipc.h"
#include "kernel/heapmm.h"
#include "kernel/syscall.h"

//void *shmat(int shmid, void *shmaddr, int shmflg);
uint32_t sys_shmat(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_shmat((int) param[0], (void *) param[1], (int) param[2]);
}

//int shmdt(void *shmaddr);
uint32_t sys_shmdt(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_shmdt((void *) param[0]);
}

//int shmctl(int id, int cmd, struct shmid_ds *buf);
uint32_t sys_shmctl(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_shmctl((int) param[0], (int) param[1], (struct shmid_ds *) param[2]);
}

//int shmget(key_t key, size_t size, int flags);
uint32_t sys_shmget(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_shmget((key_t) param[0], (size_t) param[1], (int) param[2]);
}

//int semop(int semid, struct sembuf *sops, size_t nsops);
uint32_t sys_semop(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	struct sembuf * buf;
	int status;
	buf = heapmm_alloc(param[2] * sizeof(struct sembuf));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *) param[1], buf, param[2] * sizeof(struct sembuf))) {
		syscall_errno = EFAULT;
		heapmm_free(buf, param[2] * sizeof(struct sembuf));
		return (uint32_t) -1;
	}
	status = _sys_semop((int) param[0], buf, (size_t) param[2]);
	heapmm_free(buf, param[2] * sizeof(struct sembuf));
	return (uint32_t) status;
}


//int semctl(int id, int semnum, int cmd, void *buf);
uint32_t sys_semctl(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_semctl((int) param[0], (int) param[1], (int) param[2], (void *) param[3]);
}

//int semget(key_t key, int nsems, int flags);
uint32_t sys_semget(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_semget((key_t) param[0], (int) param[1], (int) param[2]);
}

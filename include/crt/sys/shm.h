/**
 * sys/shm.h
 *
 * Part of P-OS.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-07-2014 - Created
 */

#ifndef __SYS_SHM_H__
#define __SYS_SHM_H__

#include <sys/types.h>
#include <sys/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHM_RDONLY	(1<<0)
#define SHM_RND		(1<<1)

typedef unsigned short	shmatt_t;

struct shmid_ds {
	struct ipc_perm shm_perm;
	size_t          shm_segsz;
	pid_t           shm_lpid;
	pid_t           shm_cpid;
	shmatt_t        shm_nattch;
	time_t          shm_atime;
	time_t          shm_dtime;
	time_t          shm_ctime;
};


void	*shmat(int shmid, const void *shmaddr, int shmflg);
int	 shmctl(int shmid, int cmd, struct shmid_ds *buf);
int	 shmdt(const void *shmaddr);
int	 shmget(key_t key, size_t size, int shmflg);

#ifdef __cplusplus
}
#endif

#endif

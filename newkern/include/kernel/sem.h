/**
 * kernel/sem.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */

#ifndef __KERNEL_SEM_H__
#define __KERNEL_SEM_H__

#include "util/llist.h"
#include <stdint.h>
#include <sys/types.h>
#include <sys/sem.h>

typedef struct {
	unsigned short	 semval;
	pid_t		 sempid;
	unsigned short	 semncnt;
	unsigned short	 semzcnt;
} sysv_sem_t;

typedef struct {
	llist_t		 link;
	key_t		 key;
	int		 id;
	int		 del;
	int		 refs;
	struct semid_ds	 info;
	sysv_sem_t	*sems;
} sem_info_t;

int _sys_semctl(int id, int cmd, void *arg);

void sem_do_delete(sem_info_t *info);

int _sys_semget(key_t key, int nsems, int flags);

#endif

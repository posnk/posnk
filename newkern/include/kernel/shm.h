/**
 * kernel/shm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */

#ifndef __KERNEL_SHM_H__
#define __KERNEL_SHM_H__

#include "util/llist.h"
#include <stdint.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "kernel/physmm.h"

typedef struct {
	llist_t		 link;
	key_t		 key;
	int		 id;
	int		 del;
	int		 zero;
	struct shmid_ds	 info;
	physaddr_t	*frames;
} shm_info_t;


void shm_do_delete(shm_info_t *info);

#endif

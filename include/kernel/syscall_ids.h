/**
 * kernel/syscall_ids.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-04-2014 - Created
 */

#ifndef __KERNEL_SYSCALL_IDS_H__
#define __KERNEL_SYSCALL_IDS_H__

#include <stdint.h>

#define SYSCALL_MAGIC	(0xCAFECA11)

#define syscall_errno	(scheduler_current_task->sc_errno)

struct syscall_params {
	volatile uint32_t	magic;
	volatile uint32_t	call_id;
	volatile uint32_t	param[4];
	volatile uint32_t	param_size[4];
	volatile uint32_t	return_val;
	volatile uint32_t	sc_errno;
};

typedef struct syscall_params syscall_params_t;

#include <sys/syscall.h>

uint32_t nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4]);
#endif

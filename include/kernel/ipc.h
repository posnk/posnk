/**
 * kernel/ipc.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */

#ifndef __KERNEL_IPC_H__
#define __KERNEL_IPC_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include "kernel/permissions.h"

#define IPC_PERM_READ	4
#define IPC_PERM_WRITE	2

perm_class_t ipc_get_min_permissions(const struct ipc_perm *ipc, mode_t req_mode);

int ipc_have_permissions(const struct ipc_perm *ipc, mode_t req_mode);

int ipc_is_creator(const struct ipc_perm *ipc);

void ipc_init( void );
#endif

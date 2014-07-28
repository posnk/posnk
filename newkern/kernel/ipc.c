/**
 * kernel/ipc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-07-2014 - Created
 */
#include "kernel/shm.h"
#include "kernel/sem.h"
#include "kernel/msg.h"
#include <sys/ipc.h>
#include <assert.h>
#include "kernel/permissions.h"


/**
 * @brief Returns the minimum privilege level required for access with mode
 * @param ipc IPC permission object to check
 * @param req_mode The requested access mode
 * @return The required privilege class : other, group or owner
 */

perm_class_t ipc_get_min_permissions(struct ipc_perm *ipc, mode_t req_mode)
{
	assert (ipc != NULL);
	if (req_mode & ((ipc->mode) & 7))
		return PERM_CLASS_OTHER;
	if (req_mode & ((ipc->mode >> 3) & 7))
		return PERM_CLASS_GROUP;
	if (req_mode & ((ipc->mode >> 6) & 7))
		return PERM_CLASS_OWNER;
	return PERM_CLASS_NONE;
}

/**
 * @brief Check permissions for access with mode
 * @param ipc IPC permission object to check
 * @param req_mode The requested access mode
 * @return Whether the requested access is allowed
 */

int ipc_have_permissions(struct ipc_perm *ipc, mode_t req_mode) {
	assert (ipc != NULL);
	return get_perm_class(ipc->uid, ipc->gid) <= ipc_get_min_permissions(ipc, req_mode);
}

int ipc_is_creator(struct ipc_perm *ipc) {
	assert (ipc != NULL);
	return get_perm_class(ipc->cuid, ipc->cgid) == PERM_CLASS_OWNER;
}

void ipc_init()
{
	shm_init();
	sem_init();
	msg_init();
}

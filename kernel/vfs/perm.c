/**
 * @file kernel/vfs/perm.c
 *
 * Implements path resolution
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 06-02-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include <string.h>

#include "util/llist.h"

#include "kernel/permissions.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

/* Internal type definitions */

/* Public Functions */

/**
 * @brief Returns the minimum privilege level required for access with mode
 * @param inode The file to check
 * @param req_mode The requested access mode
 * @return The required privilege class : other, group or owner
 */

perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode)
{
	assert (inode != NULL);
	if (req_mode & ((inode->mode) & 7))
		return PERM_CLASS_OTHER;
	if (req_mode & ((inode->mode >> 3) & 7))
		return PERM_CLASS_GROUP;
	if (req_mode & ((inode->mode >> 6) & 7))
		return PERM_CLASS_OWNER;
	return PERM_CLASS_NONE;
}

/**
 * @brief Check permissions for access with mode
 * @param inode The file to check
 * @param req_mode The requested access mode
 * @return Whether the requested access is allowed
 */

int vfs_have_permissions(inode_t *inode, mode_t req_mode) {
	assert (inode != NULL);
	return get_perm_class(inode->uid, inode->gid) <= vfs_get_min_permissions(inode, req_mode);
}

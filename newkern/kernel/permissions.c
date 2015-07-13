/**
 * kernel/permissions.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-04-2014 - Created
 */
 
#include "kernel/permissions.h"
#include "kernel/scheduler.h"

uid_t get_uid()
{
	return scheduler_current_task->uid;
}

uid_t get_effective_uid()
{
	return scheduler_current_task->effective_uid;
}

gid_t get_gid()
{
	return scheduler_current_task->uid;
}

gid_t get_effective_gid()
{
	return scheduler_current_task->effective_gid;
}

int is_superuser() 
{
	uid_t r_uid = get_uid();
	gid_t r_gid = get_gid();
	
	return ((uid == 0) && (gid == 0));	
}

perm_class_t get_perm_class(uid_t resource_uid, gid_t resource_gid)
{
	uid_t uid = get_effective_uid();
	gid_t gid = get_effective_gid();
	uid_t r_uid = get_uid();
	gid_t r_gid = get_gid();
	/* Are we root? */
	if ((uid == 0) && (gid == 0))
		return PERM_CLASS_OWNER; /* Root is almighty */
	/* Nope, perform checks */
	if ((resource_uid == uid) || (resource_uid == r_uid))
		return PERM_CLASS_OWNER;
	else if ((resource_gid == gid) || (resource_uid == r_gid))
		return PERM_CLASS_GROUP;
	else
		return PERM_CLASS_OTHER;
}


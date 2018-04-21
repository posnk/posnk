/**
 * kernel/permissions.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-04-2014 - Created
 */

#include "kernel/process.h"


#ifndef __KERNEL_PERMISSIONS_H__
#define __KERNEL_PERMISSIONS_H__

#define PERM_CLASS_NONE  0
#define PERM_CLASS_OWNER 1
#define PERM_CLASS_GROUP 2
#define PERM_CLASS_OTHER 3

typedef int perm_class_t;


uid_t get_uid(void);

uid_t get_effective_uid(void);

gid_t get_gid(void);

gid_t get_effective_gid(void);

perm_class_t get_perm_class(uid_t resource_uid, gid_t resource_gid);

#endif

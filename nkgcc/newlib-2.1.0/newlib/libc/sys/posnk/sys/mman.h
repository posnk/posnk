/**
 * sys/mman.h
 *
 * Part of P-OS.
 *
 * Except where otherwise specified, this file is POSIX compliant.
 * 
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-07-2014 - Created
 */

#ifndef __SYS_MMAN_H__
#define __SYS_MMAN_H__

#include <sys/types.h>

#define PROT_READ	(1<<0)
#define PROT_WRITE	(1<<1)
#define PROT_EXEC	(1<<2)
#define PROT_NONE	(0)

#define MAP_SHARED	(1<<0)
#define MAP_PRIVATE	(0)
#define MAP_FIXED	(1<<2)
#define MAP_FAILED	((void *)0)

#endif

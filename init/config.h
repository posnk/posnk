/* 
 * config.h
 *
 * Part of P-OS init
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define INITTAB_PATH "/etc/inittab"
#define SINGLEUSER_CMD "/busybox ash\n"

#define RC_INTERPRETER_NAME "ash"
#define RC_INTERPRETER_PATH "/busybox"

#define RC_SCRIPT_PATH "/etc/rc"

#endif

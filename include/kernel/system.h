/**
 * kernel/system.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-04-2014 - Created
 */
 
#ifndef __KERNEL_SYSTEM_H__
#define __KERNEL_SYSTEM_H__

#include "config.h"
#include <sys/types.h>
extern char kernel_cmdline [ CONFIG_CMDLINE_MAX_LENGTH ];
extern char init_path      [ CONFIG_FILE_MAX_NAME_LENGTH ];
extern char console_path   [ CONFIG_FILE_MAX_NAME_LENGTH ];
extern char root_fs        [ 32 ];
extern dev_t root_dev;
int cmdline_parse();

void halt();
 
void shutdown();

 #endif

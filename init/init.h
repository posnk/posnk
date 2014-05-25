/* 
 * init.h
 *
 * Part of P-OS init
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#ifndef __INIT_H__
#define __INIT_H__

#include <sys/types.h>
#include "config.h"

int execute_rc_scripts();

void start_ttys();

void setup_singleuser_mode();

void parse_args(char *buffer, char** args, 
                size_t args_size, size_t *nargs);

void start_tty(int id);

int find_tty(pid_t pid);

void read_inittab();

#endif

/* 
 * getty.h
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#ifndef __GETTY_H__
#define __GETTY_H__

#include "config.h"

int tty_open(char *path);

void print_issue(char *tty);

#endif

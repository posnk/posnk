/**
 * kernel/exception.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-04-2014 - Created
 */

#ifndef __KERNEL_EXCEPTION_H__
#define __KERNEL_EXCEPTION_H__

#include <stddef.h>
#include <signal.h>

void exception_handle( int sig, struct siginfo info, void *instr );

#endif

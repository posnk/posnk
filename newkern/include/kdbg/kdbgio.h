/* 
 * kdbg/kdbgio.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#ifndef __KDBG_KDBGIO_H__
#define __KDBG_KDBGIO_H__

#include "kernel/earlycon.h"
#include <stdint.h>

char *kdbg_gets(char *buf, size_t size);
uintptr_t kdbg_parsehex(char *str);
#define kdbg_printf debugcon_aprintf

#endif

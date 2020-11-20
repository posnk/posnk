/**
 * kernel/earlycon.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 31-03-2014 - Created
 */

#ifndef __KERNEL_EARLYCON_H__
#define __KERNEL_EARLYCON_H__

#include <stddef.h>
#include <stdarg.h>
#include "kernel/console.h"
void earlycon_init();

void debugcon_init();

void debugcon_putc(char c);

int debugcon_aprintf(const char* str,...);

int panic_printf(const char* str,...);

void debugcon_aputs(const char *str);

int debugcon_have_data();

void panicscreen(const char *text);
void earlycon_switchover();

#define earlycon_printf(...) con_hprintf( 0, CON_ERROR, __VA_ARGS__ )
#define debugcon_printf(...) con_hprintf( 0, CON_DEBUG, __VA_ARGS__ )
#define panic_printf(...) con_hprintf( 0, CON_PANIC, __VA_ARGS__ )
#define earlycon_puts(...) con_hprintf( 0, CON_ERROR, __VA_ARGS__ )
#define debugcon_puts(...) con_hprintf( 0, CON_DEBUG, __VA_ARGS__ )
#define panic_puts(...) con_hprintf( 0, CON_PANIC, __VA_ARGS__ )
#endif

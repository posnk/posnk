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

void earlycon_init();

void debugcon_init();

void earlycon_putc(char c);

void earlycon_aputs(const char *string);

int earlycon_vsprintf(char *str,const char* format, va_list args);

int earlycon_sprintf(char *str,const char* format,...);

int earlycon_aprintf(const char* str,...);

int debugcon_aprintf(const char* str,...);

void debugcon_aputs(const char *str);
#define earlycon_printf earlycon_aprintf
#define earlycon_puts earlycon_aputs

#define debugcon_printf debugcon_aprintf
#define debugcon_puts debugcon_aputs
#endif

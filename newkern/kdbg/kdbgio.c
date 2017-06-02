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

#include "kdbg/kdbgio.h"
#include <string.h>
#include "kernel/earlycon.h"

char debugcon_getc();

char *kdbg_gets(char *buf, size_t size)
{
	size_t i;
	char c; 
	for (i = 0; i < size;) {
		c = debugcon_getc();
		if (((c == 8) || (c == 0x7F)) && (i != 0)) {
			buf[i--] = 0;
			debugcon_putc('\b');
			debugcon_putc(' ');
			debugcon_putc('\b');
		} else if ((c == '\n') || (c == '\r')) {
			buf[i++] = 0;
			debugcon_putc('\n');
			return buf;
		} else {
			debugcon_putc(c);
			buf[i++] = c;
		}
	}
	return NULL;
}

uintptr_t kdbg_parsehex(char *str)
{
	size_t l = strlen(str);
	int n;
	char c; 
	uintptr_t acc = 0;
	int b = -4;
	for (n = l -1; n >= 0; n--) {
		c = str[n];
		if ((c >= '0') && (c <= '9'))
			c -= '0';
		else if ((c >= 'A') && (c <= 'F'))
			c -= 'A' - 10;
		else if ((c >= 'a') && (c <= 'f'))
			c -= 'a' - 10;
		else
			break; 
		acc |= ((uintptr_t)c) << (b+=4);
	}
	return acc;
}

uintptr_t kdbg_parsedec(char *str)
{
	size_t l = strlen(str);
	int n;
	char c; 
	uintptr_t acc = 0;
	int b = 1;
	for (n = l -1; n >= 0; n--) {
		c = str[n];
		if ((c >= '0') && (c <= '9'))
			c -= '0';
		else
			break; 
		acc += ((uintptr_t)c) * b;
		b *= 10;
	}
	return acc;
}

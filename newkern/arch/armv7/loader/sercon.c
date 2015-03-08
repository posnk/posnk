/**
 * arch/armv7/loader/sercon.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-03-2014 - Created
 */

#include "driver/uart/uart16c750.h"
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define OMAP_UART3_BASE		(0x49020000)

volatile u16c750_general_reg_read  *uart3_read_reg = (void *) OMAP_UART3_BASE;
volatile u16c750_general_reg_write *uart3_write_reg = (void *) OMAP_UART3_BASE;
volatile u16c750_baud_reg	  *uart3_baud_reg = (void *) OMAP_UART3_BASE;

void sercon_init()
{
	/* Do nothing much yet, bootloader has configured UART for us */
}

void sercon_putc(char out)
{
	while (!(uart3_read_reg->line_status & U16C750_LSR_THLD_EMPTY));
	uart3_write_reg->data_transmit = (uint32_t) out;
}

char sercon_getc()
{
	while (!( uart3_read_reg->line_status & U16C750_LSR_DATA_AVAIL ));
	return (char) uart3_read_reg->data_receive;
}

void sercon_puts(const char *string)
{
	while (*string) {
		sercon_putc(*string);
		string++;
	}
}

int earlycon_conup = 0;

char sercon_printf_buffer[256];
void utoa(unsigned int value,int base,char *str);
void utoa_s(int value,int base,char *str);

typedef int (*printf_helper_t)(char c,uint32_t impl);

int sercon_ivprintf (printf_helper_t __helper,uint32_t impl,const char* str, va_list args) {
	char c;
	char strz[64];
	int iii;
	va_list vva;
	size_t i;
	if(!str)
		return 0;
	for (i=0; i<strlen(str);i++) {
		switch (str[i]) {
			case '%':
				switch (str[i+1]) {
					/*** characters ***/
					case 'c': {
						c = (char)va_arg (args, int);
						if (__helper(c,impl) != 0)
							return -1;
						i++;		// go to next character
						break;
					}

					/*** address of ***/
					case 's': {
						iii = (uint32_t) va_arg (args, uint32_t);
						sercon_ivprintf (__helper,impl,(const char*)iii,vva);
						i++;		// go to next character
						break;
					}

					/*** integers ***/
					case 'd':
					case 'i': {
						iii = va_arg (args, int);
						utoa_s (iii, 10, strz);
						sercon_ivprintf (__helper,impl,strz,vva);
						i++;		// go to next character
						break;
					}

					/*** display in hex ***/
					case 'X':
					case 'x': {
						iii = va_arg (args, int);
						utoa (iii,16,strz);
						sercon_ivprintf (__helper,impl,strz,vva);
						i++;		// go to next character
						break;
					}

					default:
						return 1;
				}

				break;
			default:
				if (__helper(str[i],impl) != 0)
					return -1;
				break;
		}

	}
	return i;
}

int sercon_sprintf_helper(char tok,uint32_t impl){
	char **strp = (char **) impl;
	(**strp) = tok;
	(*strp)++;
	(**strp) = 0;
	return 0;
}

int sercon_vsprintf(char *str,const char* format, va_list args){
	char *str2 = str;
	return sercon_ivprintf(
		&sercon_sprintf_helper,
		(uint32_t) &str2,
		format,	
		args);
}

int sercon_sprintf(char *str,const char* format,...){
	va_list args;
	va_start(args,format);
	int res = sercon_vsprintf(str,format,args);
	va_end(args);
	return res;
}

int sercon_printf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = sercon_vsprintf(sercon_printf_buffer, str, args);
	sercon_puts(sercon_printf_buffer);
	va_end(args);
	return res;
}

int debugcon_aprintf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = sercon_vsprintf(sercon_printf_buffer, str, args);
	sercon_puts(sercon_printf_buffer);
	va_end(args);
	return res;
}

int panic_printf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = sercon_vsprintf(sercon_printf_buffer, str, args);
	va_end(args);
	return res;
}

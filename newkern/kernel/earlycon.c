#include "kernel/earlycon.h"
#include <stdint.h>
#include <string.h>

int earlycon_conup = 0;

char earlycon_printf_buffer[256];
char earlycon_screen_buffer[40960];
int earlycon_screen_ptr = 0;
void utoa(unsigned int value,int base,char *str);
void utoa_s(int value,int base,char *str);

typedef int (*printf_helper_t)(char c,uint32_t impl);

int earlycon_ivprintf (printf_helper_t __helper,uint32_t impl,const char* str, va_list args) {
	char c;
	char strz[64];
	int iii;
	size_t i;
	va_list _va_null;
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
						earlycon_ivprintf (__helper,impl,(const char*)iii, _va_null);
						i++;		// go to next character
						break;
					}

					/*** integers ***/
					case 'd':
					case 'i': {
						iii = va_arg (args, int);
						utoa_s (iii, 10, strz);
						earlycon_ivprintf (__helper,impl,strz, _va_null);
						i++;		// go to next character
						break;
					}

					/*** display in hex ***/
					case 'X':
					case 'x': {
						iii = va_arg (args, int);
						utoa (iii,16,strz);
						earlycon_ivprintf (__helper,impl,strz, _va_null);
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

int earlycon_sprintf_helper(char tok,uint32_t impl){
	char **strp = (char **) impl;
	(**strp) = tok;
	(*strp)++;
	(**strp) = 0;
	return 0;
}

int earlycon_vsprintf(char *str,const char* format, va_list args){
	char *str2 = str;
	return earlycon_ivprintf(
		&earlycon_sprintf_helper,
		(uint32_t) &str2,
		format,	
		args);
}

int earlycon_sprintf(char *str,const char* format,...){
	va_list args;
	va_start(args,format);
	int res = earlycon_vsprintf(str,format,args);
	va_end(args);
	return res;
}

int earlycon_aprintf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = earlycon_vsprintf(earlycon_printf_buffer, str, args);
	earlycon_puts(earlycon_printf_buffer);
	va_end(args);
	return res;
}

void earlycon_puts(const char *str){
	size_t len;
	if (!earlycon_conup) {
		len = strlen(str);
		earlycon_aputs(str);
		memcpy(&(earlycon_screen_buffer[earlycon_screen_ptr]), str, len);
		earlycon_screen_ptr += len;
	} else {
		con_puts(str);
	}
}

int debugcon_aprintf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = earlycon_vsprintf(earlycon_printf_buffer, str, args);
	debugcon_aputs(earlycon_printf_buffer);
	va_end(args);
	return res;
}

int panic_printf(const char* str,...){
	va_list args;
	va_start(args,str);
	int res = earlycon_vsprintf(earlycon_printf_buffer, str, args);
	panicscreen(earlycon_printf_buffer);
	va_end(args);
	return res;
}

void earlycon_switchover()
{
	con_puts(earlycon_screen_buffer);
	earlycon_conup = 1;
}

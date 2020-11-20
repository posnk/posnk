/**
 * kernel/cmdline.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 11-06-2017 - Created
 */
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/errno.h>
#include <sys/types.h>
#include "config.h"
#define CON_SRC ("kinit")
#include "kernel/console.h"

char kernel_cmdline [ CONFIG_CMDLINE_MAX_LENGTH ]   = CONFIG_CMDLINE_DEFAULT;
char init_path      [ CONFIG_FILE_MAX_NAME_LENGTH ] = CONFIG_INIT_DEFAULT;
char console_path   [ CONFIG_FILE_MAX_NAME_LENGTH ] = CONFIG_CONSOLE_DEFAULT;
char root_fs        [ 32 ]                          = CONFIG_ROOT_FS_DEFAULT;
dev_t root_dev                                      = CONFIG_ROOT_DEFAULT;

uintptr_t cmdline_parse_hex( const char *str )
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

int cmdline_do_field( char *field, char *value )
{
	      if ( !strcmp( field, "init" ) )
		strcpy( init_path, value );
	 else if ( !strcmp( field, "console" ) )
		strcpy( console_path, value );
	 else if ( !strcmp( field, "rootfs" ) )
		strcpy( root_fs, value );
	 else if ( !strcmp( field, "root" ) )
		root_dev = (dev_t) cmdline_parse_hex(value); //TODO: Support symb. root
	 else if ( con_handle_cmdline( field, value ) )
	 	return 0;
	 else
		printf(CON_WARN, "Unknown kernel parameter \"%s\" with value \"%s\"",
		                field,value);
	//TODO: Allow drivers to parse options
	return 0;
}

void cmdline_parse()
{
	char *pos, *field, *value, *work;
	pos = kernel_cmdline;
	for (;;) {
		/* Look for end of field name */
		work = strchr( pos, '=' );
		/* If there is none, we have reached the end */
		if ( !work )
			break;
		/* Set the pointers for the field */
		field = pos;
		value = work + 1;
		/* Terminate the field name */
		*work = 0;
		/* Look for the next field */
		work = strchr( value, ' ' );
		/* If there is, terminate the current field and update pos */
		if ( work ) {
			*work = 0;
			pos = work + 1;
		}
		/* Handle cmdline field */
		cmdline_do_field( field, value );
	}
}

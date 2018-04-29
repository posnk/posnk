/**
 * kdbg/gdb/io.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#include "kernel/earlycon.h"
#include <string.h>

char debugcon_getc();

int gdblog(const char* str,...)
{
	va_list args;
	va_start(args,str);
	int res = earlycon_vsprintf(earlycon_printf_buffer, str, args);
	earlycon_puts(earlycon_printf_buffer);
	va_end(args);
	return res;
}
 
char gdbport_in()
{
	return debugcon_getc();
}

int  gdbport_avail() { return 0; };

void gdbport_out ( char c )
{
    debugcon_putc ( c );
}

char hstring[17] = "0123456789ABCDEF";

void gdb_outhex ( uint8_t v )
{
    gdbport_out ( hstring[ ( v >> 4 ) & 0xf] );
    gdbport_out ( hstring[   v        & 0xf] );
}

int gdb_dechex ( char c )
{
    if ( c >= '0' && c <= '9' )
        return c - '0';
    else if ( c >= 'A' && c <= 'F' )
        return c - 'A' + 10;
    else if ( c >= 'a' && c <= 'f' )
        return c - 'a' + 10;
    else {
        gdblog ( "Invalid text char" );
        return -1;
    }
}

int gdb_ishex ( char c )
{
    return ( c >= '0' && c <= '9' ) ||
           ( c >= 'A' && c <= 'F' ) ||
           ( c >= 'a' && c <= 'f' );
}

uint32_t gdb_dec32 ( char** str )
{
    uint32_t v = 0;

    while ( gdb_ishex ( **str ) ) {
        v = ( ( v << 4 ) & 0xFFFFFFF0 ) |  gdb_dechex ( **str );
        ( *str )++;
    }

    return v;
}

void gdb_enc32 ( char* buf, uint32_t v )
{
    buf[6] = hstring[ ( v >> 28 ) & 0xF ];
    buf[7] = hstring[ ( v >> 24 ) & 0xF ];
    buf[4] = hstring[ ( v >> 20 ) & 0xF ];
    buf[5] = hstring[ ( v >> 16 ) & 0xF ];
    buf[2] = hstring[ ( v >> 12 ) & 0xF ];
    buf[3] = hstring[ ( v >>  8 ) & 0xF ];
    buf[0] = hstring[ ( v >>  4 ) & 0xF ];
    buf[1] = hstring[   v        & 0xF ];
}


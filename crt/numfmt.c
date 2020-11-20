/******************************************************************************\
Copyright (C) 2015 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file crt/numfmt.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <numfmt.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

void numfmt_signed (	intmax_t num,
			int flags,
			int _width,
			int base,
			char *str,
			size_t str_len )
{

	char pad;
	char buf[ CHAR_BIT * sizeof(intmax_t) + 1];
	int  pos = 1;
	int  sgn = 0;
	int  room;
	size_t width = (size_t) _width;

	assert( base >= 2 );
	assert( base <= 16 );
	assert( _width >= 0 );
	assert( width < sizeof buf );

	/* Convert the number to unsigned */
	if ( ( sgn = (num < 0) ) )
		num = -num;

	/* Convert the number */
	do {
		buf[ sizeof buf - pos++ ] = "0123456789ABCDEF"[ num % base ];
		num /= (intmax_t) base;
	} while ( num != 0 );

	/* Prepend the sign */
	if ( sgn )
		buf[ sizeof buf - pos++ ] = '-';
	else if ( flags & NF_SGNPLUS )
		buf[ sizeof buf - pos++ ] = '+';
	pos--;

	/* Pad the output to the required width */
	room = width - pos;

	if ( room > 0 ) {
		pad = ( flags & NF_ZEROPAD ) ? '0' : ' ';
		memset( &buf[ sizeof buf - width ], pad, room );
	} else
		width = pos;

	if ( width >= str_len - 1 )
		width = str_len - 2;

	/* Copy the output */
	memcpy( str, &buf[ sizeof buf - width ], width );

	str[width] = 0;

}

void numfmt_unsigned (	uintmax_t num,
			int flags,
			int _width,
			unsigned int base,
			char *str,
			size_t str_len )
{

	char pad;
	char buf[ CHAR_BIT * sizeof(int) + 1];
	int  pos = 1;
	int  room;

	size_t width = (size_t) _width;

	assert( base >= 2 );
	assert( base <= 16 );
	assert( _width >= 0 );
	assert( width < sizeof buf );

	/* Convert the number */
	do {
		buf[ sizeof buf - pos++ ] = "0123456789ABCDEF"[ num % base ];
		num /= (uintmax_t) base;
	} while ( num != 0 );

	/* Prepend the sign */
	if ( flags & NF_SGNPLUS )
		buf[ sizeof buf - pos++ ] = '+';

	pos--;

	/* Pad the output to the required width */
	room = width - pos;

	if ( room > 0 ) {
		pad = ( flags & NF_ZEROPAD ) ? '0' : ' ';
		memset( &buf[ sizeof buf - width ], pad, room );
	} else
		width = pos;

	if ( width >= str_len - 1 )
		width = str_len - 2;

	/* Copy the output */

	memcpy( str, &buf[ sizeof buf - width ], width );

	str[width] = 0;

}


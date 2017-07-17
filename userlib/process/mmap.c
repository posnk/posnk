/******************************************************************************\
Copyright (C) 2017 Peter Bosch

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
 * @file userlib/mmap.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include <sys/syscall.h>
#include <sys/types.h>
#include <stdint.h>

void *	mmap( void *addr,
		size_t length,
		int prot,
		int flags,
		int fd,
		off_t offset )
{
	return ( void * ) syscall( SYS_MMAP, 
				( uint32_t ) addr, 
				( uint32_t ) length, 
				( uint32_t ) prot, 
				( uint32_t ) flags,
				( uint32_t ) fd,
				( uint32_t ) offset );
}

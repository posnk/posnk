/******************************************************************************\
Copyright (C) 2020 Peter Bosch

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
 * @file userlib/signal/signal.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include <sys/syscall.h>

static void sigexit(
	__attribute__((unused)) int sig,
	__attribute__((unused)) void *info,
	__attribute__((unused)) void *context,
	__attribute__((unused)) void *sigret )
{
	syscall( SYS_SIGNAL, (uint32_t) sigret, 0, 0, 0, 0, 0 );
	/* does not return */
}

static void __attribute__((constructor)) set_sigexit() {
	syscall( SYS_SSIGEX, (uint32_t) sigexit, 0, 0, 0, 0, 0 );
}


void (*signal(int sig, void (*func)(int)))(int)
{
	return ( void (*)(int) )
		syscall( SYS_SIGNAL, sig, (uint32_t) func, 0, 0, 0, 0 );
}

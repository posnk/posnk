/**
 * vterm/vterm_tty.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-05-2014 - Created
 */

/*
Copyright (C) 2014 Peter Bosch

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
*/

#include <string.h>

#include <glib.h>

#include <sys/types.h>

#include "vterm/vterm.h"
#include "vterm/vterm_private.h"
#include "vterm/vterm_render.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

void vterm_write_tty(vterm_t *vterm, void *buffer, int size)
{
	write(vterm->fd, buffer, size);
}

int vterm_process_tty(vterm_t *vterm)
{
	ssize_t nr;
	char buffer[512];
	do {
		nr = read(vterm->fd, buffer, 512);
		if (nr == -1)
			return 0;
		else if (nr)
			vterm_render(vterm, &buffer, (size_t) nr);
	} while (nr);
	return 1;
}

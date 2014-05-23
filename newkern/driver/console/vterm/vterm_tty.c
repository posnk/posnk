/**
 * driver/console/vterm/vterm_tty.h
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

#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"
#include "driver/console/vterm/vterm_render.h"

#include "kernel/heapmm.h"
#include "kernel/tty.h"

vterm_t *vterm_minor_terms;

void vterm_post_key_tty(dev_t dev, int keycode)
{
	vterm_write_pipe(&(vterm_minor_terms[MINOR(dev)]), keycode);
}

void vterm_write_tty(vterm_t *vterm, void *buffer, int size)
{
	int c;
	char *b = (char *) buffer;
	for (c = 0; c < size; c++)
		tty_input_char(vterm->device_id, b[c]);
}

int vterm_putc(dev_t dev, char c)
{
	vterm_render(&(vterm_minor_terms[MINOR(dev)]), (const char *) &c, sizeof(char));
	return 0;
}

void vterm_tty_setup(char *name, dev_t major, int minor_count, int rows, int cols)
{
	dev_t minor;
	vterm_minor_terms = heapmm_alloc(sizeof(vterm_t) * minor_count);
	for (minor = 0; minor < (dev_t) minor_count; minor++) {
		vterm_init(&(vterm_minor_terms[minor]), VTERM_FLAG_VT100, rows, cols);
		//vterm_minor_terms[minor].ttyname   = "console";
		vterm_minor_terms[minor].device_id = MAKEDEV(major, minor);
	}
	tty_register_driver(name, major, minor_count, &vterm_putc);
}

//tty_register_driver("vgacon", 12, 1, &vgacon_putc);

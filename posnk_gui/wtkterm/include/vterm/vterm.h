/*
Copyright (C) 2009 Bryan Christ

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

/*
This library is based on ROTE written by Bruno Takahashi C. de Oliveira
*/


#ifndef _VTERM_H_
#define _VTERM_H_

#include <glib.h>
#include <sys/types.h>


#define	LIBVTERM_VERSION	"0.99.7"

#define  VTERM_FLAG_VT100	(1<<1)

typedef struct _vterm_s		   vterm_t;

void	       vterm_init(vterm_t *vterm, guint flags, int rows, int cols);
void           vterm_destroy(vterm_t *vterm);
const gchar*   vterm_get_ttyname(vterm_t *vterm);

ssize_t        vterm_read_pipe(vterm_t *vterm);
void           vterm_write_pipe(vterm_t *vterm,guint32 keycode);

int            vterm_set_colors(vterm_t *vterm,short fg,short bg);
short          vterm_get_colors(vterm_t *vterm);

void           vterm_erase(vterm_t *vterm);
void           vterm_erase_row(vterm_t *vterm,gint row);
void           vterm_erase_rows(vterm_t *vterm,gint start_row);
void           vterm_erase_col(vterm_t *vterm,gint col);
void           vterm_erase_cols(vterm_t *vterm,gint start_cols);
void           vterm_scroll_up(vterm_t *vterm);
void           vterm_scroll_down(vterm_t *vterm);

void           vterm_resize(vterm_t *vterm,guint width,guint height);
void		vterm_write_tty(vterm_t *vterm, void *buffer, int size);

int		vterm_process_tty(vterm_t *vterm);
#endif






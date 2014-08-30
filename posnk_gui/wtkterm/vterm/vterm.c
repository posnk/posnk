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

#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include "vterm/vterm.h"
#include "vterm/vterm_private.h"
#include "vterm/vterm_write.h"

void vterm_init(vterm_t *vterm, guint flags, int rows, int cols)
{
   int row;
   /* record dimensions */
   vterm->rows=rows;
   vterm->cols=cols;

   vterm->cells = malloc(rows * sizeof(vterm_cell_t *));
   for (row = 0; row < rows; row++){
       vterm->cells[row] = malloc(cols * sizeof(vterm_cell_t));
	
   }
   vterm->colors = 0x07;
   vterm->curattr = 0x07;
   // initialize all cells with defaults
   vterm_erase(vterm);

   // initialization of other public fields
   vterm->crow=0;
   vterm->ccol=0;
   vterm->state = 0;
   // default active colors
   //TODO:vterm->curattr=COLOR_PAIR(vterm->colors);
   // initial scrolling area is the whole window
   vterm->scroll_min=0;
   vterm->scroll_max=rows-1;
   vterm->fg = 7;
   vterm->bg = 0;
   vterm->curattr = 0x07;

   vterm->flags=flags;

   if(flags & VTERM_FLAG_VT100)
	   vterm->write=vterm_write_vt100;
   else
	   vterm->write=vterm_write_rxvt;
}

void vterm_destroy(vterm_t *vterm)
{

   if(vterm==NULL) return;
   free(vterm);
   return;
}

const gchar* vterm_get_ttyname(vterm_t *vterm)
{
   if(vterm == NULL) return NULL;

   return (const gchar*)vterm->ttyname;
}


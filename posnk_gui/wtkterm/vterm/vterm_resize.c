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

#include "vterm/vterm.h"
#include "vterm/vterm_private.h"
#include "vterm/vterm_misc.h"

void vterm_resize(__attribute__((__unused__)) vterm_t *vterm, __attribute__((__unused__)) guint width, __attribute__((__unused__)) guint height)
{
	/*
   struct winsize ws={.ws_xpixel=0,.ws_ypixel=0};
   guint          i;
   gint           delta_x;
   gint           delta_y;
   gint           start_x;
   gint           start_y;

   if(vterm==NULL) return;
   if(width==0 || height==0) return;

   delta_x=width-vterm->cols;
   delta_y=height-vterm->rows;
   start_x=vterm->cols;
   start_y=vterm->rows;

   vterm->cells=(vterm_cell_t**)g_realloc(vterm->cells,
      sizeof(vterm_cell_t*)*height);

   for(i=0;i < height;i++)
   {
      // realloc() does not initlize new elements
      if((delta_y > 0) && (i > vterm->rows-1)) vterm->cells[i]=NULL;

      vterm->cells[i]=(vterm_cell_t*)g_realloc(vterm->cells[i],
         sizeof(vterm_cell_t)*width);
   }

   vterm->rows=height;
   vterm->cols=width;
   if(!(vterm->state & STATE_SCROLL_SHORT))
   {
      vterm->scroll_max=height-1;
   }

   ws.ws_row=height;
   ws.ws_col=width;

   clamp_cursor_to_bounds(vterm);

   if(delta_x > 0) vterm_erase_cols(vterm,start_x);
   if(delta_y > 0) vterm_erase_rows(vterm,start_y);
*/
   //ioctl(vterm->pty_fd,TIOCSWINSZ,&ws);
   //kill(vterm->child_pid,SIGWINCH);

   return;
}

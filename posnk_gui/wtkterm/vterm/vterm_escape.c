/*
LICENSE INFORMATION:
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License (LGPL) as published by the Free Software Foundation.

Please refer to the COPYING file for more information.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

Copyright (c) 2004 Bruno T. C. de Oliveira
*/

#include <string.h>

#include "vterm/vterm.h"
#include "vterm/vterm_private.h"
#include "vterm/vterm_csi.h"
#include "vterm/vterm_render.h"
#include "vterm/vterm_misc.h"

void vterm_escape_start(vterm_t *vterm)
{
   vterm->state |= STATE_ESCAPE_MODE;
   vterm->esbuf_len=0;
   vterm->esbuf[0]='\0';

   return;
}

void vterm_escape_cancel(vterm_t *vterm)
{
   vterm->state &= ~STATE_ESCAPE_MODE;
   vterm->esbuf_len=0;
   vterm->esbuf[0]='\0';

   return;
}

void try_interpret_escape_seq(vterm_t *vterm)
{
   char  firstchar=vterm->esbuf[0];
   char  lastchar=vterm->esbuf[vterm->esbuf_len-1];

   /* too early to do anything */
   if(!firstchar) return;

   /* interpret ESC-M as reverse line-feed */
   if(firstchar=='M')
   {
      vterm_scroll_up(vterm);
      vterm_escape_cancel(vterm);
      return;
   }

   if(firstchar != '[' && firstchar != ']')
   {
      vterm_escape_cancel(vterm);
      return;
   }

   /* we have a csi escape sequence: interpret it */
   if(firstchar == '[' && validate_escape_suffix(lastchar))
   {
      vterm_interpret_csi(vterm);
      vterm_escape_cancel(vterm);
   }
   /* we have an xterm escape sequence: interpret it */
   else if(firstchar == ']' && lastchar == '\a')
   {
      vterm_escape_cancel(vterm);
   }

   /* if the escape sequence took up all available space and could
    * not yet be parsed, abort it */
   if(vterm->esbuf_len + 1 >= ESEQ_BUF_SIZE) vterm_escape_cancel(vterm);

   return;
}



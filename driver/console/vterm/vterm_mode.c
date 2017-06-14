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
#include "kernel/earlycon.h"

#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"
#include "driver/console/vterm/vterm_write.h"

void vterm_mode_change( vterm_t *vt, vterm_state_t dstate )
{
    debugcon_printf("Modechange: dmode: %x\n", dstate);
    if ( dstate & STATE_COLM )
    {
       debugcon_printf("Modechange: erase screen\n", dstate);
       vt->crow=0;
       vt->ccol=0;
       vt->scroll_min = 0;
       vt->scroll_max = vt->rows-1;
       vt->state &= ~STATE_SCROLL_SHORT;
       vterm_erase( vt );
    }
    if ( dstate & STATE_OM )
    {
        debugcon_printf("Modechange: home cursor\n", dstate);
        if ( vt->state & STATE_OM )
          vt->crow=vt->scroll_min;
        else
          vt->crow=0;
        vt->ccol=0;
    }
}

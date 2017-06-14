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

#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"
#include "driver/console/vterm/vterm_csi.h"
#include "kernel/earlycon.h"

/* Interpret DEC SM (set mode) */
void interpret_dec_SM(vterm_t *vterm,int param[],int pcount)
{
   int i;

   vterm_state_t ostate = vterm->state;

   if(pcount==0) return;

   for(i=0;i < pcount;i++)
   {
      switch ( param[i] )
      {
          case 20:		//LNM     Line feed new line mode
          {
              vterm->state |= STATE_LNM;
              break;
          }
          case 25:		//DECTCEM Cursor Enable
          {
              /* civis is actually "normal" for rxvt */
              vterm->state &= ~STATE_CURSOR_INVIS;
              break;
          }
          case 1:       //DECCKM  Cursor key mode
          {
              vterm->state |= STATE_CKM;
              break;
          }
          case 2:       //DECANM  ANSI/VT52
          {
              vterm->state |= STATE_ANM;
              break;
          }
          case 3:       //DECCOLM Column (80/132 column mode)
          {
              vterm->state |= STATE_COLM;
              ostate &= ~STATE_COLM;// Always signal change
              break;
          }
          case 4:       //DECSCLM Scrolling mode
          {
              vterm->state |= STATE_SCLM;
              break;
          }
          case 5:       //DECSCNM Screen mode
          {
              vterm->state |= STATE_SCNM;
              break;
          }
          case 6:       //DECOM   Origin mode
          {
              vterm->state |= STATE_OM;
              break;
          }
          case 7:       //DECAWM  Auto wrap mode
          {
              vterm->state |= STATE_AWM;
              break;
          }
          case 8:       //DECCKM  Auto repeat mode
          {
              vterm->state |= STATE_ARM;
              break;
          }
          case 9:       //DECINLM Interlace mode
          {
              vterm->state |= STATE_INLM;
              break;
          }
          default:
          {
              debugcon_printf("Unknown VT mode:%i\n",param[i]);
          }
      }
   }

   vterm_mode_change( vterm, ostate ^ vterm->state );

}

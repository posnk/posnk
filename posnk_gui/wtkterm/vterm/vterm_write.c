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

#include <glib.h>

#include "vterm/vterm.h"
#include "vterm/vterm_private.h"
#include "vterm/vterm_write.h"

void vterm_write_tty(vterm_t *vterm, void *buffer, int size);

void vterm_write_pipe(vterm_t *vterm,guint32 keycode)
{
   if(vterm == NULL) return;

   vterm->write(vterm,keycode);

   return;
}



void vterm_write_rxvt(vterm_t *vterm,guint32 keycode)
{
   gchar *buffer=NULL;

   switch(keycode)
   {
      case '\n':           buffer="\r";      break;
      case KEY_UP:         buffer="\033[A";    break;
      case KEY_DOWN:       buffer="\033[B";    break;
      case KEY_RIGHT:      buffer="\033[C";    break;
      case KEY_LEFT:       buffer="\033[D";    break;
      case KEY_BACKSPACE:  buffer="\b";      break;
      case KEY_IC:         buffer="\033[2~";   break;
      case KEY_DC:         buffer="\033[3~";   break;
      case KEY_HOME:       buffer="\033[7~";   break;
      case KEY_END:        buffer="\033[8~";   break;
      case KEY_PPAGE:      buffer="\033[5~";   break;
      case KEY_NPAGE:      buffer="\033[6~";   break;
      case KEY_SUSPEND:    buffer="\x1A";    break;      // ctrl-z
      case KEY_F(1):       buffer="\033[11~";  break;
      case KEY_F(2):       buffer="\033[12~";  break;
      case KEY_F(3):       buffer="\033[13~";  break;
      case KEY_F(4):       buffer="\033[14~";  break;
      case KEY_F(5):       buffer="\033[15~";  break;
      case KEY_F(6):       buffer="\033[17~";  break;
      case KEY_F(7):       buffer="\033[18~";  break;
      case KEY_F(8):       buffer="\033[19~";  break;
      case KEY_F(9):       buffer="\033[20~";  break;
      case KEY_F(10):      buffer="\033[21~";  break;
      case KEY_F(11):      buffer="\033[23~";  break;
      case KEY_F(12):      buffer="\033[24~";  break;
   }

   if(buffer==NULL)
	   vterm_write_tty(vterm, &keycode,sizeof(char));
   else
	   vterm_write_tty(vterm, buffer,strlen(buffer));

   return;
}

void vterm_write_vt100(vterm_t *vterm,guint32 keycode)
{
   gchar *buffer=NULL;

   switch(keycode)
   {
      case '\n':           buffer="\r";      break;
      case KEY_UP:         buffer="\033[A";    break;
      case KEY_DOWN:       buffer="\033[B";    break;
      case KEY_RIGHT:      buffer="\033[C";    break;
      case KEY_LEFT:       buffer="\033[D";    break;
      case KEY_BACKSPACE:  buffer="\b";      break;
      case KEY_IC:         buffer="\033[2~";   break;
      case KEY_DC:         buffer="\033[3~";   break;
      case KEY_HOME:       buffer="\033[7~";   break;
      case KEY_END:        buffer="\033[8~";   break;
      case KEY_PPAGE:      buffer="\033[5~";   break;
      case KEY_NPAGE:      buffer="\033[6~";   break;
      case KEY_SUSPEND:    buffer="\x1A";    break;      // ctrl-z
      case KEY_F(1):       buffer="\033[[A";   break;
      case KEY_F(2):       buffer="\033[[B";   break;
      case KEY_F(3):       buffer="\033[[C";   break;
      case KEY_F(4):       buffer="\033[[D";   break;
      case KEY_F(5):       buffer="\033[[E";   break;
      case KEY_F(6):       buffer="\033[17~";  break;
      case KEY_F(7):       buffer="\033[18~";  break;
      case KEY_F(8):       buffer="\033[19~";  break;
      case KEY_F(9):       buffer="\033[20~";  break;
      case KEY_F(10):      buffer="\033[21~";  break;
   }

   if(buffer==NULL)
	  vterm_write_tty(vterm, &keycode, sizeof(char));
   else
	  vterm_write_tty(vterm, buffer, strlen(buffer));
   return;
}



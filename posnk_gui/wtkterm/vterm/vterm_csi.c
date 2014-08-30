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
#include "vterm/vterm_csi.h"

void vterm_interpret_csi(vterm_t *vterm)
{
   static int  csiparam[MAX_CSI_ES_PARAMS];
   int         param_count = 0;
   const char  *p;
   char        verb;

   p=vterm->esbuf+1;
   verb=vterm->esbuf[vterm->esbuf_len-1];

   /* parse numeric parameters */
   while ((*p >= '0' && *p <= '9') || *p == ';' || *p == '?')
   {
      if(*p == '?')
      {
         p++;
         continue;
      }

      if(*p == ';')
      {
         if(param_count >= MAX_CSI_ES_PARAMS) return; /* too long! */
         csiparam[param_count++]=0;
      }
      else
      {
         if(param_count==0) csiparam[param_count++] = 0;

         csiparam[param_count-1] *= 10;
         csiparam[param_count-1] += *p - '0';
      }

      p++;
   }

   /* delegate handling depending on command character (verb) */
   switch (verb)
   {
      case 'm':
      {
         interpret_csi_SGR(vterm,csiparam,param_count);
			break;
      }

      case 'l':
      {
         interpret_dec_RM(vterm,csiparam,param_count);
         break;
      }

      case 'h':
      {
         interpret_dec_SM(vterm,csiparam,param_count);
         break;
      }

      case 'J':
      {
         interpret_csi_ED(vterm,csiparam,param_count);
         break;
      }
      case 'H':
      case 'f':
      {
         interpret_csi_CUP(vterm,csiparam,param_count);
         break;
      }
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'e':
      case 'a':
      case 'd':
      case '`':
      {
         interpret_csi_CUx(vterm,verb,csiparam,param_count);
         break;
      }
      case 'K':
      {
         interpret_csi_EL(vterm,csiparam,param_count);
         break;
      }
      case '@':
      {
         interpret_csi_ICH(vterm,csiparam,param_count);
         break;
      }
      case 'P':
      {
         interpret_csi_DCH(vterm,csiparam,param_count);
         break;
      }
      case 'L':
      {
         interpret_csi_IL(vterm,csiparam,param_count);
         break;
      }
      case 'M':
      {
         interpret_csi_DL(vterm,csiparam,param_count);
         break;
      }
      case 'X':
      {
         interpret_csi_ECH(vterm,csiparam,param_count);
         break;
      }
      case 'r':
      {
         interpret_csi_DECSTBM(vterm,csiparam,param_count);
         break;
      }
      case 's':
      {
         interpret_csi_SAVECUR(vterm,csiparam,param_count);
         break;
      }
      case 'u':
      {
         interpret_csi_RESTORECUR(vterm,csiparam,param_count);
         break;
      }
#ifdef DEBUG
      default:
         //TODO: fprintf(stderr, "Unrecogized CSI: <%s>\n", rt->pd->esbuf); break;
#endif
   }
}


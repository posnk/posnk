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

#ifndef _VTERM_PRIVATE_H_
#define _VTERM_PRIVATE_H_

#include <glib.h>

#include <sys/types.h>

#define ESEQ_BUF_SIZE 128  		         // size of escape sequence buffer.

#define VTERM_STATIC_WIDTH  80
#define VTERM_STATIC_HEIGHT 25

#define STATE_ALT_CHARSET     (1<<1)
#define STATE_ESCAPE_MODE     (1<<2)
#define STATE_PIPE_ERR        (1<<3)
#define STATE_CHILD_EXITED    (1<<4)
#define STATE_CURSOR_INVIS    (1<<5)
#define STATE_SCROLL_SHORT    (1<<6)      // scrolling region is not full height
#define STATE_LNM             (1<<7)
#define STATE_CKM             (1<<8)
#define STATE_ANM             (1<<9)
#define STATE_COLM            (1<<10)//x0400
#define STATE_SCLM            (1<<11)//x0800
#define STATE_SCNM            (1<<12)//x1000
#define STATE_OM              (1<<13)//x2000
#define STATE_AWM             (1<<14)
#define STATE_ARM             (1<<15)
#define STATE_INLM            (1<<16)
#define STATE_SAVED_ACS       (1<<17)

#define IS_MODE_ESCAPED(x)    (x->state & STATE_ESCAPE_MODE)
#define IS_MODE_ACS(x)        (x->state & STATE_ALT_CHARSET)

#define MOD_SHIFT             (1<<0)
#define MOD_CTRL              (1<<1)
#define MOD_CAPS              (1<<2)
#define MOD_ALT               (1<<3)

#define VTKEY_F1              (256)
#define VTKEY_F2              (257)
#define VTKEY_F3              (258)
#define VTKEY_F4              (259)
#define VTKEY_F5              (260)
#define VTKEY_F6              (261)
#define VTKEY_F7              (262)
#define VTKEY_F8              (263)
#define VTKEY_F9              (264)
#define VTKEY_F10             (265)
#define VTKEY_F11             (266)
#define VTKEY_LEFT            (267)
#define VTKEY_RIGHT           (268)
#define VTKEY_UP              (269)
#define VTKEY_DOWN            (270)
#define VTKEY_F12             (271)
#define VTKEY_F13             (272)
#define VTKEY_NUL             (273)
#define VTKEY_PUP             (274)
#define VTKEY_PDOWN           (275)
#define VTKEY_HOME            (276)
#define VTKEY_END             (277)
#define VTKEY_INS             (278)

typedef struct _vterm_cell_s vterm_cell_t;
typedef guint vterm_state_t;

struct _vterm_cell_s
{
    chtype         ch;                           // cell data
    guint          attr;                         // cell attributes
};

struct _vterm_s
{
	gint	         rows,cols;                    // terminal height & width
	vterm_cell_t     **cells;
	gchar            ttyname[96];                  // populated with ttyname_r()
	guint            curattr;                      // current attribute set
	guint            savedattr;
	gint	         crow,ccol;			            // current cursor column & row
	gint	         scroll_min;					      // top of scrolling region
	gint	         scroll_max;					      // bottom of scrolling region
	gint	         saved_x,saved_y;			      // saved cursor coords
	short            colors;                       // color pair for default fg/bg
	gint             fg,bg;                        // current fg/bg colors
	gchar	         esbuf[ESEQ_BUF_SIZE]; 	      /* 0-terminated string. Does
                                                   NOT include the initial
                                                   escape (\x1B) character.   */
	gint 	         esbuf_len;    	               /* length of buffer. The
                                                   following property is
                                                   always kept:
                                                   esbuf[esbuf_len] == '\0' 	*/
	guint	         flags;						      // user options
	vterm_state_t    state;                        // internal state control
	dev_t		 device_id;
	void             (*write) (vterm_t*,guint32);
};

#define VTERM_CELL(vterm_ptr,x,y)               (vterm_ptr->cols[y][x])

void vterm_invalidate_screen(vterm_t *vt);
void vterm_invalidate_cursor(vterm_t *vt);
void vterm_invalidate_cell(vterm_t *vt, int row, int col);
void vterm_mode_change( vterm_t *vt, vterm_state_t dstate );
#endif


/*
Copyright (C) 2009 Bryan Christ
Copyright (C) 2017 Peter Bosch

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

#include <linux/input.h>

#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"
#include "driver/console/vterm/vterm_write.h"

extern vterm_t *vterm_minor_terms;

int vterm_mods = 0;

int vkbus_reg[128] =
{
    0,  '\033',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\r',
    0,       'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, 
    '\\',       'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',
    0, ' ', 0,
    VTKEY_F1, 
    VTKEY_F2, 
    VTKEY_F3, 
    VTKEY_F4, 
    VTKEY_F5, 
    VTKEY_F6, 
    VTKEY_F7, 
    VTKEY_F8,
    VTKEY_F9,
    VTKEY_F10,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    VTKEY_HOME,  /* Home key */
    VTKEY_UP,  /* Up Arrow */
    VTKEY_PUP,  /* Page Up */
    '-',
    VTKEY_LEFT,  /* Left Arrow */
    0,
    VTKEY_RIGHT,  /* Right Arrow */
    '+',
    VTKEY_END,  /* 79 - End key*/
    VTKEY_DOWN,  /* Down Arrow */
    VTKEY_PDOWN,  /* Page Down */
    VTKEY_INS,  /* Insert Key */
    127,  /* Delete Key */
    0,   0,   0,
    VTKEY_F11,  /* F11 Key */
    VTKEY_F12,  /* F12 Key */
    0,  /* All other keys are undefined */
};      

int vkbus_shift[128] =
{
    0,  '\033',
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\r',
    0,       'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, 
    '|',       'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 
};      

int vkbus_ctrl[128] =
{
    0,  '\033',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 021, 027, 5, 022, 024, 031, 025, 011, 017, 020, 033, 035, '\r',
    0,       1, 023, 4, 6, 7, 010, 012, 013, 014, ';', '\'', 036, 0, 
    034,       032, 030, 3, 026, 2, 016, 015, ',', '.', 037, 
    '*',
    0, VTKEY_NUL,
};   


void vterm_post_key_tty(dev_t dev, int keycode, int val)
{

    int vtk;

    switch ( keycode ) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            if ( val )
                vterm_mods |= MOD_CTRL;
            else
                vterm_mods &= ~MOD_CTRL;
            return;
        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            if ( val )
                vterm_mods |= MOD_ALT;
            else
                vterm_mods &= ~MOD_ALT;
            return;
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            if ( val )
                vterm_mods |= MOD_SHIFT;
            else
                vterm_mods &= ~MOD_SHIFT;
            return;
        case KEY_CAPSLOCK:
            if ( val )
                vterm_mods ^= MOD_CAPS;
            return;
        case 0:
            return;
    }

    if ( keycode >= ( sizeof vkbus_reg / sizeof vkbus_reg[0] ) )
        return;
        
    vtk = vkbus_reg[ keycode ];
     
    if ( ( vterm_mods & MOD_CTRL ) &&
         ( keycode < ( sizeof vkbus_ctrl / sizeof vkbus_ctrl[0] ) ) )
        vtk = vkbus_ctrl[ keycode ];
     
    if ( ( vterm_mods & MOD_SHIFT ) &&
         ( keycode < ( sizeof vkbus_shift / sizeof vkbus_shift[0] ) ) )
        vtk = vkbus_shift[ keycode ];

    if ( vterm_mods & MOD_CAPS )
    {
        if ( vtk >= 'a' && vtk <= 'z' )
            vtk = ( vtk - 'a') + 'A';
        else if ( vtk >= 'A' && vtk <= 'Z' )
            vtk = ( vtk - 'A') + 'a';
    }

    if ( val )
    	vterm_write_pipe(&(vterm_minor_terms[MINOR(dev)]), vtk);
}

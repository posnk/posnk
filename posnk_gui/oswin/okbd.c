#include <stdint.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <clara/cinput.h>
#include <errno.h>
#include <string.h>
#include "oinput.h"

#define KBD_CPSLCK_BIT 1
#define KBD_SHIFT_BIT 2
#define KBD_CTRL_BIT 4
#define KBD_ALT_BIT 8
#define KBD_WIN_BIT 16
#define KBD_MENU_BIT 32
#define KBD_SCLLCK_BIT 64
#define KBD_NUMLCK_BIT 128

extern int kbdus_reg[128];           
extern int kbdus_caps[128];              
extern int kbdus_shift[128];              
//extern int kbdus_ctrl[128]; 
extern int kbdus_nt[128];

uint8_t oswin_modifiers;

void oswin_kb_toggle_modifier(uint8_t mask){
	if (oswin_modifiers & mask)
		oswin_modifiers &= ~mask;
	else
		oswin_modifiers |= mask;
}

void oswin_kb_set_modifier(uint8_t mask){
	oswin_modifiers |= mask;
}

void oswin_kb_clear_modifier(uint8_t mask){
	oswin_modifiers &= ~mask;
}

void oswin_handle_key(int val, uint32_t scancode){
        int result;
	clara_event_msg_t msg;
        if (oswin_modifiers & KBD_CPSLCK_BIT){
                result = kbdus_caps[scancode];
        } else if (oswin_modifiers & KBD_SHIFT_BIT){
                result = kbdus_shift[scancode];
       // } else if (oswin_modifiers & KBD_CTRL_BIT){
       //         result = kbdus_ctrl[scancode];
        }  else {
                result = kbdus_reg[scancode];
        }
	if (val) {
		switch (scancode){
			case KEY_LEFTCTRL:
			case KEY_RIGHTCTRL:
				oswin_kb_set_modifier(KBD_CTRL_BIT);
				break;
			case KEY_LEFTALT:
			case KEY_RIGHTALT:
				oswin_kb_set_modifier(KBD_ALT_BIT);
				break;
			case KEY_LEFTSHIFT:
			case KEY_RIGHTSHIFT:
				oswin_kb_set_modifier(KBD_SHIFT_BIT);
				break;
			case KEY_LEFTMETA:
			case KEY_RIGHTMETA:
				oswin_kb_set_modifier(KBD_WIN_BIT);
				break;
			case KEY_MENU:
				oswin_kb_set_modifier(KBD_MENU_BIT);
	  			break;
			case KEY_CAPSLOCK:
				oswin_kb_toggle_modifier(KBD_CPSLCK_BIT);
				break;
			case KEY_SCROLLLOCK:
				oswin_kb_toggle_modifier(KBD_SCLLCK_BIT);
				break;
			case KEY_NUMLOCK:
				oswin_kb_toggle_modifier(KBD_NUMLCK_BIT);
				break;
		}
		msg.event_type = CLARA_EVENT_TYPE_KEY_DOWN;
		msg.flags = CLARA_EVENT_FLAG_FOCUS;
		msg.param[0] = scancode;
		msg.param[1] = kbdus_nt[scancode];
		msg.param[2] = oswin_modifiers;
		oswin_input_handle(&msg);
	} else {
		switch (scancode){
			case KEY_LEFTCTRL:
			case KEY_RIGHTCTRL:
				oswin_kb_clear_modifier(KBD_CTRL_BIT);
				break;
			case KEY_LEFTALT:
			case KEY_RIGHTALT:
				oswin_kb_clear_modifier(KBD_ALT_BIT);
				break;
			case KEY_LEFTSHIFT:
			case KEY_RIGHTSHIFT:
				oswin_kb_clear_modifier(KBD_SHIFT_BIT);
				break;
			case KEY_LEFTMETA:
			case KEY_RIGHTMETA:
				oswin_kb_clear_modifier(KBD_WIN_BIT);
				break;
			case KEY_MENU:
				oswin_kb_clear_modifier(KBD_MENU_BIT);
				break;
		}
		msg.event_type = CLARA_EVENT_TYPE_KEY_UP;
		msg.flags = CLARA_EVENT_FLAG_FOCUS;
		msg.param[0] = scancode;
		msg.param[1] = kbdus_nt[scancode];
		msg.param[2] = oswin_modifiers;
		oswin_input_handle(&msg);
	}
	if (result && val) {
		msg.event_type = CLARA_EVENT_TYPE_KEY_TYPE;
		msg.flags = CLARA_EVENT_FLAG_FOCUS;
		msg.param[0] = scancode;
		msg.param[1] = result;
		msg.param[2] = oswin_modifiers;
		oswin_input_handle(&msg);
	}
}


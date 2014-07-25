#include "driver/input/ps2/i8042.h"
#include "driver/oldkb/kbd.h"
#include "kernel/tty.h"
#include <glib.h>

uint8_t modifiers = 0;
uint8_t extended = 0;
uint8_t currently_held_key = 0;
void oldkb_init(){
        kb_set_leds(0xFF);
        kb_set_leds(0x00);
}

void kb_set_leds(uint8_t leds){
        uint8_t leds_hw = 0;
        if ((leds & KBD_SCLLCK_BIT) == KBD_SCLLCK_BIT)
                leds_hw |= 0x1;
        if ((leds & KBD_NUMLCK_BIT) == KBD_NUMLCK_BIT)
                leds_hw |= 0x2;
        if ((leds & KBD_CPSLCK_BIT) == KBD_CPSLCK_BIT)
                leds_hw |= 0x4;
        i8042_write(0,0xED);
        i8042_write(0,leds_hw);
}

void kb_set_modifiers(uint8_t _modifiers){
        modifiers = _modifiers;
}

void kb_toggle_modifier(uint8_t mask){
        if (modifiers & mask)
                modifiers &= ~mask;
        else
                modifiers |= mask;
}

void kb_set_modifier(uint8_t mask){
        modifiers |= mask;
}

void kb_clear_modifier(uint8_t mask){
        modifiers &= ~mask;
}

void kb_pressed(uint8_t scan){
        kb_typed(scan);
}

void con_handle_key(int keycode);

void kb_typed(uint8_t scan){
        int t = decode_keystroke(scan, modifiers);
	if (t)		
		con_handle_key(t);
}

void kb_released(__attribute__((__unused__)) uint8_t scan){
}

void ps2kbd_handle_data(uint8_t scancode)
{
if ((scancode == 0xE0) || (scancode == 0xE1)){
                        extended = 1;
                        return;
                }
                if (scancode & 0x80){
                        kb_released(currently_held_key);//
                        if ((currently_held_key & 0x80) == scancode)
                                return;
                        scancode &= ~0x80;
                        switch (scancode){
                                case 0x1D:
                                        kb_clear_modifier(KBD_CTRL_BIT);
                                        break;
                                case 0x38:
                                        kb_clear_modifier(KBD_ALT_BIT);
                                        break;
                                case 0x2A:
                                case 0x36:
                                        if (!extended)
                                        kb_clear_modifier(KBD_SHIFT_BIT);
                                        break;
                                case 0x5B:
                                case 0x5C:
                                        if (extended)
                                                kb_clear_modifier(KBD_WIN_BIT);
                                        break;
                                case 0x5D:
                                        if (extended)
                                                kb_clear_modifier(KBD_MENU_BIT);
                                        break;
                        }
			extended = 0;
                        return;        
                }
                if (scancode == currently_held_key){
                        kb_typed(scancode);//
			extended = 0;
                        return;
                }
                currently_held_key = scancode;
                kb_pressed(scancode);//
                switch (scancode){
                                case 0x1D:
                                        kb_set_modifier(KBD_CTRL_BIT);
                                        break;
                                case 0x38:
                                        kb_set_modifier(KBD_ALT_BIT);
                                        break;
                                case 0x2A:
                                case 0x36:
                                        if (!extended)
                                        kb_set_modifier(KBD_SHIFT_BIT);
                                        break;
                                case 0x5B:
                                case 0x5C:
                                        if (extended)
                                                kb_set_modifier(KBD_WIN_BIT);
                                        break;
                                case 0x5D:
                                        if (extended)
                                                kb_set_modifier(KBD_MENU_BIT);
                                        break;
                                case 0x3A:
                                        if (!extended)
                                        kb_toggle_modifier(KBD_CPSLCK_BIT);
                                        break;
                                case 0x46:
                                        if (!extended)
                                        kb_toggle_modifier(KBD_SCLLCK_BIT);
                                        break;
                                case 0x45:
                                        if (!extended)
                                        kb_toggle_modifier(KBD_NUMLCK_BIT);
                                        break;
               }    
			extended = 0;
}

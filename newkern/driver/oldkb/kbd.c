#include "driver/oldkb/i8042.h"
#include "driver/oldkb/kbd.h"
#include "kernel/tty.h"


uint8_t modifiers = 0;

void kb_initialize(){
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
        kbd_send_encoder_command(0xED);
        kbd_send_encoder_command(leds_hw);
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

void kb_released(uint8_t scan){
}


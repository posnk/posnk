#include <stdint.h>
#include "arch/i386/x86.h"
#include "driver/oldkb/i8042.h"
#define KBD_CPSLCK_BIT 1
#define KBD_SHIFT_BIT 2
#define KBD_CTRL_BIT 4
#define KBD_ALT_BIT 8
#define KBD_WIN_BIT 16
#define KBD_MENU_BIT 32
#define KBD_SCLLCK_BIT 64
#define KBD_NUMLCK_BIT 128
void uprinti(uint32_t i);
uint8_t currently_held_key = 0x00;
uint8_t extended = 0;
int sigfile_hndl = 0;

uint8_t kbd_controller_status(){
        return i386_inb(KBD_CTRL_IO_PORT);
}

void kbd_send_controller_command(uint8_t cmd){
        while(1)
                if((kbd_controller_status() & KBD_CTRL_STATUS_IN_BUF_BIT) == 0)
                        break;
        i386_outb(KBD_CTRL_IO_PORT,cmd);
}

uint8_t kbd_encoder_read_buffer(){
        return i386_inb(KBD_ENC_IO_PORT);
}

void kbd_send_encoder_command(uint8_t cmd){
        while(1)
                if((kbd_controller_status() & KBD_CTRL_STATUS_IN_BUF_BIT) == 0)
                        break;
        i386_outb(KBD_ENC_IO_PORT,cmd);
}

void kbd_isr(){
        uint8_t scancode;
        if (kbd_controller_status () & KBD_CTRL_STATUS_OUT_BUF_BIT){
                scancode = kbd_encoder_read_buffer();
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
}


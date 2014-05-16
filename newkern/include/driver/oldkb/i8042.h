#ifndef __i8042_kbd_driver_i8042_h__
#define __i8042_kbd_driver_i8042_h__

#include <stdint.h>
#include "arch/i386/x86.h"

#define KBD_ENC_IO_PORT			0x60
#define KBD_CTRL_IO_PORT		0x64
#define KBD_CTRL_STATUS_OUT_BUF_BIT	1<<0
#define KBD_CTRL_STATUS_IN_BUF_BIT      1<<1
#define KBD_CTRL_STATUS_SYSTEM_BIT      1<<2
#define KBD_CTRL_STATUS_COMMAND_BIT	1<<3
#define KBD_CTRL_STATUS_LOCKED_BIT      1<<4
#define KBD_CTRL_STATUS_BUFFER_BIT      1<<5
#define KBD_CTRL_STATUS_TIMEOUT_BIT     1<<6
#define KBD_CTRL_STATUS_PARITY_BIT      1<<7

#define KBD_CPSLCK_BIT			1
#define KBD_SHIFT_BIT			2
#define KBD_CTRL_BIT			4
#define KBD_ALT_BIT			8
#define KBD_WIN_BIT			16
#define KBD_MENU_BIT			32
#define KBD_SCLLCK_BIT			64
#define KBD_NUMLCK_BIT			128

uint8_t kbd_controller_status();
void kbd_send_controller_command(uint8_t cmd);

uint8_t kbd_encoder_read_buffer();
void kbd_send_encoder_command(uint8_t cmd);
void kbd_isr();

#endif


/**
 * driver/input/ps2/mouse.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-08-2014 - Created
 */

#include "driver/input/ps2/i8042.h"
#include "driver/input/evdev.h"

evdev_device_info_t ps2mouse_info;

int mouse_pkt_len = 3;
int mouse_ptr = 0;
uint8_t mouse_buf[4];
int stream_data = 0;
int mouse_btns = 0;
struct input_event mouse_ev_buf;

void ps2mouse_enable_stream()
{	
	i8042_write(1, 0xF4);
	mouse_ptr = 0;
	stream_data = 2;
}

void ps2mouse_reset()
{
	stream_data = 3;
	mouse_btns = 0;
	i8042_write(1, 0xFF);	
}

void ps2mouse_send_btn(int btn, int val)
{
	mouse_ev_buf.type = EV_KEY;	
	mouse_ev_buf.code = btn;
	mouse_ev_buf.value = val ? 1 : 0;
	evdev_post_event(ps2mouse_info.device, mouse_ev_buf);	
}

void ps2mouse_send_axis(int axs, int val)
{
	mouse_ev_buf.type = EV_REL;	
	mouse_ev_buf.code = axs;
	mouse_ev_buf.value = val;
	evdev_post_event(ps2mouse_info.device, mouse_ev_buf);	
}

void ps2aux_handle_data(uint8_t data)
{
	int newbtns, x, y, bchg;
	if (stream_data == 0)
		return;
	else if (stream_data == 2){
		if (data == 0xFA)
			stream_data--;
		return;
	} else if (stream_data == 4){
		if (data == 0xFA)
			stream_data--;
		return;
	} else if (stream_data == 3){
		if (data == 0xAA)
			ps2mouse_enable_stream();
		return;
	}
	
	mouse_buf[mouse_ptr++] = data;
	if (mouse_ptr == mouse_pkt_len) {
		mouse_ptr = 0;
		if (~mouse_buf[0] & 0x08) {
			ps2mouse_reset();
			return;
		}
		newbtns = mouse_buf[0] & 0x07;
		bchg = mouse_btns ^ newbtns;
		mouse_btns = newbtns;
		if (bchg & 1) 
			ps2mouse_send_btn(BTN_LEFT, mouse_btns & 1);
		if (bchg & 2) 
			ps2mouse_send_btn(BTN_RIGHT, mouse_btns & 2);
		if (bchg & 4) 
			ps2mouse_send_btn(BTN_MIDDLE, mouse_btns & 4);
		x = mouse_buf[1] & 0xFF;
		y = mouse_buf[2] & 0xFF;
		if (mouse_buf[0] & 0x10)
			x |= 0xFFFFFF00;
		if (mouse_buf[0] & 0x20)
			y |= 0xFFFFFF00;
		if (x != 0)
			ps2mouse_send_axis(REL_X, x);
		if (y != 0)
			ps2mouse_send_axis(REL_Y, y);
	} 
}

void ps2mouse_init()
{
	evdev_register_device(&ps2mouse_info);
	i8042_enable_aux();
	ps2mouse_reset();
}

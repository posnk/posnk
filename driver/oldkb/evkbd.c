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

evdev_device_info_t evkbd_info;

struct input_event evkbd_ev_buf;

void evkbd_send_btn(int btn, int val)
{
	evkbd_ev_buf.type = EV_KEY;	
	evkbd_ev_buf.code = btn;
	evkbd_ev_buf.value = val ? 1 : 0;
	evdev_post_event(evkbd_info.device, evkbd_ev_buf);	
}

void evkbd_init()
{
	evdev_register_device(&evkbd_info);
}

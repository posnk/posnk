#include <clara/cmsg.h>
#include <clara/cinput.h>
#include <clara/ctypes.h>
#ifndef __OSWIN_OINPUT_H__
#define __OSWIN_OINPUT_H__

extern clara_point_t	oswin_input_pointer_position;

int oswin_input_init();
void oswin_input_close();
int oswin_input_send(clara_event_msg_t *msg);
int oswin_input_process();
int oswin_input_handle(clara_event_msg_t *event);
#endif

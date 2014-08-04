#include <clara/cmsg.h>
#include <clara/cinput.h>
#include <clara/ctypes.h>
#ifndef __OSWIN_OINPUT_H__
#define __OSWIN_OINPUT_H__

#define OSWIN_INPUT_ACTION_WIN_CLOSE	(0)
#define OSWIN_INPUT_ACTION_WIN_MAXIMIZE	(1)
#define OSWIN_INPUT_ACTION_WIN_ICONIFY	(2)
#define OSWIN_INPUT_ACTION_WIN_DRAG	(3)
#define OSWIN_INPUT_ACTION_WIN_RESIZE	(4)
#define OSWIN_INPUT_ACTION_NONE		(5)

extern clara_point_t	oswin_input_pointer_position;

int oswin_input_init();
void oswin_input_close();
int oswin_input_send(clara_event_msg_t *msg);
int oswin_input_process();
int oswin_input_handle(clara_event_msg_t *event);
#endif

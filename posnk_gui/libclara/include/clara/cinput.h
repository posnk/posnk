#include <stdint.h>
#include <sys/types.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#ifndef __CLARA_CINPUT_H__
#define __CLARA_CINPUT_H__

#define CLARA_MSG_EVENT			(8)

#define CLARA_EVENT_FLAG_FOCUS		(1<<0)
#define CLARA_EVENT_FLAG_WM		(1<<1)
#define CLARA_EVENT_FLAG_PNT_DELTA	(1<<2)
#define CLARA_EVENT_FLAG_PNT_ABS	(1<<3)
#define CLARA_EVENT_FLAG_SETFOCUS	(1<<4)

#define CLARA_EVENT_TYPE_MOUSE_BTN_UP	(0)
#define CLARA_EVENT_TYPE_MOUSE_BTN_DOWN	(1)
#define CLARA_EVENT_TYPE_MOUSE_MOVE	(2)
#define CLARA_EVENT_TYPE_CLOSE_WINDOW	(3)
#define CLARA_EVENT_TYPE_KEY_UP		(4)
#define CLARA_EVENT_TYPE_KEY_DOWN	(5)
#define CLARA_EVENT_TYPE_RESIZE_WINDOW	(6)
#define CLARA_EVENT_TYPE_MOVE_WINDOW	(7)
#define CLARA_EVENT_TYPE_KEY_TYPE	(8)

typedef struct {
	clara_message_t	msg;	
	int		flags;
	clara_point_t	ptr;
	int		event_type;
	uint32_t	param[4];
} clara_event_msg_t;

#endif

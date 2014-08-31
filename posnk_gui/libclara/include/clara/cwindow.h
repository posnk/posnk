#include <stdint.h>
#include <sys/types.h>
#include <clara/cllist.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#include <clara/csession.h>
#ifndef __CLARA_CWINDOW_H__
#define __CLARA_CWINDOW_H__

#define CLARA_MSG_INIT_WIN		(5)
#define CLARA_MSG_DAMAGE_WIN		(6)		

#define CLARA_WIN_FLAG_INITIALIZED	(1<<0)
#define CLARA_WIN_FLAG_UNDECORATED	(1<<1)
#define CLARA_WIN_FLAG_NOFRONT		(1<<2)
#define CLARA_WIN_FLAG_NOLIST		(1<<3)

typedef struct {
	clara_message_t	 msg;
	uint32_t	 flags;
	char		 title[128];
	clara_rect_t	 dimensions;

	int		 surface_handle;
	size_t		 surface_size;	
	uint16_t	 surface_width;
	uint16_t	 surface_height;
	
} clara_initwin_msg_t;

typedef struct {
	clara_message_t	 msg;
	clara_rect_t	 rect;
	
} clara_dmgwin_msg_t;

typedef struct {
	cllist_t	 link;
	uint32_t	 handle;
	uint32_t	 flags;
	clara_rect_t	 dimensions;
	clara_rect_t	 frame_dims;
	int		 surface_handle;
	clara_surface_t	 surface;
	char		 title[128];
} clara_window_t;

clara_window_t *clara_window_get(clara_session_t *session, uint32_t handle);

void clara_window_add(clara_session_t *session, uint32_t handle);

int clara_window_attach_surface(clara_window_t *window);

int clara_window_do_init(clara_window_t *window, clara_initwin_msg_t *msg);

int clara_init_window(uint32_t handle, const char *title, clara_rect_t dimensions, uint32_t flags, int surface_w, int surface_h);

int clara_window_add_damage(uint32_t handle, clara_rect_t rect);

#endif





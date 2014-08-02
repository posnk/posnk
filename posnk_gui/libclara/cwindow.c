#include <stdint.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include <clara/csession.h>
#include <clara/cwindow.h>
#include <clara/cllist.h>
#include <clara/cmsg.h>

int clara_window_get_iterator(cllist_t *node, void *param)
{
	clara_window_t *window = (clara_window_t *) node;
	return window->handle == (uint32_t) param;
}

clara_window_t *clara_window_get(clara_session_t *session, uint32_t handle)
{
	assert(session != NULL);
	return (clara_window_t *) cllist_iterate_select(&(session->window_list), &clara_window_get_iterator, (void *) handle);
}

void clara_window_add(clara_session_t *session, uint32_t handle)
{
	clara_window_t *window = malloc(sizeof(clara_window_t));
	assert(window != NULL);

	memset(window, 0, sizeof(clara_window_t));

	window->handle = handle;

	cllist_add_end(&(session->window_list), (cllist_t *) window);
}

int clara_window_attach_surface(clara_window_t *window)
{
	void *pixels = shmat(window->surface_handle, (void *) 0, 0);
	if (pixels == (void *) -1) {
		fprintf(stderr, "libcarla: could not attach surface %i: %s\n", window->surface_handle, strerror(errno));
		return -1;
	}
	window->surface.pixels = (uint32_t *) pixels;
	return 0;
}

int clara_window_do_init(clara_window_t *window, clara_initwin_msg_t *msg)
{
	int st;
	assert(window != NULL);
	assert(msg != NULL);
	
	window->flags = msg->flags | CLARA_WIN_FLAG_INITIALIZED;
	strcpy(window->title, msg->title);
	window->dimensions = msg->dimensions;

	assert(msg->surface_width > 0);
	assert(msg->surface_height > 0);
	assert(msg->surface_size >= (msg->surface_width * msg->surface_height  * sizeof(uint32_t)));
	
	window->surface_handle = msg->surface_handle;
	window->surface.size = msg->surface_size;
	window->surface.w = msg->surface_width;
	window->surface.h = msg->surface_height;
	
	st = clara_window_attach_surface(window);
	if (st)
		return st;

	memset(window->surface.pixels, 0, window->surface.size);

	return 0;
}

int clara_init_window(uint32_t handle, const char *title, clara_rect_t dimensions, uint32_t flags, int surface_w, int surface_h)
{
	clara_window_t *window;
	clara_initwin_msg_t iw_msg;
	int st;
	
	window = clara_window_get(&clara_client_session, handle);

	if (window == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (strlen(title) > 127) {
		errno = E2BIG;
		return -1;
	}

	iw_msg.flags = flags;
	strcpy(iw_msg.title, title);
	iw_msg.dimensions = dimensions;

	iw_msg.surface_width = (uint16_t) surface_w;
	iw_msg.surface_height = (uint16_t) surface_h;
	iw_msg.surface_size = surface_w * surface_h * sizeof(uint32_t);

	iw_msg.surface_handle = shmget(IPC_PRIVATE, iw_msg.surface_size, 0666 | IPC_CREAT);
	
	if (iw_msg.surface_handle == -1) {
		st = errno;
		fprintf(stderr, "libclara: could not allocate surface SHM object: %s\n", strerror(errno));
		errno = st;
		return -1;
	}

	st = clara_send_cmd_sync(handle, CLARA_MSG_INIT_WIN, &iw_msg, CLARA_MSG_SIZE(clara_initwin_msg_t));

	if (st < 0)
		return st;

	st = clara_window_do_init(window, &iw_msg);

	if (st < 0)
		return st;

	return 0;	
}

int clara_window_add_damage(uint32_t handle, clara_rect_t rect)
{
	clara_window_t *window;
	clara_dmgwin_msg_t dw_msg;
	int st;
	
	window = clara_window_get(&clara_client_session, handle);

	if (window == NULL) {
		errno = EINVAL;
		return -1;
	}

	dw_msg.rect = rect;

	st = clara_send_cmd_sync(handle, CLARA_MSG_DAMAGE_WIN, &dw_msg, CLARA_MSG_SIZE(clara_dmgwin_msg_t));

	if (st < 0)
		return st;

	return 0;	
}

clara_surface_t *clara_window_get_surface(uint32_t handle)
{

	clara_window_t *window;
	clara_surface_t *surface;
	
	window = clara_window_get(&clara_client_session, handle);

	if (!window) {
		errno = EINVAL;
		return NULL;
	}

	surface = malloc(sizeof(clara_surface_t));

	if (!surface) {
		errno = ENOMEM;
		return NULL;
	}

	surface->w = window->surface.w;
	surface->h = window->surface.h;
	surface->size = window->surface.size;

	surface->pixels = window->surface.pixels;
	
	return surface;
	
}

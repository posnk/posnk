#include <wtk/window.h>
#include <wtk/widget.h>
#include <clara/clara.h>
#include <clara/cinput.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

wtk_window_t *wtk_window_create(int width, int height, int flags, char *name)
{
	int st;
	cairo_status_t status;

	clara_rect_t dims = {0, 0, width, height};
	wtk_window_t *wnd = malloc ( sizeof(wtk_window_t) );

	wnd->handle = (uint32_t) clara_create_window();

	if (wnd->handle == (uint32_t) -1) {
		printf("couldn't create window: %s", strerror(errno));
		free(wnd);
		return NULL;
	}

	st = clara_init_window(wnd->handle, name, dims, flags, width, height);

	if (st == -1){
		printf("couldn't init window: %s", strerror(errno));
		free(wnd);
		return NULL;
	}

	wnd->widget = wtk_create_widget(dims);

	wnd->c_surf = clara_window_get_surface(wnd->handle);

	wnd->surface = cairo_image_surface_create_for_data ((unsigned char *)wnd->c_surf->pixels, CAIRO_FORMAT_RGB24, wnd->c_surf->w, wnd->c_surf->h, wnd->c_surf->w * 4);
	if (!wnd->surface) {
		printf("could not create cairo surface\n");
		free(wnd);
		return NULL;
	}

	wnd->context = cairo_create (wnd->surface);

	status = cairo_status(wnd->context);
	if (status != CAIRO_STATUS_SUCCESS) {
		printf("fatal: could not create cairo context: %s\n", cairo_status_to_string(status));
		free(wnd);
		return NULL;
	}

	wnd->b_surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, wnd->c_surf->w, wnd->c_surf->h);
	if (!wnd->b_surface) {
		printf("could not create cairo back buffer surface\n");
		free(wnd);
		return NULL;
	}

	wnd->b_context = cairo_create (wnd->b_surface);

	status = cairo_status(wnd->b_context);
	if (status != CAIRO_STATUS_SUCCESS) {
		printf("fatal: could not create cairo back buffer context: %s\n", cairo_status_to_string(status));
		free(wnd);
		return NULL;
	}

	return wnd;
}

void wtk_window_evprocess(wtk_window_t *wnd)
{
	ssize_t nr;
	clara_event_msg_t in_buf;
	do {
		nr = clara_recv_ev(wnd->handle, &in_buf, CLARA_MSG_SIZE(clara_event_msg_t));
		if (nr == -1) {
			printf("error: could not receive commands: %s\n", strerror(errno));
			return;
		} else if (nr) {
			switch (in_buf.event_type) {
				case CLARA_EVENT_TYPE_MOVE_WINDOW:
					break;
				case CLARA_EVENT_TYPE_RESIZE_WINDOW:
					//TODO: Resize buffer
					wtk_widget_resize(wnd->widget, in_buf.param[0], in_buf.param[1]);
					break;
				default:	
					wtk_widget_handle_event(wnd->widget, &in_buf);	
					break;
			}		
		}
	} while(nr);
}


void wtk_window_process(wtk_window_t *wnd)
{
	cairo_new_path(wnd->b_context);
	cairo_reset_clip(wnd->b_context);

	wtk_widget_do_clip(wnd->widget, wnd->b_context);

	cairo_clip (wnd->b_context);

	wtk_widget_render(wnd->widget, wnd->b_context, 1);

	cairo_set_source_surface(wnd->context, wnd->b_surface, 0, 0);
	cairo_paint(wnd->context);

	clara_window_add_damage(wnd->handle, wnd->widget->rect);

	wtk_window_evprocess(wnd);
}

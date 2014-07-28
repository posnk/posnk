#include "osession.h"
#include "odecor.h"
#include "oinput.h"
#include <clara/clara.h>
#include <stdlib.h>
#include <bitmap.h>
#include <clara/cllist.h>
#include <clara/cwindow.h>

extern cllist_t oswin_session_list;

void oswin_window_render(osession_node_t *session, clara_surface_t *target)
{
	cllist_t *_s;
	clara_surface_t *wdisp;
	clara_window_t *window;
	for (_s = session->session.window_list.next; _s != &(session->session.window_list); _s = _s->next) {
		window = (clara_window_t *) _s;
		if (!(window->flags & CLARA_WIN_FLAG_INITIALIZED))
			continue;
		wdisp =  clara_window_get_disp_surface(window);
		clara_copy_surface(target, window->dimensions.x, window->dimensions.y, wdisp);
		free(wdisp);
		oswin_decorations_render(session, target, window);
	}
}

void oswin_session_render(clara_surface_t *target)
{
	cllist_t *_s;
	osession_node_t *session;
	for (_s = oswin_session_list.next; _s != &oswin_session_list; _s = _s->next) {
		session = (osession_node_t *) _s;
		oswin_window_render(session, target);
	}
}

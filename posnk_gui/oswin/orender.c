#include "osession.h"
#include "orender.h"
#include "owindow.h"
#include "odecor.h"
#include "oinput.h"
#include "ovideo.h"
#include <clara/clara.h>
#include <stdlib.h>
#include <bitmap.h>
#include <assert.h>
#include <clara/cllist.h>
#include <clara/cwindow.h>

extern oswin_video_t	oswin_video;
extern cllist_t oswin_session_list;

cllist_t	oswin_damage_list;

clara_point_t	oswin_old_pointer_position;

void oswin_render_init()
{
	cllist_create(&oswin_damage_list);
}

void oswin_add_damage(clara_rect_t rect)
{
	oswin_damage_t *drect = malloc (sizeof(oswin_damage_t));
	assert(drect != NULL);
	drect->rect = rect;
	cllist_add_end(&oswin_damage_list, (cllist_t *) drect);
}

void oswin_render(cairo_t *cr)
{
	cairo_matrix_t	m;
	oswin_window_t *wnd;
	oswin_damage_t *drect;
	cllist_t *_n, *_d;

	clara_point_t cursr = oswin_input_pointer_position;
	int cchg = (cursr.x != oswin_old_pointer_position.x) || (cursr.y != oswin_old_pointer_position.y);

	oswin_start_clip();
	
	oswin_cursor_clip(cursr.x, cursr.y);

	if (cchg) {
		oswin_cursor_clip(oswin_old_pointer_position.x, oswin_old_pointer_position.y);
	}	

	oswin_old_pointer_position = cursr;

	for (_n = oswin_damage_list.next; _n != &oswin_damage_list; _n = _n->next) {
		drect = (oswin_damage_t *) _n;
		oswin_set_clip(drect->rect.x, drect->rect.y, drect->rect.w, drect->rect.h);
		_d = _n->next;
		cllist_unlink(_n);
		free(_n);
		_n = _d->prev;
	}

	oswin_finish_clip();
	
	oswin_background_render(cr);

	for (_n = oswin_window_list.next; _n != &oswin_window_list; _n = _n->next) {
		wnd = (oswin_window_t *) _n;
				
		cairo_get_matrix(cr, &m);
		cairo_translate(cr, wnd->window->dimensions.x, wnd->window->dimensions.y);

		oswin_decorations_render(wnd->session, cr, wnd->window);

		cairo_set_source_surface (cr, wnd->surface, 0, 0);
		cairo_rectangle(cr, 0, 0, wnd->window->dimensions.w, wnd->window->dimensions.h);
		cairo_fill(cr);	
	
		cairo_set_matrix(cr, &m);

	}
	
	oswin_cursor_render(cr, cursr.x, cursr.y);
}

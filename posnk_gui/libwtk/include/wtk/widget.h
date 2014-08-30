#ifndef __WTK_WIDGET_H__
#define __WTK_WIDGET_H__

#include <cairo.h>
#include <clara/clara.h>
#include <clara/cllist.h>
#include <clara/cinput.h>

#define WTK_WIDGET_FLAG_DIRTY	(1<<0)

typedef struct wtk_widget	wtk_widget_t;
typedef struct wtk_callbacks	wtk_callbacks_t;

struct wtk_callbacks {
	void (*paint)(wtk_widget_t *, cairo_t *, int focused);
	void (*do_clip)(wtk_widget_t *, cairo_t *);
	void (*key_down)(wtk_widget_t *, uint32_t keycode, char keychar, int modifiers);
	void (*key_up)(wtk_widget_t *, uint32_t keycode, char keychar, int modifiers);
	void (*key_typed)(wtk_widget_t *, uint32_t keycode, char keychar, int modifiers);
	void (*mouse_down)(wtk_widget_t *, clara_point_t position, int button);
	void (*mouse_up)(wtk_widget_t *, clara_point_t position, int button);
	void (*mouse_move)(wtk_widget_t *, clara_point_t position);
	void (*resize)(wtk_widget_t *);
};

struct wtk_widget {
	cllist_t	 link;
	clara_rect_t	 rect;
	int		 flags;
	wtk_callbacks_t	 callbacks;
	wtk_widget_t	*focused;
	cllist_t	 children;
	void		*impl;
};

void wtk_widget_do_clip(wtk_widget_t *widget, cairo_t *context);

void wtk_widget_render(wtk_widget_t *widget, cairo_t *context, int focused);

void wtk_widget_handle_event(wtk_widget_t *widget, clara_event_msg_t *event);

void wtk_widget_resize(wtk_widget_t *widget, int width, int height);

wtk_widget_t *wtk_create_widget(clara_rect_t rect);

void wtk_widget_redraw(wtk_widget_t *widget);

void wtk_widget_add(wtk_widget_t *parent, wtk_widget_t *widget);

#endif

#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/menubar.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <murrine.h>


void wtk_menubar_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	wtk_menubar_t *menubar = (wtk_menubar_t *) w->impl;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;

	t_w = w->rect.w;
	t_h = w->rect.h;
	murrine_draw_menubar(context, t_w, t_h, menubar->style);
	murrine_draw_menubar_item(context, t_w, t_h, focused);
}

wtk_widget_t *wtk_create_menubar(clara_rect_t rect, int style)
{
	wtk_widget_t *w = wtk_create_widget(rect);
	wtk_menubar_t *menubar = malloc(sizeof(wtk_menubar_t));
	w->callbacks.paint = &wtk_menubar_paint;
	w->impl = menubar;
	menubar->style = style;
	return w;
}

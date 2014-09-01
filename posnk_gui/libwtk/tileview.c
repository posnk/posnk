#include <cairo.h>
#include <clara/clara.h>
#include <wtk/widget.h>
#include <wtk/tileview.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <murrine.h>
#include <linux/input.h>


void wtk_tileview_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	wtk_tileview_t *tileview = (wtk_tileview_t *) w->impl;
	cairo_matrix_t m;
	int t_x, t_y, t_h, t_w;
	int n,c, x, y, n_r, n_c, i_w, i_h, n_s, c_r;

	t_w = w->rect.w;
	t_h = w->rect.h;

	c = tileview->callbacks.get_item_count(w);

	i_w = tileview->tile_w + tileview->gap_x;
	i_h = tileview->tile_h + tileview->gap_y;

	n_r = t_w / i_w;
	n_c = t_h / i_h;

	c_r = 0;
	n_s = 0;

	for (n = 0; n < c; n++) {
		if (tileview->mode == TILEVIEW_HORIZONTAL) {
			if ((n - n_s) == n_r) {
				n_s = n;
				c_r++;
			}
			x = (n - n_s) * i_w;
			y = c_r * i_h;
		} else if (tileview->mode == TILEVIEW_VERTICAL) {
			if ((n - n_s) == n_c) {
				n_s = n;
				c_r++;
			}
			y = (n - n_s) * i_h;
			x = c_r * i_w;
		}
		cairo_get_matrix(context, &m);
		cairo_translate(context, x, y);
		cairo_save(context);
		tileview->callbacks.render_tile(w, context, n, 0);
		cairo_restore(context);
		cairo_set_matrix(context, &m);
	}

	//murrine_draw_textbox(context, t_w, t_h, focused);
}

void wtk_tileview_mouse_down(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_tileview_t *tileview = (wtk_tileview_t *) w->impl;
	cairo_matrix_t m;
	clara_rect_t r;
	int t_x, t_y, t_h, t_w;
	int n,c, n_r, n_c, i_w, i_h, n_s, c_r;

	t_w = w->rect.w;
	t_h = w->rect.h;

	c = tileview->callbacks.get_item_count(w);

	i_w = tileview->tile_w + tileview->gap_x;
	i_h = tileview->tile_h + tileview->gap_y;

	n_r = t_w / i_w;
	n_c = t_h / i_h;

	c_r = 0;
	n_s = 0;

	r.w = i_w;
	r.h = i_h;

	for (n = 0; n < c; n++) {
		if (tileview->mode == TILEVIEW_HORIZONTAL) {
			if ((n - n_s) == n_r) {
				n_s = n;
				c_r++;
			}
			r.x = (n - n_s) * i_w;
			r.y = c_r * i_h;
		} else if (tileview->mode == TILEVIEW_VERTICAL) {
			if ((n - n_s) == n_c) {
				n_s = n;
				c_r++;
			}
			r.y = (n - n_s) * i_h;
			r.x = c_r * i_w;
		}
		if (clara_rect_test(r, position)){
			tileview->callbacks.on_click(w, n);
		}
	}
}

wtk_widget_t *wtk_create_tileview(clara_rect_t rect)
{
	wtk_widget_t *w = wtk_create_widget(rect);
	wtk_tileview_t *tileview = malloc(sizeof(wtk_tileview_t));
	w->callbacks.paint = &wtk_tileview_paint;
	w->callbacks.mouse_down = wtk_tileview_mouse_down;
	w->impl = tileview;
	return w;
}

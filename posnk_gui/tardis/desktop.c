#include <stdlib.h>
#include <assert.h>

#include <clara/clara.h>
#include <clara/cwindow.h>
#include <wtk/window.h>
#include <wtk/widget.h>
#include <wtk/tileview.h>
#include <wtk/resources.h>

#include "tardis.h"

int		 desktop_dirty = 1;

wtk_window_t	*desktop_window;
wtk_widget_t	*desktop_widget;
wtk_widget_t	*desktop_panel_widget;

wtk_widget_t	*desktop_tileview_widget;
int		 desktop_item_count = 3;
desktop_item_t	 desktop_item[3] = {
	{
		"wtkterm",
		"Terminal",
		"/share/icons/term.png"
	},
	{
		"wtktest",
		"WTK Test",
		"/share/icons/wtktest.png"
	},
	{
		"smith osdev.bmp",
		"ImageVwr",
		"/share/icons/imgview.png"
	}
};

cairo_surface_t *desktop_background;

void desktop_paint(wtk_widget_t *w, cairo_t *ctx, int focused) 
{
	cairo_set_source_surface(ctx, desktop_background, 0, 0);
	cairo_paint(ctx);
}

void desktop_do_clip(wtk_widget_t *w, cairo_t *ctx) 
{
	if (desktop_dirty) {
		desktop_dirty = 0;
		cairo_rectangle(ctx, 0, 0, w->rect.w, w->rect.h);
	}
}

int desktop_tv_get_item_count(wtk_widget_t *w)
{
	return desktop_item_count;
}

void desktop_tv_render_item(wtk_widget_t *w, cairo_t *context, int n, int selected)
{
	wtk_tileview_t *tileview = (wtk_tileview_t *) w->impl;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w, i_x, i_y, i_w, i_h;

	t_w = tileview->tile_w;
	t_h = w->rect.h;
	
	i_w = cairo_image_surface_get_width(desktop_item[n].icon_surface);
	i_h = cairo_image_surface_get_height(desktop_item[n].icon_surface);
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, desktop_item[n].name, &te);

	t_x = (t_w / 2) - (te.width / 2);
	t_y = 68 + (16 / 2) - (te.height / 2);

	i_x = (t_w / 2) - (i_w / 2);
	i_y = 10;

	if (selected) {
		cairo_rectangle(context, 0, 2, te.width + 10, t_h - 4);	
		cairo_set_source_rgb(context, 0.3, 0.3, 1);
		cairo_fill(context);
	}
	
	cairo_rectangle(context, i_x, i_y, i_w, i_h);	
	cairo_set_source_surface(context, desktop_item[n].icon_surface, i_x, i_y);
	cairo_fill(context);

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, desktop_item[n].name);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, desktop_item[n].name);
}

void desktop_tv_on_click(wtk_widget_t *w, int n)
{
	pid_t pid = fork();
	if (pid != 0)
		return;
	system(desktop_item[n].cmd);
}

void desktop_load_icons()
{
	int n;
	for (n = 0; n < desktop_item_count; n++) {
		desktop_item[n].icon_surface = cairo_image_surface_create_from_png (desktop_item[n].icon);
	}
}

void desktop_initialize(const char * bg_path)
{
	wtk_tileview_t *tileview;
	clara_rect_t d = clara_get_screen_dims();	
	clara_rect_t tv_rect = {0,0, d.w, d.h};

	desktop_background = cairo_image_surface_create_from_png (bg_path);
	assert (desktop_background != NULL);

	desktop_window = wtk_window_create(d.w, d.h, CLARA_WIN_FLAG_NOFRONT | CLARA_WIN_FLAG_UNDECORATED | CLARA_WIN_FLAG_NOLIST, "TARDIS (Clara Desktop)");
	assert (desktop_window != NULL);

	desktop_widget = wtk_create_widget(d);
	assert (desktop_widget != NULL);

	desktop_widget->callbacks.paint = &desktop_paint;
	desktop_widget->callbacks.do_clip = &desktop_do_clip;

	wtk_widget_add(desktop_window->widget, desktop_widget);
	
	desktop_panel_widget = panel_create();

	wtk_widget_add(desktop_widget, desktop_panel_widget);
	
	panel_add_widget(tasklist_create());
	
	panel_add_widget(clock_create());
	
	panel_add_widget(userwidg_create());

	tv_rect.y += desktop_panel_widget->rect.h;
	tv_rect.h -= desktop_panel_widget->rect.h;

	desktop_tileview_widget = wtk_create_tileview(tv_rect);
	tileview = (wtk_tileview_t *) desktop_tileview_widget->impl;

	tileview->callbacks.get_item_count = &desktop_tv_get_item_count;
	tileview->callbacks.render_tile = &desktop_tv_render_item;
	tileview->callbacks.on_click = &desktop_tv_on_click;
	tileview->tile_w = 100;
	tileview->tile_h = 100;
	tileview->gap_x = 0;
	tileview->gap_y = 0;
	tileview->mode = TILEVIEW_VERTICAL;

	wtk_widget_add(desktop_widget, desktop_tileview_widget);

	desktop_load_icons();	
}

void desktop_process()
{
	panel_process();
	wtk_window_process(desktop_window);
}

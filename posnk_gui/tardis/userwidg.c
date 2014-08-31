#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <cairo.h>

#include <wtk/widget.h>
#include <wtk/resources.h>

#include "tardis.h"

char *user_full_name;

void userwidg_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;

	t_w = w->rect.w;
	t_h = w->rect.h;
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, user_full_name, &te);

	t_x = (t_w / 2) - (te.width / 2);
	t_y = (t_h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, user_full_name);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, user_full_name);
}

void userwidg_setup()
{
	char *text;
	struct passwd *pwent = getpwuid(getuid());
	
	text = pwent->pw_gecos;

	user_full_name = malloc (strlen(text) + 1);
	strcpy ( user_full_name, text );
}

panel_widget_t *userwidg_create()
{
	clara_rect_t rect = {0, 0, 200 , 0};

	panel_widget_t *w = malloc(sizeof(panel_widget_t));

	userwidg_setup();

	w->w_mode = PANEL_W_MODE_FIXED;

	w->widget = wtk_create_widget(rect);

	w->widget->callbacks.paint = &userwidg_paint;

	w->process = NULL;

	return w;
}

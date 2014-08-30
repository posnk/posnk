#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/textbox.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <murrine.h>
#include <linux/input.h>


void wtk_textbox_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	wtk_textbox_t *textbox = (wtk_textbox_t *) w->impl;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;

	t_w = w->rect.w;
	t_h = w->rect.h;
	murrine_draw_textbox(context, t_w, t_h, focused);

	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, textbox->text, &te);

	t_x = 5;
	t_y = (t_h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, textbox->text);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, textbox->text);

	cairo_rectangle(context, t_x + te.width + 2.5, 5, 1, t_h - 10);
	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_fill(context);
}

void wtk_textbox_key_typed(wtk_widget_t *w, uint32_t keycode, char keychar)
{
	wtk_textbox_t *textbox = (wtk_textbox_t *) w->impl;
	if (strlen(textbox->text) > 159)
		return;
	sprintf(textbox->text, "%s%c", textbox->text, keychar);
	wtk_widget_redraw(w);
}

void wtk_textbox_key_down(wtk_widget_t *w, uint32_t keycode, char keychar)
{
	int len;
	wtk_textbox_t *textbox = (wtk_textbox_t *) w->impl;
	if (keycode == KEY_BACKSPACE) {
		len = strlen(textbox->text);
		if (len <= 0)
			return;
		textbox->text[len - 1] = 0;
	}
	wtk_widget_redraw(w);

}

wtk_widget_t *wtk_create_textbox(clara_rect_t rect, char *text, void (*typed)(wtk_widget_t *))
{
	wtk_widget_t *w = wtk_create_widget(rect);
	wtk_textbox_t *textbox = malloc(sizeof(wtk_textbox_t));
	w->callbacks.paint = &wtk_textbox_paint;
	w->callbacks.key_typed = wtk_textbox_key_typed;
	w->callbacks.key_down = wtk_textbox_key_down;
	w->impl = textbox;
	//button->callbacks.typed = clicked;
	textbox->text = malloc(160);
	strcpy(textbox->text, text);
	return w;
}

#ifndef __WTK_TEXTBOX_H__
#define __WTK_TEXTBOX_H__

#include <wtk/widget.h>

typedef struct {
} wtk_textbox_callbacks_t;

typedef struct {
	char			*text;
	wtk_textbox_callbacks_t	 callbacks;
	void			*impl;
} wtk_textbox_t;

wtk_widget_t *wtk_create_textbox(clara_rect_t rect, char *text, void (*typed)(wtk_widget_t *));

#endif

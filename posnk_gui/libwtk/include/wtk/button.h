#ifndef __WTK_BUTTON_H__
#define __WTK_BUTTON_H__

#include <wtk/widget.h>

typedef struct {
	void (*clicked)(wtk_widget_t *);
} wtk_button_callbacks_t;

typedef struct {
	int			 state;
	char			*text;
	wtk_button_callbacks_t	 callbacks;
	void			*impl;
} wtk_button_t;

wtk_widget_t *wtk_create_button(clara_rect_t rect, char *text, void (*clicked)(wtk_widget_t *));

#endif

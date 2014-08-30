#ifndef __WTK_MENUBAR_H__
#define __WTK_MENUBAR_H__

#include <wtk/widget.h>

typedef struct {
} wtk_menubar_callbacks_t;

typedef struct {
	char			*text;
	wtk_menubar_callbacks_t	 callbacks;
	int			 style;
	void			*impl;
} wtk_menubar_t;

wtk_widget_t *wtk_create_menubar(clara_rect_t rect, int style);

#endif

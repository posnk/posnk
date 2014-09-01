#ifndef __WTK_TILEVIEW_H__
#define __WTK_TILEVIEW_H__

#include <wtk/widget.h>
#include <cairo.h>

#define TILEVIEW_HORIZONTAL	(0)
#define TILEVIEW_VERTICAL	(1)

typedef struct {
	void	(*render_tile)(wtk_widget_t *,cairo_t *, int, int); //widget, context, n, s
	void	(*on_click)(wtk_widget_t *, int);
	int	(*get_item_count)(wtk_widget_t *);
} wtk_tileview_callbacks_t;

typedef struct {
	int				 tile_w;
	int				 tile_h;
	int				 gap_x;
	int				 gap_y;
	int				 mode;
	wtk_tileview_callbacks_t	 callbacks;
	void				*impl;
} wtk_tileview_t;

wtk_widget_t *wtk_create_tileview(clara_rect_t rect);

#endif

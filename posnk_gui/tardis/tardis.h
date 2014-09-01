#ifndef __TARDIS_H__
#define __TARDIS_H__
#include <clara/cllist.h>
void desktop_initialize(const char * bg_path);

void desktop_process();


#define PANEL_W_MODE_FILL	(0)
#define PANEL_W_MODE_FIXED	(1) 

typedef struct panel_widget panel_widget_t;

struct panel_widget {
	cllist_t	 link;
	int		 w_mode;
	wtk_widget_t	*widget;
	void 		(*process)(panel_widget_t *);	
	void		*impl;
};

typedef struct {
	char		*cmd;
	char		*name;
	char		*icon;
	cairo_surface_t *icon_surface;
} desktop_item_t;
 
wtk_widget_t *panel_create();

void panel_add_widget(panel_widget_t *widget);

void panel_process();

panel_widget_t *userwidg_create();

panel_widget_t *clock_create();

panel_widget_t *spacer_create();

panel_widget_t *tasklist_create();

#endif

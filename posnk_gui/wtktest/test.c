#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <unistd.h>
#include <wtk/window.h>
#include <wtk/widget.h>
#include <wtk/button.h>
#include <wtk/textbox.h>
#include <wtk/menubar.h>

int main(int argc, char **argv)
{
	clara_rect_t dims1 = {80,80,150,40};
	clara_rect_t dims2 = {80,160,150,40};
	clara_rect_t dimt = {80,210,150,30};
	clara_rect_t dimm = {0,0,300,24};
	wtk_window_t *window;
	wtk_widget_t *button1;
	wtk_widget_t *button2;
	wtk_widget_t *text;

	clara_init_client("/oswdisp");	
	
	window = wtk_window_create(300, 300, 0, "WTK Test Application");
	button1 = wtk_create_button(dims1, "Test Button 1", NULL);
	button2 = wtk_create_button(dims2, "Test Button 2", NULL);
	text = wtk_create_textbox(dimt, "textbox", NULL);
	wtk_widget_add(window->widget, button1);	
	wtk_widget_add(window->widget, button2);	
	wtk_widget_add(window->widget, text);		
	wtk_widget_add(window->widget, wtk_create_menubar(dimm, 0));
	dimm.y += 24;
	wtk_widget_add(window->widget, wtk_create_menubar(dimm, 1));
	dimm.y += 24;
	wtk_widget_add(window->widget, wtk_create_menubar(dimm, 2));
	dimm.y += 24;
	wtk_widget_add(window->widget, wtk_create_menubar(dimm, 3));

	while (1) {
		wtk_window_process(window);
		usleep(999);
	}

	clara_exit_client();
	return 0;
}

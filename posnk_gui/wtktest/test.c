#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <unistd.h>
#include <wtk/window.h>
#include <wtk/widget.h>
#include <wtk/button.h>

int main(int argc, char **argv)
{
	clara_rect_t dims = {80,80,150,40};
	wtk_window_t *window;
	wtk_widget_t *button;

	clara_init_client("/oswdisp");	
	
	window = wtk_window_create(300, 300, 0, "WTK Test Application");
	button = wtk_create_button(dims, "Test Button", NULL);
	wtk_widget_add(window->widget, button);	

	while (1) {
		wtk_window_process(window);
		usleep(999);
	}

	clara_exit_client();
	return 0;
}

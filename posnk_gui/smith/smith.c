#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>

int main(int argc, char **argv)
{
	int st;
	uint32_t handle;
	clara_surface_t *s;
	clara_rect_t dims = {80,80,100,100};
	clara_init_client("/oswdisp");	
	handle = (uint32_t) clara_create_window();
	if (handle == (uint32_t) -1) {
		printf("couldn't create window: %s", strerror(errno));
	}

	st = clara_init_window(handle, "libclara test app <smith>", dims, 0, 100, 100);
	if (st == -1){
		printf("couldn't init window: %s", strerror(errno));
	}
	dims.x = 0;
	dims.y = 0;
	while (1) {
		s = clara_window_get_surface(handle);
		clara_draw_rect(s, 10, 10, 10, 10, 0xFF00FF);
		free(s);
		clara_window_add_damage(dims);
	}
	clara_exit_client();
	return 0;
}

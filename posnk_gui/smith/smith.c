#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <unistd.h>
#include <bitmap.h>

int main(int argc, char **argv)
{
	int st;
	uint32_t handle;
	clara_surface_t *s;
	bitmap_image_t *img = load_bmp(argv[1]);
	clara_rect_t dims = {80,80,img->width,img->height};
	clara_init_client("/oswdisp");	
	handle = (uint32_t) clara_create_window();
	if (handle == (uint32_t) -1) {
		printf("couldn't create window: %s", strerror(errno));
	}

	st = clara_init_window(handle, argv[1], dims, 0, img->width,img->height);
	if (st == -1){
		printf("couldn't init window: %s", strerror(errno));
	}
	dims.x = 0;
	dims.y = 0;
	s = clara_window_get_surface(handle);
	clara_draw_image(s, 0,0,img);
	free(s);
	clara_window_add_damage(handle, dims);
	while (1) {
		usleep(99999);
	}
	clara_exit_client();
	return 0;
}

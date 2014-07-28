/**
 * getty.c
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include "osession.h"
#include "odisplay.h"
#include "ofbdev.h"
#include "omsg.h"
#include "ovideo.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <clara/clara.h>

void usage(){
}

int main(int argc, char *argv[], char *envp[]){
	clara_surface_t *s;
	fprintf(stderr, "info: oswin 0.01 by Peter Bosch\n");
	if (oswin_fbdev_init("/dev/fb0") == -1)
		return 255;
	if (oswin_display_init("/oswdisp") == -1)
		return 255;
	oswin_session_init();
	
	while(oswin_display_poll() != -1) {
		oswin_session_process();
		s = oswin_get_surface();
		oswin_session_render(s);
		oswin_free_surface(s);
		oswin_flip_buffers();
		usleep(9000);
	}
	return 0;
}


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

#include "graphics.h"
#include "window.h"
#include "wserv.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage(){
}

int main(int argc, char *argv[], char *envp[]){
	printf("info: wserv 0.01 by Peter Bosch\n");
	if (!open_fb())
		return 255;
	if (!fifo_init())
		return 255;
	window_init();
	while(fifo_loop()) {
		window_render_all();
		flip_fb();
		usleep(9000);
	}
	close_fb();
	return 0;
}


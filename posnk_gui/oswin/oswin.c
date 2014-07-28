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
#include "omsg.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage(){
}

int main(int argc, char *argv[], char *envp[]){
	fprintf(stderr, "info: oswin 0.01 by Peter Bosch\n");
	if (oswin_display_init("/oswdisp") == -1)
		return 255;
	oswin_session_init();
	while(oswin_display_poll() != -1) {
		oswin_session_process();
		usleep(9000);
	}
	return 0;
}


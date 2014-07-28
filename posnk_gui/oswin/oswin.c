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
#include "oinput.h"
#include "odecor.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <clara/clara.h>
#include <clara/cinput.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

static struct termios orig_termios;  /* TERMinal I/O Structure */
static int ttyfd = STDIN_FILENO;     /* STDIN_FILENO is 0 by default */

void tty_raw(void)
   {
    struct termios raw;

    raw = orig_termios;  /* copy original and then modify below */

    /* input modes - clear indicated ones giving: no break, no CR to NL, 
       no parity check, no strip char, no start/stop output (sic) control */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* output modes - clear giving: no post processing such as NL to CR+NL */
    raw.c_oflag &= ~(OPOST);

    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);

    /* local modes - clear giving: echoing off, canonical off (no erase with 
       backspace, ^U,...),  no extended functions, no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /* control chars - set return condition: min number of bytes and timer */
    raw.c_cc[VMIN] = 5; raw.c_cc[VTIME] = 8; /* after 5 bytes or .8 seconds
                                                after first byte seen      */
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0; /* immediate - anything       */
    raw.c_cc[VMIN] = 2; raw.c_cc[VTIME] = 0; /* after two bytes, no timer  */
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 8; /* after a byte or .8 seconds */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(ttyfd,TCSAFLUSH,&raw) < 0) fprintf(stderr,"can't set raw mode");
	fcntl(0, F_SETFL, O_NONBLOCK);
   }

void usage(){
}

int stdio_input_test()
{
	clara_event_msg_t msg;
	char in;
int nr;
a:
	nr = read(0, &in, sizeof(char));
	if ((nr == -1) && (errno == EAGAIN))
		return 0;
	if (nr < sizeof(char)) {
		fprintf(stderr, "error: could not get keystrokes\n");
		return 0;
	}

	switch( in) {
		case 'w':
			msg.flags = CLARA_EVENT_FLAG_FOCUS | CLARA_EVENT_FLAG_PNT_DELTA;
			msg.ptr.x = 0;
			msg.ptr.y = -5;
			oswin_input_send(&msg);
			break;
		case 's':
			msg.flags = CLARA_EVENT_FLAG_FOCUS | CLARA_EVENT_FLAG_PNT_DELTA;
			msg.ptr.x = 0;
			msg.ptr.y = 5;
			oswin_input_send(&msg);
			break;
		case 'a':
			msg.flags = CLARA_EVENT_FLAG_FOCUS | CLARA_EVENT_FLAG_PNT_DELTA;
			msg.ptr.x = 5;
			msg.ptr.y = 0;
			oswin_input_send(&msg);
			break;
		case 'd':
			msg.flags = CLARA_EVENT_FLAG_FOCUS | CLARA_EVENT_FLAG_PNT_DELTA;
			msg.ptr.x = -5;
			msg.ptr.y = 0;
			oswin_input_send(&msg);
			break;
		
		default:
			break;
	}

			goto a;
}

int main(int argc, char *argv[], char *envp[]){
	bitmap_image_t *bg = load_bmp("/root/dsotm.bmp");
	clara_surface_t *s;
	fprintf(stderr, "info: oswin 0.01 by Peter Bosch\n");
	if (oswin_fbdev_init("/dev/fb0") == -1)
		return 255;
	if (oswin_display_init("/oswdisp") == -1)
		return 255;
	if (oswin_input_init() == -1)
		return 255;
	oswin_decorator_init();
	oswin_session_init();
	tty_raw();
	while(oswin_display_poll() != -1) {
		oswin_session_process();
		stdio_input_test();
		oswin_input_process();
		s = oswin_get_surface();
		clara_draw_image(s,0,0,bg);
		oswin_session_render(s);
		oswin_cursor_render(s);
		oswin_free_surface(s);
		oswin_flip_buffers();
		usleep(900);
	}
	return 0;
}


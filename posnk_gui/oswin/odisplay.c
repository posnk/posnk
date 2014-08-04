#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h> //For error reporting
#include <string.h> //For memory functions and error reporting
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
/* mknod
 * open
 * read
 * close
 */
#include <unistd.h>
#include <clara/cllist.h>
#include <clara/cmsg.h>
#include <clara/csession.h>
#include "osession.h"

int oswin_display_fd = -1;

int oswin_display_init(const char *disp_path)
{
	mknod(disp_path, S_IFIFO | 0666, 0);
	fprintf(stderr, "info: opening display %s...\n", disp_path);
	oswin_display_fd = open(disp_path, O_RDONLY | O_NONBLOCK);
	if (oswin_display_fd == -1) {
		fprintf(stderr, "fatal: error opening %s: %s\n", disp_path, strerror(errno));
		return -1;
	}
	return 0;
}

int oswin_display_poll()
{
	uint32_t buf;
	ssize_t nr;
	do {
		nr = read(oswin_display_fd, &buf, sizeof(uint32_t));
		if (nr == -1) {
			if (errno == EAGAIN)
				return 0;
			printf("fatal: error reading display: %s\n", strerror(errno));
			return -1;			
		} else if (nr == sizeof(uint32_t))
			oswin_session_accept(buf);
	} while (nr);
	return 0;
}

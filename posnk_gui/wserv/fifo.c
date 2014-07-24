#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "window.h"

int fifo_fd;

int fifo_init()
{
	mknod("/wservsock", S_IFIFO | 0666, 0);
	printf("info: opening fifo...\n");
	fifo_fd = open("/wservsock", O_RDONLY | O_NONBLOCK);
	if (fifo_fd == -1) {
		printf("fatal: error opening fifo: %s\n", strerror(errno));
		return 0;
	}
	return 1;
}

int fifo_loop()
{
	int buf;
	int nr = read(fifo_fd, &buf, sizeof(int));
	if (nr == -1) {
		if (errno == EAGAIN)
			return 1;
		printf("fatal: error reading fifo: %s\n", strerror(errno));
		return 0;			
	}
	if (nr != 0) {
		printf("info: fifo recv (%i %i)\n",buf,nr);
		window_register(buf);
	}
	return 1;
}

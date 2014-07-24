#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <unistd.h>
#include "window.h"


wlib_window_t *window_open(posgui_window_t *w)
{
	int nw;
	int fifo_fd;
	posgui_window_t *wn;
	wlib_window_t *ww;
	int id = shmget(IPC_PRIVATE, sizeof(posgui_window_t), 0666 | IPC_CREAT);
	if (id == -1) {
		printf("Failed to create ctlshm :%s\n", strerror(errno));
		return NULL;
	}
	wn = shmat(id, NULL, 0);
	if (wn == (void *)-1) {
		printf("Failed to attach ctlshm :%s\n", strerror(errno));
		return NULL;
	}
	memcpy(wn, w, sizeof(posgui_window_t));
	free(w);
	wn->magic = WINDOW_MAGIC;
	wn->pixbuf = shmget(IPC_PRIVATE, sizeof(uint32_t) * wn->width * wn->height, 0666 | IPC_CREAT);
	if (wn->pixbuf == -1) {
		printf("Failed to create pixshm :%s\n", strerror(errno));
		return NULL;
	}
	ww = malloc (sizeof(wlib_window_t));
	ww->winfo = wn;
	ww->ctlbuf = id;
	ww->pixels = shmat(wn->pixbuf, NULL, 0);
	if (ww->pixels == (void *)-1) {
		printf("Failed to attach pixshm :%s\n", strerror(errno));
		return NULL;
	}
	fifo_fd = open("/wservsock", O_WRONLY);
	if (fifo_fd == -1) {
		printf("error opening fifo: %s\n", strerror(errno));
		return NULL;
	}
	nw = write(fifo_fd, &id, sizeof(int));
	if (nw == -1) {
		printf("error writing to fifo: %s\n", strerror(errno));
		return NULL;
	}
	if (nw != sizeof(int)) {
		printf("error: did not write full data to fifo\n");
	}
	close(fifo_fd);
	return ww;
	
}

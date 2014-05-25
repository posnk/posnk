/**
 * tty.c
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int tty_open(char *path)
{
	int res;
	int o_fd = open(path, O_RDWR);
	if (o_fd == -1)
		return 0;
	res = dup2(o_fd, 0);
	if (res == -1) {
		close(o_fd);
		return 0;
	}
	res = dup2(o_fd, 1);
	if (res == -1) {
		close(o_fd);
		return 0;
	}
	res = dup2(o_fd, 2);
	if (res == -1) {
		close(o_fd);
		return 0;
	}
	ioctl(0, TIOCSCTTY, 1);
	return 1;
}

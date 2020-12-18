/**
 * sys/ioctl.h
 *
 * Part of P-OS.
 *
 * Except where otherwise specified, this file is POSIX compliant.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 15-05-2014 - Created
 */

#ifndef __SYS_IOCTL_H__
#define __SYS_IOCTL_H__

#include <sys/types.h>

/* tty device ioctls */

/* IOCTL call number macros */

#define IOCTL_TCDRAIN	(1)

/* Linux-compatible termios API */

#define TCXONC		(2)
#define TCFLSH		(3)
#define TCGETS		(4)
#define TCSETS		(5)
#define TCSETSF		(6)
#define TCSETSW		(7)
#define TCSBRK		(8)

/* Linux-compatible job control API */

#define TIOCGPGRP	(9)
#define TIOCSPGRP	(10)
#define TIOCGSID	(11)

/* Linux-compatible ctty API */

#define TIOCSCTTY	(12)
#define TIOCNOTTY	(13)

/* Linux-compatible winsize API */

#define TIOCGWINSZ	(14)
#define TIOCSWINSZ	(15)

/* IOCTL parameter structures */

/* Linux-compatible winsize API */

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_x;
	unsigned short ws_y;
};

/* fb device ioctls */

#define IOCTL_FBGDINFO	(16)
#define IOCTL_FBGMINFO	(17)
#define IOCTL_FBSMINFO	(18)

typedef struct fb_device_info {
	size_t		fb_size;
	int		fb_flags;
} fb_device_info_t;

typedef struct fb_mode_info {
	unsigned short	fb_width;
	unsigned short	fb_height;
	unsigned short	fb_stride;
	unsigned char	fb_bpp;
} fb_mode_info_t;


/* event device ioctls */

#define EVIOCGVERSION		(19)
#define EVIOCGID		(20)
#define EVIOCGREP		(21)
#define EVIOCSREP		(22)
#define EVIOCGKEYCODE		(23)
#define EVIOCSKEYCODE		(24)
#define EVIOCGKEY		(25)
#define EVIOCGNAME(len)		(26)

#define EVIOCGBIT(ev,len)	(27)
#define EVIOCGABS(abs)		(28)

#ifdef __cplusplus
extern "C" {
#endif
int ioctl(int fildes, int request, void *arg);
#ifdef __cplusplus
}
#endif

#endif

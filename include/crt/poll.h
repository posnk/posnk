/* 
 * crt/poll.h
 *
 * Part of P-OS kernel and libc.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 10-05-2017 - Created
 */

#ifndef __poll_h__
#define __poll_h__

struct pollfd {
	/** the following descriptor being polled */
	int			fd;
	/** the input event flags */
	short int	events;
	/** the output event flags */
	short int	revents;
};

/** Data on priority band 0 may be read. */
#define	POLLRDNORM	(1<<1)
/** Data on priority bands greater than 0 may be read. */
#define POLLRDBAND	(1<<2)
/** Same effect as POLLRDNORM | POLLRDBAND. */
#define	POLLIN		(POLLRDNORM | POLLRDBAND)
/** High priority data may be read. */
#define POLLPRI		(1<<3)
/** Data on priority band 0 may be written. */
#define POLLWRNORM	(1<<5)
/** Data on priority bands greater than 0 may be written. 
  * This event only examines bands that have been written to at least once. */
#define POLLWRBAND	(1<<6)
/** Same value as POLLWRNORM. */
#define	POLLOUT		(POLLWRNORM)
/** An error has occurred (revents only). */
#define POLLERR		(1<<7)
/** Device has been disconnected (revents only). */
#define POLLHUP		(1<<8)
/** Invalid fd member (revents only). */
#define POLLNVAL	(1<<9)

/** Type describing the count of file descriptors */
typedef int nfds_t;

int poll( struct pollfd[], nfds_t, int );

#endif


/**
 * sys/termios.h
 *
 * Part of P-OS.
 *
 * Except where otherwise specified, this file is POSIX compliant,
 * for now there is no support for the SysV termio API, but that
 * might be added later on, should the need arise.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 08-05-2014 - Created
 */

#ifndef __SYS_TERMIOS_H__
#define __SYS_TERMIOS_H__

#include <sys/types.h>

/* Definitions for function calls */

/* Definitions for tcsetattr */

#define TCSANOW		(0)
#define TCSADRAIN	(1)
#define TCSAFLUSH	(2)

/* Definitions for tcflush */

#define	TCIFLUSH	(1)
#define	TCIOFLUSH	(3)
#define	TCOFLUSH	(2)

/* Definitions for tcflow */

#define TCIOFF		(0)
#define TCION		(1)
#define TCOOFF		(2)
#define TCOON		(3)

/* Definitions for termios struct */

/* Definitions for c_iflag */

#define BRKINT	(1<<0)
#define ICRNL	(1<<1)
#define IGNBRK	(1<<2)
#define IGNCR	(1<<3)
#define IGNPAR	(1<<4)
#define INLCR	(1<<5)
#define INPCK	(1<<6)
#define ISTRIP	(1<<7)
#define IUCLC	(1<<8)
#define IXANY	(1<<9)
#define IXOFF	(1<<10)
#define IXON	(1<<11)
#define PARMRK	(1<<12)

/* Definitions for c_oflag */

#define OPOST	(1<<0)
#define OLCUC	(1<<1)
#define ONLCR	(1<<2)
#define OCRNL	(1<<3)
#define ONLRET	(1<<4)
#define OFILL	(1<<5)
#define NLDLY	(1<<6)
#define NL0	(0<<6)
#define NL1	(1<<6)
#define CRDLY	(3<<7)
#define CR0	(0<<7)
#define CR1	(1<<7)
#define CR2	(2<<7)
#define CR3	(3<<7)
#define TABDLY	(3<<9)
#define TAB0	(0<<9)
#define TAB1	(1<<9)
#define TAB2	(2<<9)
#define TAB3	(3<<9)
#define BSDLY	(1<<11)
#define BS0	(0<<11)
#define BS1	(1<<11)
#define VTDLY	(1<<12)
#define VT0	(0<<12)
#define VT1	(1<<12)
#define FFDLY	(1<<13)
#define FF0	(0<<13)
#define FF1	(1<<13)

/* Definitions for c_*speed */

#define B0	    (0)
#define B50	    (1)
#define B75	    (2)
#define B110	(3)
#define B134	(4)
#define B150	(5)
#define B200	(6)
#define B300	(7)
#define B600	(8)
#define B1200	(9)
#define B1800	(10)
#define B2400	(11)
#define B4800	(12)
#define B9600	(13)
#define B19200	(14)
#define B38400	(15)
#define B57600	(16)
#define B115200	(17)

/* Definitions for c_cflag */

#define CSIZE	(3<<0)
#define CS5     (0<<0)
#define CS6     (1<<0)
#define CS7     (2<<0)
#define CS8     (3<<0)
#define CSTOPB	(1<<2)
#define CREAD	(1<<3)
#define PARENB	(1<<4)
#define PARODD	(1<<5)
#define HUPCL	(1<<6)
#define CLOCAL	(1<<7)
#define CRTSCTS (1<<8)

/* Definitions for c_lflag */

#define ECHO	(1<<0)
#define ECHOE	(1<<1)
#define ECHOK	(1<<2)
#define ECHONL	(1<<3)
#define ICANON	(1<<4)
#define IEXTEN	(1<<5)
#define ISIG	(1<<6)
#define NOFLSH	(1<<7)
#define TOSTOP	(1<<8)
#define XCASE	(1<<9)

/* Definitions for c_cc */

#define NCCS	(11)

#define	VEOF	(0)
#define	VEOL	(1)
#define	VERASE	(2)
#define	VINTR	(3)
#define	VKILL	(4)
#define	VMIN	(5)
#define	VQUIT	(6)
#define	VSTART	(7)
#define	VSTOP	(8)
#define	VSUSP	(9)
#define	VTIME	(10)

/* Typedefs */

typedef unsigned char	cc_t;
typedef unsigned short	tcflag_t;
typedef unsigned short	speed_t;

typedef struct termios termios_t;//NOT POSIX

/* Structs */

struct termios {
	tcflag_t	c_iflag;
	tcflag_t	c_oflag;
	tcflag_t	c_cflag;
	tcflag_t	c_lflag;
	cc_t		c_cc[NCCS];

	speed_t		c_ispeed;//SEMI POSIX
	speed_t		c_ospeed;//SEMI POSIX

	int		c_addopt;//NOT POSIX, REQUIRED FOR IOCTL
};

/* Function definitions */


#ifdef __cplusplus
extern "C" {
#endif

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);
int     tcdrain(int);
int     tcflow(int, int);
int     tcflush(int, int);
int     tcgetattr(int, struct termios *);
pid_t   tcgetsid(int);
int     tcsendbreak(int, int);
int     tcsetattr(int, int, struct termios *);

#ifdef __cplusplus
}
#endif

#endif

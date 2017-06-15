/*
 * glib.h
 *
 * GLIB stub for Stellaris
 *  Created on: 22 jan. 2014
 *      Author: Peter Bosch
 */

#ifndef GLIB_H_
#define GLIB_H_

#define NCURSES_BITS(mask,shift)	((mask)<<((shift)+8))
#define A_NORMAL	0x07L
#define A_COLOR		0xFFL
#define A_REVERSE	NCURSES_BITS(1UL,10)
#define A_BLINK		NCURSES_BITS(1UL,11)
#define A_DIM		NCURSES_BITS(1UL,12)
#define A_BOLD		NCURSES_BITS(1UL,13)
#define A_ALTCHARSET	NCURSES_BITS(1UL,14)
#define A_INVIS		NCURSES_BITS(1UL,15)
#define A_PROTECT	NCURSES_BITS(1UL,16)
#define A_HORIZONTAL	NCURSES_BITS(1UL,17)
#define A_LEFT		NCURSES_BITS(1UL,18)
#define A_LOW		NCURSES_BITS(1UL,19)
#define A_RIGHT		NCURSES_BITS(1UL,20)
#define A_TOP		NCURSES_BITS(1UL,21)
#define A_VERTICAL	NCURSES_BITS(1UL,22)
#define A_STANDOUT	NCURSES_BITS(1UL,8)
#define A_UNDERLINE	NCURSES_BITS(1UL,9)
#define A_ATTRIBUTES	NCURSES_BITS(~(1UL-1UL),0)

#define TRUE   (-1)
#define FALSE  (0)

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
typedef int    gint32;
typedef gint   gboolean;
typedef gint   bool;
typedef gchar  chtype;
typedef gint   ssize_t;

typedef unsigned char	guchar;
typedef unsigned short	gushort;
typedef unsigned long	gulong;
typedef unsigned int	guint;
typedef unsigned int	guint32;

typedef float	gfloat;
typedef double	gdouble;


/* HAVE_LONG_DOUBLE doesn't work correctly on all platforms.
 * Since gldouble isn't used anywhere, just disable it for now */

#if 0
#ifdef HAVE_LONG_DOUBLE
typedef long double gldouble;
#else /* HAVE_LONG_DOUBLE */
typedef double gldouble;
#endif /* HAVE_LONG_DOUBLE */
#endif /* 0 */

typedef void* gpointer;
typedef const void *gconstpointer;


typedef gint32	gssize;
typedef guint32 gsize;
typedef guint32 GQuark;
typedef gint32	GTime;


#endif /* GLIB_H_ */

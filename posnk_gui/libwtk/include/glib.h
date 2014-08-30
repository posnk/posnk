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

/* First function key (block of 64 follow) */
#define    KEY_F0         0x108
/* Function defining other function key values*/
#define    KEY_F(n)       (KEY_F0+(n))

/* First function key (block of 64 follow) */
#define    KEY_VT0         0633
/* Function defining other function key values*/
#define    KEY_VT(n)       (KEY_VT0+(n))

#define KEY_CODE_YES	0400
#define KEY_BREAK		0401
#define KEY_MIN			0401
#define KEY_DOWN		0402
#define KEY_UP			0403
#define KEY_LEFT		0404
#define KEY_RIGHT		0405
#define KEY_HOME		0406
#define KEY_BACKSPACE	0407
#define KEY_DL			0510
#define KEY_IL			0511
#define KEY_DC			0512
#define KEY_IC			0513
#define KEY_EIC			0514
#define KEY_CLEAR		0515
#define KEY_EOS			0516
#define KEY_EOL			0517
#define KEY_SF			0520
#define KEY_SR			0521
#define KEY_NPAGE		0522
#define KEY_PPAGE		0523
#define KEY_STAB		0524
#define KEY_CTAB		0525
#define KEY_CATAB		0526
#define KEY_ENTER		0527
#define KEY_SRESET		0530
#define KEY_RESET		0531
#define KEY_PRINT		0532
#define KEY_LL			0533
#define KEY_A1			0534
#define KEY_A3			0535
#define KEY_B2			0536
#define KEY_C1			0537
#define KEY_C3			0540
#define KEY_BTAB		0541
#define KEY_BEG			0542
#define KEY_CANCEL		0543
#define KEY_CLOSE		0544
#define KEY_COMMAND		0545
#define KEY_COPY		0546
#define KEY_CREATE		0547
#define KEY_END			0550
#define KEY_EXIT		0551
#define KEY_FIND		0552
#define KEY_HELP		0553
#define KEY_MARK		0554
#define KEY_MESSAGE		0555
#define KEY_MOVE		0556
#define KEY_NEXT		0557
#define KEY_OPEN		0560
#define KEY_OPTIONS		0561
#define KEY_PREVIOUS	0562
#define KEY_REDO		0563
#define KEY_REFERENCE	0564
#define KEY_REFRESH		0565
#define KEY_REPLACE		0566
#define KEY_RESTART		0567
#define KEY_RESUME		0570
#define KEY_SAVE		0571
#define KEY_SBEG		0572
#define KEY_SCANCEL		0573
#define KEY_SCOMMAND	0574
#define KEY_SCOPY		0575
#define KEY_SCREATE		0576
#define KEY_SDC			0577
#define KEY_SDL			0600
#define KEY_SELECT		0601
#define KEY_SEND		0602
#define KEY_SEOL		0603
#define KEY_SEXIT		0604
#define KEY_SFIND		0605
#define KEY_SHELP		0606
#define KEY_SHOME		0607
#define KEY_SIC			0610
#define KEY_SLEFT		0611
#define KEY_SMESSAGE	0612
#define KEY_SMOVE		0613
#define KEY_SNEXT		0614
#define KEY_SOPTIONS	0615
#define KEY_SPREVIOUS	0616
#define KEY_SPRINT		0617
#define KEY_SREDO		0620
#define KEY_SREPLACE	0621
#define KEY_SRIGHT		0622
#define KEY_SRSUME		0623
#define KEY_SSAVE		0624
#define KEY_SSUSPEND	0625
#define KEY_SUNDO		0626
#define KEY_SUSPEND		0627
#define KEY_UNDO		0630
#define KEY_MOUSE		0631
#define KEY_RESIZE		0632
#define KEY_MAX			0777

#define KEY_WUP			1000
#define KEY_WDOWN		1001
#define KEY_W0			1002
#define KEY_WLEFT		1003
#define KEY_WRIGHT		1004
#define KEY_TERMESC		1005


#endif /* GLIB_H_ */

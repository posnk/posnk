/* tinytcap.c */

/* This file contains functions which simulate the termcap functions.
 *
 * It doesn't access a "termcap" file.  Instead, it uses an initialized array
 * of strings to store the entries.  Any string that doesn't start with a ':'
 * is taken to be the name of a type of terminal.  Any string that does start
 * with a ':' is interpretted as the list of fields describing all of the
 * terminal types that precede it.
 *
 * Note: since these are C strings, you can't use special sequences like
 * ^M or \E in the fields; your C compiler won't understand them.  Also,
 * at run time there is no way to tell the difference between ':' and '\072'
 * so I sure hope your terminal definition doesn't require a ':' character.
 *
 * Note that you can include several terminal types at the same time.  Elvis
 * chooses which entry to use at runtime, based primarily on the value of $TERM.
 * Exception: getenv(TERM) on VMS checks the SET TERM device setting.  To
 * implement non-standard terminals set the logical ELVIS_TERM in VMS. (jdc)
 */


#include "elvis.h"
#ifdef FEATURE_RCSID
char id_tinytcap[] = "$Id: tinytcap.c,v 2.26 2001/10/23 01:37:09 steve Exp $";
#endif
#ifdef NEED_TGETENT
# include <stdlib.h>
# include <stdio.h>	/* for 'sprintf()' */

#define TRACE(x)

#if USE_PROTOTYPES
static char *find(char *id, int vtype);
#endif

short ospeed;


/* If no TERM_XXX macros are defined, then assume they should ALL be defined.
 * Note that many of these are traditionally called "ansi", but in fact none
 * of them truly are.  The Coherent console comes closest to true ANSI, so
 * that's the only one that is called "ansi" here; the OS-specific ttytype()
 * functions of all other ports must detect when the name "ansi" is used
 * and convert it to a unique name.
 */
#if !defined(TERM_925) && !defined(TERM_AMIGA) && !defined(TERM_ATARI)
# if !defined(TERM_COHERENT) && !defined(TERM_DOSANSI) && !defined(TERM_MINIX)
#  if !defined(TERM_NANSI) && !defined(TERM_CONSOLE) && !defined(TERM_RAINBOW)
#   if !defined(TERM_VT100) && !defined(TERM_VT100W) && !defined(TERM_VT52)
#    define TERM_925		/* 925, and many other non-ANSI terminals */
#    define TERM_AMIGA		/* Amiga'a console emulator */
#    define TERM_ATARI		/* Atari's console emulator */
#    define TERM_COHERENT	/* Coherent's console */
#    define TERM_DOSANSI	/* PC with ANSI.SYS driver */
#    define TERM_MINIX		/* Minix console, regardless of computer type */
#    define TERM_NANSI		/* PC with NANSI.SYS driver, or BIOS */
#    define TERM_CONSOLE	/* Win32 console */
#    define TERM_RAINBOW	/* DEC Rainbow PC */
#    define TERM_VT100		/* DEC VT100 terminal, 80-column mode */
#    define TERM_VT100W		/* DEC VT100 terminal, 132-column mode */
#    define TERM_VT52		/* DEC VT52 terminal */
#   endif
#  endif
# endif
#endif

#    define TERM_925		/* 925, and many other non-ANSI terminals */
#    define TERM_AMIGA		/* Amiga'a console emulator */
#    define TERM_ATARI		/* Atari's console emulator */
#    define TERM_COHERENT	/* Coherent's console */
#    define TERM_DOSANSI	/* PC with ANSI.SYS driver */
#    define TERM_MINIX		/* Minix console, regardless of computer type */
#    define TERM_NANSI		/* PC with NANSI.SYS driver, or BIOS */
#    define TERM_CONSOLE	/* Win32 console */
#    define TERM_RAINBOW	/* DEC Rainbow PC */
#    define TERM_VT100		/* DEC VT100 terminal, 80-column mode */
#    define TERM_VT100W		/* DEC VT100 terminal, 132-column mode */
#    define TERM_VT52		/* DEC VT52 terminal */
static char *termcap[] =
{

":al=\033[L:dl=\033[M:AL=\033[%dL:DL=\033[%dM:am:xn:bs:ce=\033[K:cl=\033[2J:\
:cm=\033[%i%d;%dH:co#80:\
:k1=#;:k2=#<:k3=#=:k4=#>:k5=#?:k6=#@:k7=#A:k8=#B:k9=#C:k0=#D:\
:s1=#T:s2=#U:s3=#V:s4=#W:s5=#X:s6=#Y:s7=#Z:s8=#[:s9=#\\:s0=#]:\
:c1=#^:c2=#_:c3=#`:c4=#a:c5=#b:c6=#c:c7=#d:c8=#e:c9=#f:c0=#g:\
:a1=#h:a2=#i:a3=#j:a4=#k:a5=#l:a6=#m:a7=#n:a8=#o:a9=#p:a0=#q:\
:kd=#P:kh=#G:kH=#O:kI=#R:kD=#S:kl=#K:kN=#Q:kP=#I:kr=#M:ku=#H:k+=#):kB=#\011\
:li#25:md=\033[1m:me=\033[m:nd=\033[C:se=\033[m:so=\033[7m:\
:ti=\033[?1h:te=\033[?1l:\
:ue=\033[m:up=\033[A:us=\033[4m:\
:vs=\033[?12h:ve=\033[?12l:\
:ac=q\304x\263m\300v\301j\331t\303n\305u\264l\332w\302k\277:",
(char *)0
};


static char *fields;


/*ARGSUSED*/
int tgetent(bp, name)
	char	*bp;	/* buffer for storing the entry -- ignored */
	char	*name;	/* name of the entry */
{
	fields = termcap[0];
	return 1;
}


static char *find(id, vtype)
	char	*id;	/* name of a value to locate */
	int	vtype;	/* '=' for strings, '#' for numbers, or ':' for bools */
{
	int	i;
TRACE(fprintf(stderr, "find(\"%s\", '%c')\n", id, vtype);)

	/* search for a ':' followed by the two-letter id */
	for (i = 0; fields[i]; i++)
	{
		if (fields[i] == ':'
		 && fields[i + 1] == id[0]
		 && fields[i + 2] == id[1])
		{
			/* if correct type, then return its value */
			if (fields[i + 3] == vtype)
				return &fields[i + 4];
			else
				return (char *)0;
		}
	}
	return (char *)0;
}

int tgetnum(id)
	char	*id;
{
TRACE(fprintf(stderr, "tgetnum(\"%s\")\n", id);)
	id = find(id, '#');
	if (id)
	{
		return atoi(id);
	}
	return -1;
}

int tgetflag(id)
	char	*id;
{
	if (find(id, ':'))
	{
		return 1;
	}
	return 0;
}

/*ARGSUSED*/
char *tgetstr(id, bp)
	char	*id;
	char	**bp;	/* pointer to pointer to buffer - ignored */
{
	char	*cpy;

	/* find the string */
	id = find(id, '=');
	if (!id)
	{
		return (char *)0;
	}

	/* copy it into the buffer, and terminate it with NUL */
	for (cpy = *bp; *id != ':'; )
	{
		if (id[0] == '\\' && id[1] == 'E')
			*cpy++ = '\033', id += 2;
		else
			*cpy++ = *id++;
	}
	*cpy++ = '\0';

	/* update the bp pointer */
	id = *bp;
	*bp = cpy;

	/* return a pointer to the copy of the string */
	return id;
}

/*ARGSUSED*/
char *tgoto(cm, destcol, destrow)
	char	*cm;	/* cursor movement string */
	int	destcol;/* destination column, 0 - 79 */
	int	destrow;/* destination row, 0 - 24 */
{
	static char buf[30];
	char	*build;
	int	tmp;

	for (build = buf; *cm; cm++)
	{
		if (*cm == '%')
		{
			switch (*++cm)
			{
			  case '+':
				tmp = destrow;
				destrow = destcol;
				destcol = tmp;
				*build++ = *++cm + tmp;
				break;

			  case 'i':
				destcol++;
				destrow++;
				break;

			  case 'd':
				tmp = destrow;
				destrow = destcol;
				destcol = tmp;
				sprintf(build, "%d", tmp);
				build += strlen(build);
				break;
			}
		}
		else
		{
			*build++ = *cm;
		}
	}
	*build = '\0';
	return buf;
}

/*ARGSUSED*/
void tputs(cp, affcnt, outfn)
	char	*cp;		/* the string to output */
	int	affcnt;		/* number of affected lines -- ignored */
	int	(*outfn) P_((int));	/* the output function */
{
	while (*cp)
	{
		(*outfn)(*cp);
		cp++;
	}
}
#endif /* NEED_TGETENT */

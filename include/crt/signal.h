#ifndef _SIGNAL_H
#define _SIGNAL_H
// sig type : void (*func)(int)

#include <stdint.h>
#include <sys/types.h>

/** Request for default signal handling */
#define SIG_DFL			( ( void (*)(int) ) 0xFFFFFFFE)
/** Return value from signal() in case of an error */
#define SIG_ERR			( ( void (*)(int) ) 0xFFFFFFFF)
/** Request that signal be held */
#define SIG_HOLD		( ( void (*)(int) ) 0xFFFFFFFD)
/** Request that signal be ignored */
#define SIG_IGN			( ( void (*)(int) ) 0xFFFFFFFC)

/** Type that can be accessed atomically */
typedef volatile int	sig_atomic_t;

/** Integer type used to represent sets of signals */
typedef uint32_t		sigset_t;

typedef struct siginfo	siginfo_t;

#define	SIGHUP	1		/* hangup */
#define	SIGINT	2		/* interrupt (rubout) */
#define	SIGQUIT	3		/* quit (ASCII FS) */
#define	SIGILL	4		/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5		/* trace trap (not reset when caught) */
#define	SIGIOT	6		/* IOT instruction */
#define	SIGABRT 6		/* used by abort, replace SIGIOT in the future */
#define SIGWINCH 7
#define	SIGFPE	8		/* floating point exception */
#define	SIGKILL	9		/* kill (cannot be caught or ignored) */
#define	SIGBUS	10		/* bus error */
#define	SIGSEGV	11		/* segmentation violation */
#define	SIGSYS	12		/* bad argument to system call */
#define	SIGPIPE	13		/* write on a pipe with no one to read it */
#define	SIGALRM	14		/* alarm clock */
#define	SIGTERM	15		/* software termination signal from kill */
#define	SIGUSR1	16		/* user defined signal 1 */
#define	SIGUSR2	17		/* user defined signal 2 */
#define	SIGCLD	18		/* child status change */
#define	SIGCHLD	18		/* child status change alias (POSIX) */
#define	SIGURG	21		/* urgent socket condition */
#define	SIGPOLL 22		/* pollable event occured */
#define	SIGIO	SIGPOLL	/* socket I/O possible (SIGPOLL alias) */
#define	SIGSTOP 23		/* stop (cannot be caught or ignored) */
#define	SIGTSTP 24		/* user stop requested from tty */
#define	SIGCONT 25		/* stopped process has been continued */
#define	SIGTTIN 26		/* background tty read attempted */
#define	SIGTTOU 27		/* background tty write attempted */
#define	SIGVTALRM 28	/* virtual timer expired */
#define	SIGPROF 29		/* profiling timer expired */
#define	SIGXCPU 30		/* exceeded cpu limit */
#define	SIGXFSZ 31		/* exceeded file size limit */
#define NSIG	32
struct sigaction {
	union {
		/** Pointer to a signal-catching function or one of the macros
            SIG_IGN or SIG_DFL.  */
		void (*sa_handler)(int);
		/** Pointer to a signal-catching function. */
		void (*sa_sigaction)(int, siginfo_t *, void *);
	};
	/** Set of signals to be blocked while the handler executes */
	sigset_t	sa_mask;
	/** Special flags */
	int			sa_flags;
};

/** set mask with sigprocmask() */
#define SIG_SETMASK		(0)
/** set of signals to block */
#define SIG_BLOCK		(1)
/** set of signals to unblock */
#define SIG_UNBLOCK		(2)

/** Do not generate SIGCHLD when children stop */
#define SA_NOCLDSTOP	(1 << 0)
/** Use alternate stack for handlers */
#define SA_ONSTACK		(1 << 1)
/** Remove handler after execution */
#define SA_RESETHAND	(1 << 2)
/** Prevents EINTR in some cases */
#define SA_RESTART		(1 << 3)
/** Enable passing siginfo */
#define SA_SIGINFO		(1 << 4)
/** Set children to immediately die instead of becoming zombies */
#define SA_NOCLDWAIT	(1 << 5)
/** Do not block signal on entry to handler */
#define SA_NODEFER		(1 << 6)

/** The process is on an alternate stack */
#define SS_ONSTACK		(1 << 0)
/** Alternate signal stack is disabled */
#define SS_DISABLE		(1 << 1)

/** Minimal signal stack size */
#define MINSIGSTKSZ		(1024)

/** Default signal stack size */
#define SIGSTKSZ		(2*MINSIGSTKSZ)

typedef struct {
	/** Stack base or pointer */
	void *	ss_sp;
	/** Stack size */
	size_t	ss_size;
	/** Flags */
	int		ss_flags;
} stack_t;

struct sigstack {
	/** Non-zero when signal stack is in use */
	int 	ss_onstack;
	/** Signal stack pointer */
	void *	ss_sp;
};

struct siginfo {
	/** Signal number */
	int				si_signo;
	/** Signal code */
	int				si_code;
	/** errno value associated with this signal */
	int				si_errno;
	/** Sending process ID */
	pid_t			si_pid;
	/** UID of process that sent the signal */
	uid_t			si_uid;
	/** Fault address */
	void *			si_addr;
	/** Exit value or signal */
	int				si_status;
	/** Band event for SIGPOLL */
	long			si_band;
};

/** Illegal opcode */
#define	ILL_ILLOPC	(0)
/** Illegal operand */
#define	ILL_ILLOPN	(1)
/** Illegal addressing mode */
#define	ILL_ILLADR	(2)
/** Illegal trap */
#define	ILL_ILLTRP	(3)
/** Privileged opcode */
#define	ILL_PRVOPC	(4)
/** Privileged register */
#define	ILL_PRVREG	(5)
/** Coprocessor error */
#define	ILL_COPROC	(6)
/** Internal stack error */
#define	ILL_BADSTK	(7)

/** Integer divide by zero */
#define FPE_INTDIV	(0)
/** Integer overflow */
#define FPE_INTOVF	(1)
/** Float divide by zero */
#define FPE_FLTDIV	(2)
/** Float overflow */
#define FPE_FLTOVF	(3)
/** Float underflow */
#define FPE_FLTUND	(4)
/** Float inexact */
#define FPE_FLTRES	(5)
/** Float operation invalid */
#define FPE_FLTINV	(6)
/** Float subscript invalid */
#define FPE_FLTSUB	(7)

/** Address not mapped */
#define SEGV_MAPERR	(0)
/** Access denied */
#define SEGV_ACCERR	(1)

/** Invalid address alignment */
#define BUS_ADRALN	(0)
/** No such address */
#define BUS_ADRERR	(1)
/** Hardware error */
#define BUS_OBJERR	(2)

/** Breakpoint */
#define TRAP_BRKPT	(0)
/** Tracepoint */
#define TRAP_TRACE	(1)

/** Data available */
#define POLL_IN		(0)
/** Can transmit */
#define POLL_OUT	(1)
/** Message available */
#define POLL_MSG	(2)
/** IO Error */
#define POLL_ERR	(3)
/** Priority data available */
#define POLL_PRI	(4)
/** Disconnected */
#define POLL_HUP	(5)

/** kill() sent this */
#define SI_USER		(80)
/** sigqueue() sent this */
#define SI_QUEUE	(81)
/** timer sent this */
#define SI_TIMER	(82)
/** Async IO completed */
#define SI_ASYNCIO	(83)
/** Message available */
#define SI_MESGQ	(84)

void	(*bsd_signal(int, void (*)(int)))(int);

int		kill		( pid_t, int );

int		killpg		( pid_t, int );

int		raise		( int );

int		sigaction(int, const struct sigaction *, struct sigaction *);

int		sigaddset	( sigset_t *set, int sig );

int		sigdelset	( sigset_t *set, int sig );

int		sigismember	( const sigset_t *set, int sig );

int		sigfillset	( sigset_t *set );

int		sigemptyset	( sigset_t *set );

int		sigaltstack	( const stack_t *, stack_t * );

int		siginterrupt( int, int );

void (*	signal		( int sig, void (*)(int) ))( int );

int		sighold		( int sig );
 
int		sigignore	( int sig );

int		sigpause	( int sig );

int		sigrelse	( int sig );

void (*	sigset		( int sig, void (*disp)( int ) ))( int );

int		sigpending	( sigset_t * );

int		sigprocmask	( int, const sigset_t *, sigset_t * );

int		sigsuspend	( const sigset_t * );

int		sigwait		( const sigset_t *, int * );

int		sigblock(int mask);

int		siggetmask(void);

int		sigsetmask(int mask);

int		sigmask(int signum);


#endif

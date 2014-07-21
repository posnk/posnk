#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <_ansi.h>
#include <sys/times.h>
#include <sys/types.h>
#include "posnk_sc.h"

/*
 * Version of environ for POS.
 */

char **environ = (char **) 0x4020000; 

int strlistlen(char **list)
{
	int len = 0;
	while (list[len++]);
	return len-1;
}

int strlistlen(char **list);

int
_DEFUN (_execve, (name, argv, env),
        char  *name  _AND
        char **argv  _AND
        char **env)
{
   int c;
   uint32_t argvc = (uint32_t) (strlistlen(argv) + 1);
   uint32_t envc = (uint32_t) (strlistlen(env) + 1);
   uintptr_t *argvs = malloc(argvc * sizeof(char *));
   uintptr_t *envs = malloc(envc * sizeof(char *));
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
	argvs[c] = strlen(argv[c]) + 1;
   for (c = 0; c < (envc - 1); c++)
	envs[c] = strlen(env[c]) + 1;

   return (int) nk_do_syscall(SYS_EXECVE, a, b); 
}

_VOID
_DEFUN (_exit, (rc),
	int rc)
{
   int a[] = {(uint32_t) rc,0,0,0};
   int b[] = {0,0,0,0};
   nk_do_syscall(SYS_EXIT, a, b); 
}

int
_DEFUN (fork, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int)nk_do_syscall(SYS_FORK, a, b); 
}

int
_DEFUN (vfork, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int)nk_do_syscall(SYS_FORK, a, b); 
}


int
_DEFUN (getpid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETPID, a, b); 
}


int
_DEFUN (getppid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETPPID, a, b); 
}


uid_t
_DEFUN (getuid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETUID, a, b); 
}

pid_t
_DEFUN (getpgrp, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETPGRP, a, b); 
}

pid_t
_DEFUN (getsid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETSID, a, b); 
}

uid_t
_DEFUN (geteuid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETEUID, a, b); 
}

gid_t
_DEFUN (getgid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETGID, a, b); 
}

gid_t
_DEFUN (getegid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_GETEGID, a, b); 
}

pid_t
_DEFUN (setsid, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SETSID, a, b); 
}

int
_DEFUN (setpgrp, (),
        _NOARGS)
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SETPGRP, a, b); 
}

int setuid(int uid) {
   int a[] = {uid,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SETUID, a, b); 
}

int setgid(int gid) {
   int a[] = {gid,0,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SETGID, a, b); 
}

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
   int a[] = {(uint32_t) addr,(uint32_t) len, (uint32_t) prot, (uint32_t) flags};
   int b[] = {(uint32_t) fildes, (uint32_t) off,0,0};
   return (uint32_t) nk_do_syscall(SYS_MMAP, a, b); 
}

int setresuid(int uid) {
	return 0;
}

int setresgid(int gid) {
	return 0;
}

int getgrgid(){
	return getgid();
}

int setpgid(pid_t pid, pid_t pgid) {
   int a[] = {(int)pid,(int)pgid,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SETPGID, a, b); 
}

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler)
{
   int a[] = {(int)signum,(int)handler,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SIGNAL, a, b); 
}

void exitsig()
{
   int a[] = {0,0,0,0};
   int b[] = {0,0,0,0};
   nk_do_syscall(SYS_EXITSIG, a, b); 	
}

void ssigex()
{
   int a[] = {(uint32_t) &exitsig,0,0,0};
   int b[] = {0,0,0,0};
   nk_do_syscall(SYS_SSIGEX, a, b); 
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
   int a[] = {(uint32_t) how, (uint32_t) set, (uint32_t) oldset, 0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_SIGPROCMASK, a, b); 
}

int sigsetmask(int mask)
{
	int old;
	if (sigprocmask(SIG_SETMASK, (sigset_t *) &mask, (sigset_t *) &old))
		return 0;
	return old;
}

int sigblock(int mask)
{
	int old;
	if (sigprocmask(SIG_BLOCK, (sigset_t *) &mask, (sigset_t *) &old))
		return 0;
	return old;
}

int siggetmask()
{
	int old, mask = 0;
	if (sigprocmask(SIG_BLOCK, (sigset_t *) &mask, (sigset_t *) &old))
		return 0;
	return old;
}

int killpg(int pgrp, int sig)
{
	return kill(-pgrp, sig);
}

int getgroups(int size, gid_t list[])
{
	return 0;//TODO: Implement getgroups
}
 
pid_t wait(int *status)
{
	return waitpid (-1,status,0);
}

pid_t wait3(int *status, int options,
                   struct rusage *rusage)
{
	return waitpid(-1,status,options);
}
int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
	//printf("WARN: USING SIGACTION\n");
}

unsigned int alarm(unsigned int seconds)
{
	return 0;
}

unsigned int sleep(unsigned int seconds)
{
   int a[] = {(uint32_t) seconds, 0,0,0};
   int b[] = {0,0,0,0};
   return nk_do_syscall(SYS_SLEEP, a, b); 
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	sleep((unsigned int)req->tv_sec);
	return 0;
}

int stime(time_t *t)
{
   int a[] = {(uint32_t) t, 0,0,0};
   int b[] = {0,0,0,0};
   return nk_do_syscall(SYS_STIME, a, b); 
}

int usleep(useconds_t usec)
{
   int a[] = {(uint32_t) usec, 0,0,0};
   int b[] = {0,0,0,0};
   return nk_do_syscall(SYS_USLEEP, a, b); 
}

int
_DEFUN (kill, (pid, sig),
        int pid  _AND
        int sig)
{  
   int a[] = {pid,sig,0,0};
   int b[] = {0,0,0,0};
   return (int) nk_do_syscall(SYS_KILL, a, b); 
}

void *
sbrk (incr)
     int incr;
{ 
   int a[] = {(uint32_t) incr,0,0,0};
   int b[] = {0,0,0,0};
   return (void *) nk_do_syscall(SYS_SBRK, a, b); 
} 

int
_DEFUN (waitpid, (pid, status, options),
	pid_t pid _AND
        int  *status _AND
	int  options)
{
	//Calls WAITPID!!!
   uint32_t a[] = {(uint32_t) pid, (uint32_t) status, (uint32_t) options, 0};
   uint32_t b[] = {0, 0, 0, 0};
   return (int) nk_do_syscall(SYS_WAITPID, a, b); 
}

clock_t
_DEFUN (times, (buf),
        struct tms *buf)
{
  errno = ENOSYS;
  return -1;
}

int clearenv(){
	return 0;
}

int getgrouplist(const char *user, gid_t group,
                        gid_t *groups, int *ngroups)
{
	*ngroups = 0;
	return *ngroups;
}


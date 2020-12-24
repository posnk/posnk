#ifndef __sys_procfs_h__
#define __sys_procfs_h__

#include <signal.h>

typedef struct {
    int            signal;
    int            task;
    struct siginfo info;
} procsig_t;

#endif
#ifndef __SYS_DIRENT_H__
#define __SYS_DIRENT_H__

#include <sys/types.h>

struct sys_dirent
{
    ino_t		d_ino;//2
    uint32_t		d_dev;//4
    unsigned short int  d_reclen;//2 + 2 + 4 = 8 -> this struct is long alligned
    char	 	d_name[256];//THIS IS ALLIGNED
};

typedef struct sys_dirent sys_dirent_t;

#endif

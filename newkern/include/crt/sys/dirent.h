#ifndef __SYS_DIRENT_H__
#define __SYS_DIRENT_H__

#include <sys/types.h>

struct sys_dirent
{
    ino_t		d_ino;//4
    dev_t		d_dev;//2
    unsigned short int  d_reclen;//4 + 2 + 4 = 8 -> this struct is long alligned
    char	 	d_name[257];//THIS IS ALLIGNED
}  __attribute__((packed));

typedef struct sys_dirent sys_dirent_t;

#endif

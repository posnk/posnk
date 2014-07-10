/* libc/sys/linux/sys/dirent.h - Directory entry as returned by readdir */

/* Written 2000 by Werner Almesberger */


#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <sys/types.h>
#define _LIBC 1
#define  NOT_IN_libc 1
#include <sys/lock.h>
#undef _LIBC

#define HAVE_NO_D_NAMLEN	/* no struct dirent->d_namlen */

#define MAXNAMLEN 255		/* sizeof(struct dirent.d_name)-1 */


struct dirent
{
    ino_t		d_ino;//2
    unsigned short int	d_dev;//4
    unsigned short int	d_reclen;//2 + 2 + 4 = 8 -> this struct is long alligned
    char		d_name[257];//THIS IS ALLIGNED
}  __attribute__((packed));

typedef struct {
    int dd_fd;		/* directory file */
    int dd_loc;		/* position in buffer */
    int dd_seek;
    char *dd_buf;	/* buffer */
    int dd_len;		/* buffer length */
    int dd_size;	/* amount of data in buffer */
} DIR;

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict,
              struct dirent **__restrict);
void rewinddir(DIR *);
int closedir(DIR *);

#endif

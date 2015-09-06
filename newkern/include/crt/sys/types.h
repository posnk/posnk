#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int ssize_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
typedef long suseconds_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define MAJOR(d) ((d & 0xFF00) >> 8)
#define MINOR(d) (d & 0xFF)

#define MAKEDEV(m,n) (((m << 8) & 0xFF00) | (n & 0xFF))

typedef int pid_t;
typedef unsigned short uid_t;
typedef unsigned short gid_t;
typedef unsigned short dev_t;
typedef unsigned long ino_t;
typedef unsigned long mode_t;
typedef unsigned long umode_t;
typedef unsigned short nlink_t;
typedef int daddr_t;
typedef long off_t;
typedef unsigned long aoff_t;
typedef unsigned char u_char;
typedef unsigned short ushort;
typedef unsigned long key_t;

typedef struct { int quot,rem; } div_t;
typedef struct { long quot,rem; } ldiv_t;

struct ustat {
	daddr_t f_tfree;
	ino_t f_tinode;
	char f_fname[6];
	char f_fpack[6];
};



#endif

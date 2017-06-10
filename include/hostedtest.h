#define MAJOR(d) ((d & 0xFF00) >> 8)
#define MINOR(d) (d & 0xFF)

#define MAKEDEV(m,n) (((m << 8) & 0xFF00) | (n & 0xFF))

typedef unsigned long umode_t;
typedef int daddr_t;
typedef long off_t;
typedef unsigned long aoff_t;
typedef unsigned char u_char;
typedef unsigned short ushort;

#include <stdio.h>
#include <crt/sys/error.h>
#define ESUCCESS 0
#define ENMFILE  424

#define debugcon_aprintf	printf
#define debugcon_printf	printf

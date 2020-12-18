#ifndef __SYS_UTSNAME_H__
#define __SYS_UTSNAME_H__

struct utsname {
	char	sysname	[20];
	char	nodename[80];
	char	release	[20];
	char	version	[80];
	char	machine	[10];
};



#ifdef __cplusplus
extern "C" {
#endif

int uname( struct utsname * );

#ifdef __cplusplus
}
#endif

#endif

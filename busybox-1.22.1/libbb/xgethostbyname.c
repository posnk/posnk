/* vi: set sw=4 ts=4: */
/*
 * Mini xgethostbyname implementation.
 *
 * Copyright (C) 2001 Matt Kraai <kraai@alumni.carnegiemellon.edu>.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
//#include "libbb.h"
/*
struct hostent* FAST_FUNC xgethostbyname(const char *name)
{
	struct hostent *retval = gethostbyname(name);
	if (!retval)
		bb_herror_msg_and_die("%s", name);
	return retval;
}*/

int setuid(int uid) {
	return 0;
}

int setgid(int gid) {
	return 0;
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

int setpgid(int gid) {
	return 0;
}

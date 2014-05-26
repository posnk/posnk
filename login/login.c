/**
 * login.c
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include "login.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

void usage(){
}

int main(int argc, char *argv[], char *envp[]){
	char *pass;	
	struct passwd *pwent;
	pwent = getpwnam(argv[1]);
	//pass = getpass("Password: ");
	if (!pwent) {
		printf("Invalid username or password!\n");
		return 255;
	}
	setuid(pwent->pw_uid);
	setgid(pwent->pw_gid);
	setenv("PATH","/bin:/sbin:/usr/bin:/usr/sbin",1);
	chdir(pwent->pw_dir);
	execlp(pwent->pw_shell, pwent->pw_shell, NULL);
	return 0;
}


/**
 * getty.c
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include "getty.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage(){
}

int main(int argc, char *argv[], char *envp[]){
	char username[80];
	pid_t pid;	
	if (argc < 2) {
		usage();
		return 0;
	}
	pid = setsid();	
	if (pid < 0) {
		fprintf(stderr, "getty: already a group leader!\n");
		return 255;
	}	
	if(!tty_open(argv[1])) {
		return 255;//No point in printing an error message
	}
	print_issue(argv[1]);
	while (strlen(username) == 0) {
		printf("login: ");
		scanf("%s", username);
	}
	
	execlp(LOGIN_PROGRAM, LOGIN_PROGRAM, username, NULL);
	return 0;
}


/**
 * main.c
 *
 * Part of P-OS csession
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 29-08-2014 - Created
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t oswin_pid;

void start_oswin()
{
	oswin_pid = fork();
	if (oswin_pid == 0)
		execlp("oswin", "oswin", "", NULL);
	usleep(10000);
}

int main(int argc, char *argv[], char *envp[]){
	int ret_status;
	system("stty -echo raw");
	start_oswin();
	system("tardis");
	waitpid(oswin_pid,&ret_status,0);
	return 0;
}


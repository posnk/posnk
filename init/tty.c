/**
 * tty.c
 *
 * Part of P-OS init.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

#include "init.h"

#define ARR_SIZE 1<<16
#define BUF_SIZE 80

int		  tty_count;
pid_t		 *tty_process_list;
time_t		 *tty_process_start;
char		**tty_process_cmd;

int find_tty(pid_t pid)
{
	int c;
	for (c = 0; c < tty_count; c++)
		if (tty_process_list[c] == pid)
			return c;
	return -1;
}

void start_tty(int id) 
{
	char *args[ARR_SIZE];
	char buf[BUF_SIZE];
	size_t nargs;
	pid_t pid;

	strcpy(buf, tty_process_cmd[id]);
	parse_args(buf, args, ARR_SIZE, &nargs); 
	
	if (nargs == 0) {
		fprintf(stderr, "init: tty %i has invalid command!\n", id);
		return;
	}
	
	if (tty_process_list[id]) {
		if (difftime(time(NULL), tty_process_start[id]) < 1) {
			fprintf(stderr, "init: tty %i respawning too fast!\nDropping into recovery shell\n", id);
			execlp(RC_INTERPRETER_PATH, RC_INTERPRETER_NAME, NULL);
		}			
	}

	tty_process_start[id] = time(NULL);	
	
	pid = fork();

	if (pid) {
		tty_process_list[id] = pid;
		return;
	} else {	
		setsid();//We are a new session!
		execvp(args[0], args);
	}
}

void start_ttys()
{
	int c;
	for (c = 0; c < tty_count; c++){
		tty_process_list[c] = 0;
		start_tty(c);
	}
}

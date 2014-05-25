/**
 * rc.c
 *
 * Part of P-OS init.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include "init.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int execute_rc_scripts()
{
	pid_t pid;
	int ret_status;
	pid = fork();
	if (pid) {
		pid = waitpid(pid, &ret_status, 0);
		if (WIFSIGNALED(ret_status)) {
			printf("init: /etc/rc was killed by signal: %s\n", strsignal(WTERMSIG(ret_status)));
			return 0;
		} else if (WEXITSTATUS(ret_status)) {
			printf("init: /etc/rc terminated unsuccessfully with status %i\n", WEXITSTATUS(ret_status));
			return 0;
		}
		return 1;
	} else {
		execlp(RC_INTERPRETER_PATH, RC_INTERPRETER_NAME, RC_SCRIPT_PATH, NULL);
		return 0;//Keep GCC happy
	}
}

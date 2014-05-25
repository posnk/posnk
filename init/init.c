/**
 * init.c
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
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void wait_loop()
{
	pid_t terminated_pid;
	int ret_status, tty_id;
	for (;;) {
		terminated_pid = wait(&ret_status);
		tty_id = find_tty(terminated_pid);
		if (tty_id == -1) {
			//Orphaned process, ignore!
		} else {
			//Respawn tty
			start_tty(tty_id);
		}
	}
}

int main(int argc, char *argv[], char *envp[]){
    if (!execute_rc_scripts()) {
	fprintf(stderr, "init: error running rc script, dropping to singleuser mode!\n");
	setup_singleuser_mode();
    } else
	read_inittab();
    start_ttys();
    wait_loop();
    return 0;
}


/**
 * inittab.c
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
#include <string.h>

extern int		  tty_count;
extern pid_t		 *tty_process_list;
extern time_t		 *tty_process_start;
extern char		**tty_process_cmd;

void setup_singleuser_mode()
{
	tty_process_start= malloc(sizeof(time_t));
	tty_process_list = malloc(sizeof(pid_t));
	tty_process_cmd	 = malloc(sizeof(char *));
	tty_count = 1;
	tty_process_cmd[0] = SINGLEUSER_CMD;
}

void read_inittab()
{
	int tty_ctr;
	FILE *inittab;	
	inittab = fopen(INITTAB_PATH, "r");
	if (!inittab) {
		fprintf(stderr, "init: could not open %s, dropping to single user mode!\n", INITTAB_PATH);
		setup_singleuser_mode();
		return;
	}
	fscanf(inittab, "%i\n", &tty_count);
	tty_process_list = malloc(tty_count * sizeof(pid_t));
	tty_process_cmd	 = malloc(tty_count * sizeof(char *));
	tty_process_start= malloc(tty_count * sizeof(time_t));
	for (tty_ctr = 0; tty_ctr < tty_count; tty_ctr++) {
		tty_process_cmd[tty_ctr] = malloc(80);
		tty_process_cmd[tty_ctr][0] = 0;
		fgets(tty_process_cmd[tty_ctr], 80, inittab);
		if (feof(inittab)) {
			fprintf(stderr, "init: unexpected EOF reading %s\n", INITTAB_PATH);
		}
	}
	fclose(inittab);
}

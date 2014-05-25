/**
 * issue.c
 *
 * Part of P-OS getty.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */
#include <string.h>
#include <stdio.h>
#include "getty.h"

void print_issue(char *tty_dev) {
	FILE *f;
	char s[1120];

	f = fopen(ISSUE_FILE, "r");

	if(!f) {
		perror("getty; could not open issue file:")  ;  
		return;
	}
	while (fgets(s,1120,f)!=NULL) {
		if (!strcmp(s, "<ttydev>\n"))
			printf("%s\n", tty_dev);
		else
			printf("%s", s);
	}
	fclose(f);
}

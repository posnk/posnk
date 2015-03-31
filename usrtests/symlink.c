/**
 * @file usrtests/symlink.c
 *
 * Tests symlink behaviour 
 *
 * Part of P-OS.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 06-03-2015 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


void smkdir(const char *expl, const char *name, mode_t mode)
{
	printf("%s", expl);

	if (mkdir(name, mode)) {
		printf("FAIL [%s]\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("OK\n");
}

void ssymlink(const char *expl, const char *name, const char *dest)
{
	printf("%s", expl);

	if (symlink(dest, name)) {
		printf("FAIL [%s]\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("OK\n");
}


void schdir(const char *expl, const char *name)
{
	printf("%s", expl);

	if (chdir(name)) {
		printf("FAIL [%s]\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("OK\n");
}

void pwd(const char *reason)
{
	char path_buffer[256];

	getcwd(path_buffer, 255);

	printf("%s: %s\n", reason, path_buffer);
}

int main( int argc, char **argv )
{
	char path_buffer[256];
	
	printf("symlink test utility v0.01 by Peter Bosch\n\n");

	getcwd(path_buffer, 255);

	printf("Initial working directory: %s\n", path_buffer);

	smkdir("Creating working directory [symlink_wd] for tests...", 
		"./symlink_wd", 
		0777);

	schdir("Entering working directory [symlink_wd]...", 
		"./symlink_wd");

	getcwd(path_buffer, 255);

	printf("Working directory: %s\n", path_buffer);

	smkdir("Creating 1st directory [dir1] for tests...", 
		"./dir1", 
		0777);

	smkdir("Creating 2nd directory [dir2] for tests...", 
		"./dir2", 
		0777);

	smkdir("Creating 1st subdirectory [dir1/sdir1] for tests...", 
		"./dir1/sdir1", 
		0777);

	ssymlink("Creating symlink [dir2/sdirlink1 -> dir1/sdir1]...", 
		"./dir2/sdirlink1",
		"../dir1/sdir1");
	
	
	schdir("Entering symlinked directory [dir2/sdirlink1]...",
		"./dir2/sdirlink1");

	pwd("Working directory inside symlink");
	
	schdir("Going up one directory [..]...",
		"..");	
	
	pwd("Working directory after .. on symlink");
	
	schdir("Returning to original working directory...",
		path_buffer);
	
	pwd("Current wd");
	
	
	schdir("Entering symlinked directory's parent directly [dir2/sdirlink1/..]...",
		"./dir2/sdirlink1/..");
	
	pwd("Working directory after symlink/.. ");

		
	
	
	

}

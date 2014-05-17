/* 
 * This program illustrates how to use the pipe(), fork(), dup2() and exec()
 * system calls to control stdin and stdout of an exec'ed program via pipes
 * from within the calling program.
 *
 * Just because someone asked me ;-)
 * Eike Rathke
 * Tue Jun 26 21:44:17 CEST 2001
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_SIZE 1<<16
#define ARR_SIZE 1<<16

char *cur_dir = "/";

void parse_args(char *buffer, char** args, 
                size_t args_size, size_t *nargs)
{
    char *buf_args[args_size]; /* You need C99 */
    char **cp;
    char *wbuf;
    size_t i, j;
    
    wbuf=buffer;
    buf_args[0]=buffer; 
    args[0] =buffer;
    
    for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }
    
    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
            args[j++]=buf_args[i];
    }
    
    *nargs=j;
    args[j]=NULL;
}

int builtin_cd(char *argv[])
{
	if (argv[1] == NULL){
		printf("psh: cd: No directory specified\n");
		return 0;
	}
	chdir(argv[1]);
	cur_dir = argv[1];//TODO: FIX PWD
	return 1;
}

int builtin_export(char *argv[])
{
	if ((argv[1] == NULL) || (argv[2] == NULL)){
		printf("psh: export: Usage: export <VARNAME> <VALUE>\n");
		return 0;
	}
	setenv(argv[1], argv[2], 1);
	return 1;
}

int builtin_ls(char *argv[])
{	
    int ret_status;
    pid_t pid;
	char *path = cur_dir;
	if ((argv[1] != NULL)){
		path = argv[1];
	}
		pid = fork();
		if (pid){
		    pid = wait(&ret_status);
			if (WIFSIGNALED(ret_status))
			    printf("%s: %s\n", argv[0], strsignal(WTERMSIG(ret_status)));
		} else {
			execlp("ls", "ls", "l", path, NULL);
		}
	return 1;
}

int main(int argc, char *argv[], char *envp[]){
    char buffer[BUFFER_SIZE];
    char *args[ARR_SIZE];

    int ret_status;
    size_t nargs;
    pid_t pid;
    
    while(1){
        printf("$ ");
        fgets(buffer, BUFFER_SIZE, stdin);
        parse_args(buffer, args, ARR_SIZE, &nargs); 

        if (nargs==0) continue;
        if (!strcmp(args[0], "exit" )) 
		exit(0);
	else if (!strcmp(args[0], "cd" )) 
		builtin_cd(args);
	else if (!strcmp(args[0], "export" )) 
		builtin_export(args);
	else if (!strcmp(args[0], "ls" )) 
		builtin_ls(args);
	else {
		pid = fork();
		if (pid){
		    pid = wait(&ret_status);
			if (WIFSIGNALED(ret_status))
			    printf("%s: %s\n", args[0], strsignal(WTERMSIG(ret_status)));
		} else {
		    if( execvp(args[0], args)) {
		        puts(strerror(errno));
		        exit(127);
		    }
		}
	}
    }    
    return 0;
}


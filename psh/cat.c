
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
	FILE *f;
	char s[1120];
	printf("p-os cat v0.01 argc=%i ",argc);
	printf("argv[0]=%s ",argv[0]);
	printf("argv[1]=%s\n",argv[1]);
	f=fopen(argv[1], "r");
	if(!f) {
		perror("Could not open file:")  ;  
		return 1;
	}
	while (fgets(s,1120,f)!=NULL) {
		printf("%s", s);
	}
	fclose(f);
	return 0;
}

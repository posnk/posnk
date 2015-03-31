#include <signal.h>
#include <stdio.h>

void exitsig();

void our_handler(int signum)
{
	printf("Signal handler called! %i\n", signum);
//	exitsig();
}

int main()
{
	int nuf;
	printf("Installing handler...\n");
	signal(SIGUSR1, &our_handler);
	printf("OK\nWaiting for signal...\n");
	while (1)
		wait(&nuf);
	
}

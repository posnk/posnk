#include <stdio.h>
#include <clara/clara.h>

int main(int argc, char **argv)
{
	clara_init_client("/oswdisp");
	clara_exit_client();
	return 0;
}

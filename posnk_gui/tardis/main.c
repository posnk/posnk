#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <clara/clara.h>
#include <wtk/window.h>
#include <wtk/widget.h>

#include "tardis.h"

int main(int argc, char **argv)
{

	clara_init_client("/oswdisp");	
	
	desktop_initialize("/share/background.png");

	while (1) {
		desktop_process();
		usleep(999);
	}

	clara_exit_client();
	return 0;
}

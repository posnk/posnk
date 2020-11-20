#include "kernel/earlycon.h"
#include <stdint.h>
#include <string.h>

int earlycon_conup = 0;

char earlycon_screen_buffer[40960];
int earlycon_screen_ptr = 0;
void utoa(unsigned int value,int base,char *str);
void utoa_s(int value,int base,char *str);


void earlycon_switchover()
{
	con_hputs(0,0,earlycon_screen_buffer);
	earlycon_conup = 1;
}

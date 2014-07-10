#include <stdio.h>
#include <stdint.h>
#define SYS_DBGDROP	58

uint32_t nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4]);

int main()
{
	uint32_t nullarray[4];
	printf("dropping into kernel debugger\n");
	nk_do_syscall(SYS_DBGDROP, nullarray, nullarray);
	printf("back\n");
	return 0;
}

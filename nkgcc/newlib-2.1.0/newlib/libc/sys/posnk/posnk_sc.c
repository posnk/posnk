#include <stdint.h>
#include <errno.h>
#include "posnk_sc.h"

uint32_t __attribute__((optimize("O0"))) nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4])
{
	int i;
	volatile syscall_params_t p;
	uint32_t pa = (uint32_t) &p;
	p.magic = SYSCALL_MAGIC;
	p.call_id = no;
	for (i = 0; i < 4; i++)
		p.param[i] = param[i];
	for (i = 0; i < 4; i++)
		p.param_size[i] = param_size[i];
	asm ("movl %0, %%eax;int $0x80;"::"r"(pa));
	errno = (int) p.sc_errno;
	
	return p.return_val;		
}	
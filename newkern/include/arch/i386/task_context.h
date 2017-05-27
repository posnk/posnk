/* 
 * arch/i386/task_context.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 */

#ifndef __ARCH_I386_TASK_CONTEXT_H__
#define __ARCH_I386_TASK_CONTEXT_H__

#include <stdint.h>
#include "arch/i386/x86.h"

struct i386_task_context {
	uint32_t				user_cs;
	uint32_t				user_eip;
	uint32_t				user_ss;
	uint32_t				user_ds;
	i386_pusha_registers_t	user_regs;
	/* We only need these three as the ISR handles the rest */
	uint32_t				kern_eip;
	uint32_t				kern_esp;
	uint32_t				kern_ebp;
	uint32_t				kern_eflags;
	uint8_t					fpu_state[512];
	int						fpu_used;
}  __attribute__((packed));

typedef struct i386_task_context i386_task_context_t;

#endif

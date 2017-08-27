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

/**
 * Structure describing the stack layout on context switches
 */
struct csstack {
	i386_pusha_registers_t	regs;
	uint32_t				eip;
}  __attribute__((packed));

/**
 * Structure containing the architecture specific state
 */
struct i386_task_context {

	/* The user state on last RING3->RING0 transition */
	uint32_t				user_cs;
	uint32_t				user_eip;
	uint32_t				user_ss;
	uint32_t				user_ds;
	i386_pusha_registers_t	user_regs;
	uint32_t				user_eflags;
	
	/* The task state on last interrupt */
	uint32_t				intr_cs;
	uint32_t				intr_eip;
	uint32_t				intr_ds;
	i386_pusha_registers_t	intr_regs;
	
	/* We only need the kernel stack, stack base and tss */
	uint32_t				kern_esp;
	uint32_t				tss_esp;
	uint8_t					fpu_state[512];
	int						fpu_used;
	
}  __attribute__((packed));

typedef struct i386_task_context i386_task_context_t;
typedef struct csstack csstack_t;

#endif

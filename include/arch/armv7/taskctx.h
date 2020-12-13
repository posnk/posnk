/*
 * arch/armv7/taskctx.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-12-2020 - Created
 */

#ifndef __ARCH_ARMV7_TASKCTX_H__
#define __ARCH_ARMV7_TASKCTX_H__

#include <stdint.h>

/**
 * Structure containing the architecture specific state
 */
struct armv7_task_context {

	/* The user state on last RING3->RING0 transition */
	uint32_t				user_pc;
	uint32_t                                user_sp;
	uint32_t                                user_lr;
	uint32_t                                user_regs[13];
	uint32_t                                user_sr;

	/* The task state on last interrupt */
	uint32_t				intr_pc;
	uint32_t                                intr_sp;
	uint32_t                                intr_lr;
	uint32_t                                intr_regs[13];
	uint32_t                                intr_sr;


	/* We only need the kernel stack, stack base and tss */
	uint32_t				kern_sp;

}  __attribute__((packed));

typedef struct armv7_task_context armv7_task_context_t;

#endif

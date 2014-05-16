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

struct i386_task_context {
/* We only need these three as the ISR handles the rest */
	uint32_t eip;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eflags;
}  __attribute__((packed));

typedef struct i386_task_context i386_task_context_t;

#endif

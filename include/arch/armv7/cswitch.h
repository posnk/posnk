/**
 * @file arch/armv7/cswitch.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 19-03-2015 - Created
 */

#ifndef __arch_armv7_cswitch_h__
#define __arch_armv7_cswitch_h__

#include <stdint.h>
#include "kernel/paging.h"

/**
 * @brief Switch kernel context
 * Pushes the callee saved state, stores stack pointer in the address
 * pointed to by _sp_out_, switches the page directory and stack, then
 * finally restores the state from the newly selected stack
 *
 * @param sp		Stack belonging to the context to switch to
 * @param sp_out	Pointer used to store the current stack pointer
 */
void armv7_context_switch ( uint32_t sp, uint32_t *sp_out );

/**
 * @brief Forks a new kernel context
 * Pushes the callee saved state, stores stack pointer in the address pointed
 * to by _sp_out_
 *
 * @param sp_out	Pointer used to store the current stack pointer
 * @param pdir_out	Pointer used to store the new page directory
 * @return 		When returning after creating the state: 1, when
 *			returning as the child: 0
 */
uint32_t armv7_context_fork ( uint32_t *sp_out, page_dir_t **pdir_out, uint32_t tempstack);

void armv7_spawnentry( void );

/**
 * The structure used for interpreting the stack contents left by the exception
 * entry routine
 */
struct a7css {
	uint32_t	regs[9];
	uint32_t	lr;
} __attribute__((packed));

typedef struct a7css armv7_cswitch_stack_t;


#endif

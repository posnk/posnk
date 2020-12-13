#
# arch/armv7/cswitch.S
#
# Part of P-OS kernel.
#
# Written by Peter Bosch <peterbosc@gmail.com>
#
# Changelog:
# 18-03-2015 - Created
#

#include "arch/armv7/cpu.h"

.section .text

#armv7_mmu_set_ttbr0,1

.global	armv7_context_switch
.global armv7_context_fork
.global armv7_context_postfork
.global armv7_spawnentry


#void armv7_context_switch( uint32_t sp, uint32_t *sp_out );
armv7_context_switch:

	# Push callee saved state
	push { r4-r12, lr }

	# Store stackpointer in output parameter
	str	sp,	[r2]

	# All state is now preserved, switch to new stack
	mov	sp,	r0

	# We are now using the correct stack

	# Restore state

	pop { r4-r12, lr }

	# Clear r0
	mov	r0,	#0

	# Return to where we came from

	bx	lr

.ltorg

# armv7_spawnentry
#    R4 callee address
#    R5 callee arg0
#    R6 interrupt status
armv7_spawnentry:
	# Load r0 with callee argument
	mov	r0,	r5

	# Jump to callee
	blx r4

	# Return to user mode
	blx	armv7_fork_exit
here:
	b here

.ltorg

#uint32_t armv7_context_fork( uint32_t *sp_out, page_directory_t **pdir_out );
armv7_context_fork:

	# Push callee saved state
	push { r4-r12, lr }

	# Store stackpointer in output parameter
	str	sp,	[r0]

	# Call address space cloning routine
	mov	r4,	r1

	# pdir * paging_create_dir (void);
	blx	paging_create_dir

	# Store new page directory pointer
	str	r0,	[r4]

	# Restore state
	pop { r4-r12, lr }

	# Return flag value so we know we are the parent
	mov	r0,	#1

	# Return to where we came from
	bx	lr

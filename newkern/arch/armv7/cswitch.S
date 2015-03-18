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


#void armv7_context_switch( uint32_t sp, uint32_t pdir_phys, uint32_t *sp_out );
armv7_context_switch:

	# Push callee saved state
	push { r4-r12, lr } 

	# Grab stack pointer and stuff it in r4
	mov	r4,	sp

	# Store stackpointer in output parameter
	str	r4,	[r3]

	# All state is now preserved, switch to new stack
	mov	sp,	r0

	# Switch page directories: 
	# THESE FUNCTIONS MUST NOT USE STACK

	mov	r1, r0
	blx	=armv7_mmu_set_ttbr0
	blx	=armv7_mmu_set_ttbr1
	
	# Page directory switched, flush TLBs

	blx	=armv7_mmu_flush_tlb

	# We are now using the correct page directory and stack

	# Restore state

	pop { r4-r12, lr }

	# Clear r0
	mov	r0,	#0

	# Return to where we came from

	bx	lr

.ltorg

#uint32_t armv7_context_fork( uint32_t *sp_out );
armv7_context_fork:

	# Push callee saved state
	push { r4-r12, lr } 

	# Grab stack pointer and stuff it in r4
	mov	r4,	sp

	# Store stackpointer in output parameter
	str	r4,	[r0]

	# Return flag value so we know we are the parent
	mov	r0,	#1

	# Return to where we came from

	bx	lr

# FORK Process:
#    call armv7_context_fork
#!   if (returned zero)
#	return in new task
#    clone stack or address space (ktask vs user task)
#    call armv7_context_postfork
#    return in parent

#void armv7_context_postfork ( void );
armv7_context_postfork:
	
	# Clean up stack mess created by parent end of fork
	# Remove process state from this copy of the stack
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 } 
	pop { r0 }

	# Return
	bx	lr	 

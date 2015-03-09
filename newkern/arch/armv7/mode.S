#
# arch/armv7/mode.S
#
# Part of P-OS kernel.
#
# Written by Peter Bosch <peterbosc@gmail.com>
#
# Changelog:
# 08-03-2015 - Created
#

#include "arch/armv7/cpu.h"

.section .text

.global armv7_get_mode

armv7_get_mode:
	mrs	r0,	cpsr_all
	mov	pc,	lr

.global armv7_set_mode_stack

# armv7_set_mode_stack: Set the stack pointer register for a different mode
# Not to be used for the current operating mode or a mode that shares the SP
# with the current operating mode.
armv7_set_mode_stack:
	# Save state
	push	{r0, r1, r2, r3}
	mrs	r2,		cpsr_all	@ Load R2 with the CPSR value
	# Calculate new CPSR
	and	r3,	r2,	#PSR_RESERVED	@ Preserve reserved CPSR bit
	and	r0,	r0,	#PSR_MODE	@ Isolate mode bits from param
	orr	r3,	r3,	r0		@ Set mode bits on new CPSR	
	# Load new CPSR
	msr	cpsr_all,	r3		@ Load CPSR with the new value
	# Load new stack pointer
	mov	sp,		r1
	# Return to original mode
	msr	cpsr_all,	r2		@Restore original CPSR value
	# Restore old values
	pop	{r0, r1, r2, r3}
	mov	pc,		lr		@Return from subroutine

.ltorg

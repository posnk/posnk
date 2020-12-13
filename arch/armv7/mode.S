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

.global armv7_enable_ints
.global armv7_disable_ints

armv7_enable_ints:
	cpsie	if
	mov	pc,	lr

armv7_disable_ints:
	cpsid	if
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

.global process_user_call

process_user_call:
	mrs	r2,		cpsr_all	@ Load R2 with the CPSR value
	# Calculate new CPSR
	mvn	r3,	#(PSR_MODE|PSR_FIQ_MASK|PSR_IRQ_MASK)
						@ Generate NOT_MODE mask
	and	r2,	r2,	r3		@ Clear mode bits from CPSR
	orr	r2,	r2,	#PSR_MODE_USR	@ Set mode bits on new CPSR
	# Load new CPSR
	msr	spsr_all,	r2		@ Load SPSR with the new value
	# Load new stack pointer
	cpsid	aif,		#PSR_MODE_SYS	@Switch to system mode to set usr/sys sp
	mov	sp,		r1
	cpsid	aif,		#PSR_MODE_SVC	@Switch back to user mode to do the call
	mov	lr,		r0
	movs	pc,		lr

.ltorg

.global stack_switch_call

stack_switch_call:
	mov	sp,		r1
	mov	pc,		r0
.ltorg

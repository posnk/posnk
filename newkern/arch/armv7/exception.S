#
# arch/armv7/exception.S
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

.global armv7_handler_table

.balign	32

armv7_vector_table:
	ldr	pc, armv7_exhl_res
	ldr	pc, armv7_exhl_und
	ldr	pc, armv7_exhl_svc
	ldr	pc, armv7_exhl_pre
	ldr	pc, armv7_exhl_abt
	b	.
	ldr	pc, armv7_exhl_irq
	ldr	pc, armv7_exhl_fiq

armv7_handler_table:
armv7_exhl_res:
	.word	0
armv7_exhl_und:
	.word	0
armv7_exhl_svc:
	.word	0
armv7_exhl_pre:
	.word	0
armv7_exhl_abt:
	.word	0
	.word	0
armv7_exhl_irq:
	.word	0
armv7_exhl_fiq:
	.word	0

.global	armv7_exception_init

armv7_exception_init:
	ldr	r0,	=armv7_vector_table
	mcr	p15, 0, r0, c12, c0, 0		@Set Vector Base Address Reg
	mov	pc,	lr			@return



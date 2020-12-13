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
.global armv7_chandler_table
.global armv7_fork_exit

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

#
# many thanks to geist on #osdev for these
#
.macro armv7_save_userstate

	srsdb	#PSR_MODE_SVC!		@ push {LR, SPSR} > SVC_STACK
	cpsid	aif, #PSR_MODE_SVC	@ switch to SVC mode

	@ We are in supervisor mode now

	push	{r0-r12, lr}		@ save lr too, need keep stack aligned

	sub	sp,	sp,	#8	@ manually decrement the stack ptr
	stmia	sp,	{sp, lr}^	@ push {sp_usr, lr_usr} > SVC_STACK

@STACK HERE:
@44		user_cpsr (SPSR)
@40		 exc_lr ( return instruction )
@3C		 svc_lr
@08		 r0-12 [13 u32]
@04		user_lr
@00	SP -> 	user_sp


.endm

.macro armv7_restore_userstate

	ldmia	sp,	{sp, lr}^	@ load {sp_usr, lr_usr} from SVC_STACK
	add	sp,	sp,	#8	@ manually increment the stack ptr

	pop	{r0-r12, lr}		@ restore lr too

	rfeia	sp!			@ pop {LR, SPSR} < SVC_STACK

.endm

armv7_exst_res:
	sub	lr,	lr,	#VEC_LRO_RESET
	armv7_save_userstate
	mov	r0,	#VEC_RESET
	mov	r1,	sp
	ldr	r4,	armv7_exhl_res_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_res_ptr:
	.word	armv7_exception_handler

armv7_exst_und:
	sub	lr,	lr,	#VEC_LRO_UNDEFINED
	armv7_save_userstate
	mov	r0,	#VEC_UNDEFINED
	mov	r1,	sp
	ldr	r4,	armv7_exhl_und_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_und_ptr:
	.word	armv7_exception_handler

armv7_exst_svc:
	sub	lr,	lr,	#VEC_LRO_SUPERVISOR_CALL
	armv7_save_userstate
	mov	r0,	#VEC_SUPERVISOR_CALL
	mov	r1,	sp
	ldr	r4,	armv7_exhl_svc_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_svc_ptr:
	.word	armv7_exception_handler

armv7_exst_pre:
	sub	lr,	lr,	#VEC_LRO_PREFETCH_ABORT
	armv7_save_userstate
	mov	r0,	#VEC_PREFETCH_ABORT
	mov	r1,	sp
	ldr	r4,	armv7_exhl_pre_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_pre_ptr:
	.word	armv7_exception_handler

armv7_exst_abt:
	sub	lr,	lr,	#VEC_LRO_DATA_ABORT
	armv7_save_userstate
	mov	r0,	#VEC_DATA_ABORT
	mov	r1,	sp
	ldr	r4,	armv7_exhl_abt_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_abt_ptr:
	.word	armv7_exception_handler

armv7_exst_irq:
	sub	lr,	lr,	#VEC_LRO_IRQ
	armv7_save_userstate
	mov	r0,	#VEC_IRQ
	mov	r1,	sp
	ldr	r4,	armv7_exhl_irq_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_irq_ptr:
	.word	armv7_exception_handler

armv7_exst_fiq:
	sub	lr,	lr,	#VEC_FIQ
	armv7_save_userstate
	mov	r0,	#VEC_FIQ
	mov	r1,	sp
	ldr	r4,	armv7_exhl_fiq_ptr
	blx	r4
	armv7_restore_userstate

armv7_exhl_fiq_ptr:
	.word	armv7_exception_handler

armv7_fork_exit:
	sub     sp,      sp,     #72
	mov	r0,  	 sp
	ldr     r4,      armv7_user_exit_ptr
	blx     r4
	armv7_restore_userstate

armv7_user_exit_ptr:
	.word armv7_user_exit
armv7_handler_table:
armv7_exhl_res:
	.word	armv7_exst_res
armv7_exhl_und:
	.word	armv7_exst_und
armv7_exhl_svc:
	.word	armv7_exst_svc
armv7_exhl_pre:
	.word	armv7_exst_pre
armv7_exhl_abt:
	.word	armv7_exst_abt
	.word	0
armv7_exhl_irq:
	.word	armv7_exst_irq
armv7_exhl_fiq:
	.word	armv7_exst_fiq

.global	armv7_exception_init

armv7_exception_init:
	ldr	r0,	=armv7_vector_table
	mcr	p15, 0, r0, c12, c0, 0		@Set Vector Base Address Reg
	mov	pc,	lr			@return



#
# arch/armv7/mmu.S
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

.global	armv7_mmu_set_dacr
.global	armv7_mmu_set_ttbcr
.global	armv7_mmu_set_ttbr0
.global	armv7_mmu_set_ttbr1
.global	armv7_mmu_enable
.global	armv7_mmu_disable
.global	armv7_mmu_flush_tlb
.global	armv7_mmu_flush_tlb_single
.global	armv7_mmu_data_abort_addr
.global armv7_mmu_data_abort_status
.global	armv7_mmu_pf_abort_addr
.global armv7_mmu_pf_abort_status
.global	armv7_mmu_translate

armv7_mmu_pf_abort_status:
	mrc	p15,	0,	r0,	c5,	c0,	1
	mov	pc,	lr

armv7_mmu_data_abort_status:
	mrc	p15,	0,	r0,	c5,	c0,	0
	mov	pc,	lr

armv7_mmu_pf_abort_addr:
	mrc	p15,	0,	r0,	c6,	c0,	2
	mov	pc,	lr

armv7_mmu_data_abort_addr:
	mrc	p15,	0,	r0,	c6,	c0,	0
	mov	pc,	lr

# void armv7_mmu_set_dacr(uint32_t dacr);
armv7_mmu_set_dacr:
	mcr	p15,	0,	r0,	c3,	c0,	0
	mov	pc,	lr

# void armv7_mmu_set_ttbr0(uint32_t ttbr0);
armv7_mmu_set_ttbr0:
	mcr	p15,	0,	r0,	c2,	c0,	0
	mov	pc,	lr

# void armv7_mmu_set_ttbr1(uint32_t ttbr1);
armv7_mmu_set_ttbr1:
	mcr	p15,	0,	r0,	c2,	c0,	1
	mov	pc,	lr

# void armv7_mmu_set_ttbcr(uint32_t ttbcr);
armv7_mmu_set_ttbcr:
	mcr	p15,	0,	r0,	c2,	c0,	2
	mov	pc,	lr

# void armv7_mmu_enable( void );
armv7_mmu_enable:
	mrc	p15,	0,	r0,	c1,	c0,	0	@Read SCTLR
	orr	r0,	r0,	#1				@Set MMU enable
	mcr	p15,	0,	r0,	c1,	c0,	0	@Write SCTLR
	mov	pc,	lr

# void armv7_mmu_enable( void );
armv7_mmu_disable:
	mrc	p15,	0,	r0,	c1,	c0,	0	@Read SCTLR
	bic	r0,	r0,	#1				@Set MMU enable
	mcr	p15,	0,	r0,	c1,	c0,	0	@Write SCTLR
	mov	pc,	lr

# void armv7_mmu_flush_tlb( void );
armv7_mmu_flush_tlb:
	and	r0,	r0,	#0
	mcr	p15,	0,	r0,	c8,	c7,	0
	mov	pc,	lr

# void armv7_mmu_flush_tlb_single( uint32_t );
armv7_mmu_flush_tlb_single:
	mcr	p15,	0,	r0,	c8,	c7,	1
	mov	pc,	lr

# uint32_t armv7_mmu_translate ( uint32_t va, uint32_t op )
armv7_mmu_translate:

	cmp	r1,	#0					@If OP == 0
	beq	amt_priv_read					@ Do a PrivRead
	cmp	r1,	#1					@Else If OP == 1
	beq	amt_priv_write					@ Do a PrivWrite
	cmp	r1,	#2					@Else If OP == 2
	beq	amt_usr_read					@ Do a UserRead
	cmp	r1,	#3					@Else If OP == 3
	beq	amt_usr_write					@ Do a UserWrite
								@Else
	mov	r0,	#0					@ Return 2^32-1
	sub	r0,	r0,	#1				
	mov	pc,	lr					@ Return

amt_priv_read:
	mcr	p15,	0,	r0,	c7,	c8,	0
	b	amt_exit

amt_priv_write:
	mcr	p15,	0,	r0,	c7,	c8,	1
	b	amt_exit

amt_usr_read:
	mcr	p15,	0,	r0,	c7,	c8,	2
	b	amt_exit

amt_usr_write:
	mcr	p15,	0,	r0,	c7,	c8,	3
	b	amt_exit

amt_exit:
	mrc	p15,	0,	r0,	c7,	c4,	0	@Read PAR value
	mov	pc,	lr					@Return PAR val.

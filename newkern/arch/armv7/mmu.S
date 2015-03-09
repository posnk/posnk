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

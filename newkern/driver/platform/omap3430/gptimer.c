/**
 * @file driver/platform/omap3430/gptimer.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 * 
 * Changelist:
 * \li 26-03-2015 - Created
 */

#include "arch/armv7/mmu.h"
#include "driver/platform/omap3430/gptimer.h"
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include <stdint.h>
#include <assert.h>

int omap3430_gptimer_int_is_overflow(	omap3430_gptimer_regs_t *regs )
{
	return regs->intstatus & OMAP3430_GPT_ISR_OVERFLOW;
} 	

int omap3430_gptimer_int_is_capture(	omap3430_gptimer_regs_t *regs )
{
	return regs->intstatus & OMAP3430_GPT_ISR_CAPTURE;
} 	

int omap3430_gptimer_int_is_match(	omap3430_gptimer_regs_t *regs )
{
	return regs->intstatus & OMAP3430_GPT_ISR_MATCH;
} 	

void omap3430_gptimer_enable_overflow_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable |= OMAP3430_GPT_IER_OVERFLOW;
}

void omap3430_gptimer_enable_capture_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable |= OMAP3430_GPT_IER_CAPTURE;
}

void omap3430_gptimer_enable_match_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable |= OMAP3430_GPT_IER_MATCH;
}

void omap3430_gptimer_disable_overflow_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable &= ~OMAP3430_GPT_IER_OVERFLOW;
}

void omap3430_gptimer_disable_capture_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable &= ~OMAP3430_GPT_IER_CAPTURE;
}

void omap3430_gptimer_disable_match_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intenable &= ~OMAP3430_GPT_IER_MATCH;
}

void omap3430_gptimer_reset_overflow_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intstatus |= OMAP3430_GPT_ISR_OVERFLOW;
}

void omap3430_gptimer_reset_capture_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intstatus |= OMAP3430_GPT_ISR_CAPTURE;
}

void omap3430_gptimer_reset_match_int ( omap3430_gptimer_regs_t *regs )
{
	regs->intstatus |= OMAP3430_GPT_ISR_MATCH;
}

void omap3430_gptimer_set_prescaler(	omap3430_gptimer_regs_t *regs,
					int prescale_log2 )
{
	assert (prescale_log2 >= 0);
	assert (prescale_log2 <= 8);
	if (!prescale_log2) {
		regs->timerctl &= ~OMAP3430_GPT_CTL_PRESCALER;
	} else {
		regs->timerctl &= ~OMAP3430_GPT_CTL_PTV(0x7);
		regs->timerctl |=  OMAP3430_GPT_CTL_PTV(prescale_log2) | 
			     OMAP3430_GPT_CTL_PRESCALER;
	}
}

void omap3430_gptimer_start_count(	omap3430_gptimer_regs_t *regs, 
					uint32_t duration, 
					int one_shot	)
{
	regs->timerctl &= ~OMAP3430_GPT_CTL_START_STOP;
	regs->loadvalue = 0xFFFFFFFFu - duration;
	regs->trigger = 0x03424234; // Trigger a reload
	if (one_shot)
		regs->timerctl &= ~OMAP3430_GPT_CTL_AUTORELOAD;
	else
		regs->timerctl |= OMAP3430_GPT_CTL_AUTORELOAD;
	regs->timerctl |= OMAP3430_GPT_CTL_START_STOP;
}

void omap3430_gptimer_stop(		omap3430_gptimer_regs_t *regs )
{
	regs->timerctl &= ~OMAP3430_GPT_CTL_START_STOP;
}

/** 
 * @brief Initialize general purpose timer
 * @param phys		The physical base address of the timer
 * @return A timer instance
 */
omap3430_gptimer_regs_t *omap3430_gptimer_initialize ( physaddr_t phys )
{
	physaddr_t frame;
	
	omap3430_gptimer_regs_t *regs = heapmm_alloc_alligned( PHYSMM_PAGE_SIZE, PHYSMM_PAGE_SIZE );

	assert ( regs != NULL );

	frame = paging_get_physical_address ( regs );

	paging_unmap ( regs );

	physmm_free_frame ( frame );

	paging_map ( regs, phys, PAGING_PAGE_FLAG_NOCACHE );

	earlycon_printf(
		"omap_gptimer: Initializing timer revision %i.%i\n"
		, (regs->revision & 0xF0) >> 4, regs->revision & 0xF);

	regs->sysconfig = 	OMAP3430_GPT_SCFG_CLOCK_BOTHALWAYS |
				OMAP3430_GPT_SCFG_IDLEMODE_NEVER;
	
	regs->intenable = 	0;
	regs->wupenable = 	0;
	regs->intstatus = 	OMAP3430_GPT_ISR_CAPTURE | 
				OMAP3430_GPT_ISR_OVERFLOW |
				OMAP3430_GPT_ISR_MATCH;
	
	regs->timerctl  =	OMAP3430_GPT_CTL_GPO_CFG;

	return regs;
}

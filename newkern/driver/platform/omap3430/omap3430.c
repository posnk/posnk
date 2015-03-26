/**
 * @file driver/platform/omap3430/omap3430.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 * 
 * Changelog:
 * \li 23-03-2015 - Created
 *
 */

#include "driver/platform/omap3430/omap3430.h"
#include "driver/platform/omap3430/gptimer.h"
#include "driver/platform/omap3430/mpu_intc.h"
#include "kernel/interrupt.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include <string.h>
#include <assert.h>

omap3430_gptimer_regs_t 	*omap3430_p_systimer;
omap3430_mpu_intc_regs_t	*omap3430_p_intc;

int omap3430_systick_isr( irq_id_t id, void *context )
{
	assert ( id == OMAP3430_MPU_IRQ_GPTIMER1 );

	if ( omap3430_gptimer_int_is_overflow ( omap3430_p_systimer ) ) {
		debugcon_printf("tick\n");
		omap3430_gptimer_reset_overflow_int ( omap3430_p_systimer );
	}
	if ( omap3430_gptimer_int_is_match ( omap3430_p_systimer ) ) {
		omap3430_gptimer_reset_match_int ( omap3430_p_systimer );
	}
	if ( omap3430_gptimer_int_is_capture ( omap3430_p_systimer ) ) {
		omap3430_gptimer_reset_capture_int ( omap3430_p_systimer );
	}

	return 1;
}

void platform_initialize( void )
{
	omap3430_p_intc = omap3430_mpu_intc_initialize( OMAP3430_MPU_INTC_BASE );
	omap3430_p_systimer = omap3430_gptimer_initialize ( OMAP3430_GPTIMER0_BASE );

	interrupt_register_handler(OMAP3430_MPU_IRQ_GPTIMER1, &omap3430_systick_isr, NULL);	

	omap3430_mpu_intc_config_irq( omap3430_p_intc, OMAP3430_MPU_IRQ_GPTIMER1, 0, 1 );
	omap3430_mpu_intc_unmask_int( omap3430_p_intc, OMAP3430_MPU_IRQ_GPTIMER1 );

	omap3430_gptimer_set_prescaler( omap3430_p_systimer, 0);
	omap3430_gptimer_enable_overflow_int ( omap3430_p_systimer );
	omap3430_gptimer_start_count ( omap3430_p_systimer, 600, 0 );

}

int platform_get_interrupt_id( int int_channel )
{
	return omap3430_mpu_intc_get_active_int ( omap3430_p_intc, int_channel );
}

void platform_end_of_interrupt( int int_channel, int int_id )
{
	
	omap3430_mpu_intc_acknowledge_int ( omap3430_p_intc, int_channel );
}

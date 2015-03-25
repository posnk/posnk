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
#include "driver/platform/omap3430/mpu_intc.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"

omap3430_mpu_intc_regs_t *omap3430_p_intc;

void platform_initialize( void )
{
	omap3430_p_intc = omap3430_mpu_intc_initialize( OMAP3430_MPU_INTC_BASE );
}

int platform_get_interrupt_id( int int_channel )
{
	return omap3430_mpu_intc_get_active_int ( omap3430_p_intc, int_channel );
}

void platform_end_of_interrupt( int int_channel, int int_id )
{
	
	omap3430_mpu_intc_acknowledge_int ( omap3430_p_intc, int_channel );
}

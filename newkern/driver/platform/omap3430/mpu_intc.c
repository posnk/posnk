/**
 * @file driver/platform/omap3430/mpu_intc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 * 
 * Changelist:
 * \li 22-03-2015 - Created
 */

#include "arch/armv7/mmu.h"
#include "driver/platform/omap3430/mpu_intc.h"
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include <stdint.h>
#include <assert.h>

/**
 * @brief Prevent a specific interrupt from reaching the MPU
 * @param regs		A (virtual) pointer to the intc mmio registers
 * @param int_num	The interrupt to mask.
 */
void omap3430_mpu_intc_mask_int ( omap3430_mpu_intc_regs_t *regs, int int_num )
{
	assert ( ( int_num >= 0 ) && ( int_num < 96 ) );
	regs->bank[int_num >> 5].mir_set = 1 << ( int_num & 0x1F );
}

/**
 * @brief Allow a specific interrupt to reach the MPU
 * @param regs		A (virtual) pointer to the intc mmio registers
 * @param int_num	The interrupt to mask.
 */
void omap3430_mpu_intc_unmask_int ( omap3430_mpu_intc_regs_t *regs, int int_num )
{
	assert ( ( int_num >= 0 ) && ( int_num < 96 ) );
	regs->bank[int_num >> 5].mir_clear = 1 << ( int_num & 0x1F );
}

/**
 * @brief Configure an external interrupt on the interrupt controller
 * @param regs 		A (virtual) pointer to the intc mmio registers
 * @param int_num	The interrupt to configure
 * @param fiq		Whether the interrupt is to be signaled as a FIQ
 * @param priority	The priority to assign to the interrupt
 */
void omap3430_mpu_intc_config_irq ( omap3430_mpu_intc_regs_t *regs,
					int int_num,
					int fiq, 
					int priority )
{
	assert ( ( int_num >= 0 ) && ( int_num < 96 ) );
	if (fiq)
		regs->ilr[int_num] = 
			OMAP3430_MPU_INTC_ILR_PRIORITY(priority) | 
			OMAP3430_MPU_INTC_ILR_FIQnIRQ;
	else
		regs->ilr[int_num] = 
			OMAP3430_MPU_INTC_ILR_PRIORITY(priority);
}

/**
 * @brief Determine which interrupt is currently active
 * @param regs 		A (virtual) pointer to the intc mmio registers
 * @param fiq		Whether the interrupt was a FIQ
 * @return The interrupt number that was active or -1 in case of a spurious IRQ
 */
int omap3430_mpu_intc_get_active_int ( 	omap3430_mpu_intc_regs_t *regs, 
					int fiq  )
{
	uint32_t sir;
	if ( fiq )
		sir = regs->sir_fiq;
	else
		sir = regs->sir_irq;
	if ( sir & OMAP3430_MPU_INTC_SIR_SPURIOUSFLAG )
		return -1;
	else
		return sir & OMAP3430_MPU_INTC_SIR_ACTIVEINT( sir );
}

/**
 * @brief Acknowledge and reset an interrupt
 * @param regs 		A (virtual) pointer to the intc mmio registers
 * @param fiq		Whether the interrupt was a FIQ
 */
void omap3430_mpu_intc_acknowledge_int ( omap3430_mpu_intc_regs_t *regs, int fiq )
{
	regs->control |= (fiq) ? (OMAP3430_MPU_INTC_CTL_NEWFIQAGR) : 
		       		 (OMAP3430_MPU_INTC_CTL_NEWIRQAGR);
	armv7_mmu_data_barrier();;	
}

/** 
 * @brief Initialize MPU interrupt controller and disable all hardware ints
 * @param phys		The physical base address of the intc
 * @return A interrupt controller instance
 */
omap3430_mpu_intc_regs_t *omap3430_mpu_intc_initialize ( physaddr_t phys )
{
	physaddr_t frame;
	
	omap3430_mpu_intc_regs_t *regs = heapmm_alloc( PHYSMM_PAGE_SIZE );

	assert ( regs != NULL );

	frame = paging_get_physical_address ( regs );

	paging_unmap ( regs );

	physmm_free_frame ( frame );

	paging_map ( regs, phys, PAGING_PAGE_FLAG_NOCACHE );

	earlycon_printf(
		"omap_intc: Initializing interrupt controller revision %i.%i\n"
		, (regs->revision & 0xF0) >> 4, regs->revision & 0xF);

	regs->protection = 1;
	regs->threshold  = 0;

	regs->bank[0].isr_clear = 0xFFFFFFFF;
	regs->bank[0].mir_set	= 0xFFFFFFFF;
	regs->bank[1].isr_clear = 0xFFFFFFFF;
	regs->bank[1].mir_set	= 0xFFFFFFFF;
	regs->bank[2].isr_clear = 0xFFFFFFFF;
	regs->bank[2].mir_set 	= 0xFFFFFFFF;

	return regs;
}

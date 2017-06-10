/**
 * @file driver/platform/omap3430/mpu_intc.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelist:
 * \li 22-03-2015 - Created
 */

#ifndef __driver_platform_omap3430_mpu_intc__
#define __driver_platform_omap3430_mpu_intc__

/* We need the defined size types */
#include <stdint.h>
#include "kernel/physmm.h"

#define OMAP3430_MPU_INTC_SCFG_SOFTRESET	(0x00000002)
#define OMAP3430_MPU_INTC_SCFG_AUTOIDLE 	(0x00000001)

#define OMAP3430_MPU_INTC_SSTAT_RESETDONE	(0x00000001)

#define OMAP3430_MPU_INTC_SIR_SPURIOUSFLAG	(0xFFFFFF80)
#define OMAP3430_MPU_INTC_SIR_ACTIVEINT(VaL)	(0x7F & (VaL))

#define OMAP3430_MPU_INTC_CTL_NEWFIQAGR		(0x00000002)
#define OMAP3430_MPU_INTC_CTL_NEWIRQAGR		(0x00000001)

#define OMAP3430_MPU_INTC_IDL_TURBO		(0x00000002)
#define OMAP3430_MPU_INTC_IDL_FUNCIDLE		(0x00000001)

#define OMAP3430_MPU_INTC_PRI_SPURIOUSFLAG	(0xFFFFFFC0)
#define OMAP3430_MPU_INTC_PRI_ACTIVEPRI(VaL)	(0x3F & (VaL))

#define OMAP3430_MPU_INTC_ILR_FIQnIRQ		(0x00000001)
#define OMAP3430_MPU_INTC_ILR_PRIORITY(VaL)	((0x3F & (VaL)) << 2)

struct omap3430_mpu_intc_bank {

	/* 0x080 + 0x020 * n - BITFIELD INT STATUS BEFORE MASK */
	uint32_t	itr;
	/* 0x084 + 0x020 * n - BITFIELD INT MASK */
	uint32_t	mir;
	/* 0x088 + 0x020 * n - BITFIELD INT MASK CLEAR*/
	uint32_t	mir_clear;
	/* 0x08C + 0x020 * n - BITFIELD INT MASK SET */
	uint32_t	mir_set;
	/* 0x090 + 0x020 * n - BITFIELD INT SET SOFTWARE IRQ */
	uint32_t	isr_set;
	/* 0x094 + 0x020 * n - BITFIELD INT CLEAR SOFTWARE IRQ */
	uint32_t	isr_clear;
	/* 0x098 + 0x020 * n - BITFIELD INT STATUS AFTER IRQM */
	uint32_t	pending_irq;
	/* 0x09C + 0x020 * n - BITFIELD INT STATUS AFTER FIQM */
	uint32_t	pending_fiq;

} __attribute__((aligned(8)));

typedef struct omap3430_mpu_intc_bank	omap3430_mpu_intc_bnk_t;

struct omap3430_mpu_intc_regs {
	/* 0x000 - Interrupt controller block revision */
	uint32_t		revision;

	uint32_t		pad1[3];

	/* 0x010 - Module interface control */
	uint32_t		sysconfig;
	/* 0x014 - Module interface status */
	uint32_t		sysstatus;
	
	uint32_t		pad2[10];	

	/* 0x040 - Active IRQ */
	uint32_t		sir_irq;
	/* 0x044 - Active FIQ */
	uint32_t		sir_fiq;
	/* 0x048 - New Interrupt Agreement register */
	uint32_t		control;
	/* 0x04C - Protection */
	uint32_t		protection;
	/* 0x050 - Clock auto idle and sync clock */
	uint32_t		idle;
	
	uint32_t		pad3[3];

	/* 0x060 - Active IRQ Priority */
	uint32_t		irq_priority;
	/* 0x064 - Active FIQ Priority */
	uint32_t		fiq_priority;
	/* 0x068 - Priority threshold*/
	uint32_t		threshold;

	uint32_t		pad4[4];

	/* 0x080 */
	omap3430_mpu_intc_bnk_t	bank[3];

	/* 0x100 + 0x004 * m - Interrupt routing and priority */
	uint32_t		ilr[96];

} __attribute__((aligned(4)));

typedef struct omap3430_mpu_intc_regs	omap3430_mpu_intc_regs_t;

void omap3430_mpu_intc_mask_int ( omap3430_mpu_intc_regs_t *regs, int int_num );
void omap3430_mpu_intc_unmask_int ( omap3430_mpu_intc_regs_t *regs, int int_num );
void omap3430_mpu_intc_config_irq ( omap3430_mpu_intc_regs_t *regs, 
					int int_num,
					int fiq,
					int priority );
int  omap3430_mpu_intc_get_active_int ( omap3430_mpu_intc_regs_t *regs, int fiq );
void omap3430_mpu_intc_acknowledge_int ( omap3430_mpu_intc_regs_t *regs, int fiq);

omap3430_mpu_intc_regs_t *omap3430_mpu_intc_initialize ( physaddr_t phys );

#endif

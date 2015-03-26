/**
 * @file driver/platform/omap3430/gptimer.h
 * 
 * Part of P-OS kernel.
 * 
 * Written by Peter Bosch <peterbosc@gmail.com>
 * 
 * Changelog:
 * \li 23-03-2014 - Created
 */

#ifndef __driver_platform_omap3430_gptimer__
#define __driver_platform_omap3430_gptimer__

#define OMAP3430_GPT_SCFG_CLOCK_BOTHALWAYS	(0x00000300)
#define OMAP3430_GPT_SCFG_CLOCK_L4ALWAYS	(0x00000100)
#define OMAP3430_GPT_SCFG_CLOCK_FUNCALWAYS	(0x00000200)
#define OMAP3430_GPT_SCFG_CLOCK_BOTHGATED	(0x00000000)

#define OMAP3430_GPT_SCFG_EMUFREE		(0x00000020)

#define OMAP3430_GPT_SCFG_IDLEMODE_FORCE	(0x00000000)
#define OMAP3430_GPT_SCFG_IDLEMODE_NEVER	(0x00000008)
#define OMAP3430_GPT_SCFG_IDLEMODE_SMART	(0x00000010)

#define OMAP3430_GPT_SCFG_ENAWAKE		(0x00000004)
#define OMAP3430_GPT_SCFG_SOFTRESET		(0x00000002)
#define OMAP3430_GPT_SCFG_AUTOIDLE		(0x00000001)

#define OMAP3430_GPT_SSTAT_RESETDONE		(0x00000001)

#define OMAP3430_GPT_ISR_CAPTURE		(0x00000004)
#define OMAP3430_GPT_ISR_OVERFLOW		(0x00000002)
#define OMAP3430_GPT_ISR_MATCH			(0x00000001)

#define OMAP3430_GPT_IER_CAPTURE		(0x00000004)
#define OMAP3430_GPT_IER_OVERFLOW		(0x00000002)
#define OMAP3430_GPT_IER_MATCH			(0x00000001)

#define OMAP3430_GPT_WER_CAPTURE		(0x00000004)
#define OMAP3430_GPT_WER_OVERFLOW		(0x00000002)
#define OMAP3430_GPT_WER_MATCH			(0x00000001)


#define OMAP3430_GPT_CTL_GPO_CFG		(0x00004000)
#define OMAP3430_GPT_CTL_CAPT_MODE		(0x00002000)
#define OMAP3430_GPT_CTL_TOGGLEMOD		(0x00001000)
#define OMAP3430_GPT_CTL_TRG(VaL)		((VaL & 0x3) << 10)
#define OMAP3430_GPT_CTL_TCM(VaL)		((VaL & 0x3) << 8)
#define OMAP3430_GPT_CTL_PWM_DEFAULTHIGH	(0x00000080)
#define OMAP3430_GPT_CTL_COMPARE		(0x00000040)
#define OMAP3430_GPT_CTL_PRESCALER		(0x00000020)
#define OMAP3430_GPT_CTL_PTV(VaL)		((VaL & 0x7) << 2)
#define OMAP3430_GPT_CTL_AUTORELOAD		(0x00000002)
#define OMAP3430_GPT_CTL_START_STOP		(0x00000001)

struct omap3430_gptimer_regs {
	uint32_t	revision;
	uint32_t	gap1[3];
	uint32_t	sysconfig;
	uint32_t	sysstatus;
	uint32_t	intstatus;
	uint32_t	intenable;
	uint32_t	wupenable;
	uint32_t	timerctl;
	uint32_t	counter;
	uint32_t	loadvalue;
	uint32_t	trigger;
	uint32_t	writepend;
	uint32_t	match;
	uint32_t	capture0;
	uint32_t	interfctl;
	uint32_t	capture1;
	uint32_t	pincrement;
	uint32_t	nincrement;
	uint32_t	cvr_value;
	uint32_t	ovf_value;
	uint32_t	ovf_count;
} __attribute__((aligned(4)));

typedef struct omap3430_gptimer_regs omap3430_gptimer_regs_t;

int omap3430_gptimer_int_is_overflow(  omap3430_gptimer_regs_t *regs );

int omap3430_gptimer_int_is_capture(   omap3430_gptimer_regs_t *regs );

int omap3430_gptimer_int_is_match(     omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_enable_overflow_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_enable_capture_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_enable_match_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_disable_overflow_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_disable_capture_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_disable_match_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_reset_overflow_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_reset_capture_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_reset_match_int ( omap3430_gptimer_regs_t *regs );

void omap3430_gptimer_set_prescaler(    omap3430_gptimer_regs_t *regs,
                                        int prescale_log2 );

void omap3430_gptimer_start_count(      omap3430_gptimer_regs_t *regs,
                                        uint32_t duration,
                                        int one_shot    );

void omap3430_gptimer_stop(             omap3430_gptimer_regs_t *regs );

omap3430_gptimer_regs_t *omap3430_gptimer_initialize ( physaddr_t phys );

#endif

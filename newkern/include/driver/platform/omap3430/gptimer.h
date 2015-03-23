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
} __attribute__((__packed__));

#endif

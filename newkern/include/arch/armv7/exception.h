/**
 * arch/armv7/exception.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 10-03-2015 - Created
 */

#ifndef __ARCH_ARMV7_EXCEPTION_H__
#define __ARCH_ARMV7_EXCEPTION_H__

struct a7est {
	uint32_t	usr_sp;
	uint32_t	usr_lr;
	uint32_t	usr_regs[13];
	uint32_t	svc_lr;
	uint32_t	exc_lr;
	uint32_t	usr_psr;
} __attribute__((packed));

typedef struct a7est armv7_exception_state_t;

void armv7_exception_init();

void armv7_exception_handler(uint32_t vec_id, armv7_exception_state_t *state);

#endif

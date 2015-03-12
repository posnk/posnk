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

void armv7_exception_init();

extern uint32_t armv7_handler_table[8];

#endif

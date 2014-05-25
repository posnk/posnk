/* 
 * arch/i386/pit.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 * Might have been derived from James Molloy's 
 * tutorial but I'm not quite sure
 * 
 * Changelog:
 * 2010       - Created
 * 24-05-2014 - Cleaned up
 */

#ifndef __ARCH_I386_PIT_H__
#define __ARCH_I386_PIT_H__

#include <stdint.h>

#define	I386_PIT_OCW_BINCOUNT_BINARY	0	//0		//! Use when setting I86_PIT_OCW_MASK_BINCOUNT
#define	I386_PIT_OCW_BINCOUNT_BCD	1	//1
#define I386_PIT_OCW_MODE_TERMINALCOUNT	0	//0000		//! Use when setting I86_PIT_OCW_MASK_MODE
#define	I386_PIT_OCW_MODE_ONESHOT	0x2	//0010
#define	I386_PIT_OCW_MODE_RATEGEN	0x4	//0100
#define	I386_PIT_OCW_MODE_SQUAREWAVEGEN	0x6	//0110
#define	I386_PIT_OCW_MODE_SOFTWARETRIG	0x8	//1000
#define	I386_PIT_OCW_MODE_HARDWARETRIG	0xA	//1010
#define	I386_PIT_OCW_RL_LATCH		0	//000000	//! Use when setting I86_PIT_OCW_MASK_RL
#define	I386_PIT_OCW_RL_LSBONLY		0x10	//010000
#define	I386_PIT_OCW_RL_MSBONLY		0x20	//100000
#define	I386_PIT_OCW_RL_DATA		0x30	//110000
#define	I386_PIT_OCW_COUNTER0		0	//00000000	//! Use when setting I86_PIT_OCW_MASK_COUNTER
#define	I386_PIT_OCW_COUNTER1		0x40	//01000000
#define	I386_PIT_OCW_COUNTER2		0x80	//10000000
#define	I386_PIT_REG_COUNTER0		0x40
#define	I386_PIT_REG_COUNTER1		0x41
#define	I386_PIT_REG_COUNTER2		0x42
#define	I386_PIT_REG_COMMAND		0x43

void i386_pit_setup(uint32_t freq, int counter, uint8_t mode);

#endif

/* 
 * arch/i386/pit.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2010       - Created
 * 24-05-2014 - Cleaned up
 */


#include "arch/i386/pit.h"
#include "arch/i386/x86.h"
#include "kernel/time.h"

uint8_t i386_pit_data_ports[] = {I386_PIT_REG_COUNTER0, I386_PIT_REG_COUNTER1, I386_PIT_REG_COUNTER2};
uint8_t i386_pit_ocw_ctrs[]   = {I386_PIT_OCW_COUNTER0, I386_PIT_OCW_COUNTER1, I386_PIT_OCW_COUNTER2};

void i386_pit_send_command (uint8_t cmd) {
	i386_outb (I386_PIT_REG_COMMAND, cmd);
}

void i386_pit_send_data (uint8_t counter, uint8_t data) {
	i386_outb (i386_pit_data_ports[counter], data);
}

void i386_pit_setup(uint32_t freq, int counter, uint8_t mode) {

	uint16_t divisor;
	if (freq == 0)
		return;

	if (counter == 0)
		timer_freq = (ticks_t) freq;

	if (counter == 0)
		timer_mfreq = (ticks_t) (freq / 1000);
	
	divisor =  1193180 / freq;
	i386_pit_send_command (mode | I386_PIT_OCW_RL_DATA | i386_pit_ocw_ctrs[counter]);
	//! set frequency rate
	i386_pit_send_data(0, divisor & 0xff);
	i386_pit_send_data(0, (divisor >> 8) & 0xff);
}

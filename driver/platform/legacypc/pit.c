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


#include "driver/platform/legacypc/pit.h"
#include "arch/i386/x86.h"
#include "kernel/time.h"

static const uint8_t DATA_PORTS[] = {
	PIT_REG_COUNTER0, 
	PIT_REG_COUNTER1, 
	PIT_REG_COUNTER2 };

static const uint8_t OCW_CTRS[]   = {
	PIT_OCW_COUNTER0, 
	PIT_OCW_COUNTER1, 
	PIT_OCW_COUNTER2 };

static void pit_send_command ( uint8_t cmd ) {
	i386_outb ( PIT_REG_COMMAND, cmd );
}

static void pit_send_data ( uint8_t counter, uint8_t data ) {
	i386_outb ( DATA_PORTS[counter], data );
}

void pit_setup( uint32_t freq, int counter, uint8_t mode ) {

	uint16_t divisor;
	if (freq == 0)
		return;

	if (counter == 0)
		timer_freq = (ticks_t) freq;

	if (counter == 0)
		timer_mfreq = (ticks_t) (freq / 1000);
	
	divisor =  1193180 / freq;
	pit_send_command (
		mode | PIT_OCW_RL_DATA | OCW_CTRS[counter]);

	//! set frequency rate
	pit_send_data(0, divisor & 0xff);
	pit_send_data(0, (divisor >> 8) & 0xff);
}

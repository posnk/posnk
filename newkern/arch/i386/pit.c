#include "pit.h"
#include "dri.h"
#include "x86.h"
uint32_t _pit_ticks = 0;

void x86_pit_isr (registers_t regs) {
	//! increment tick count
	_pit_ticks++;
}

void x86_pit_send_command (uint8_t cmd) {
	_outp (I86_PIT_REG_COMMAND, cmd);
}

//! send data to a counter
void x86_pit_send_data (uint8_t data, uint8_t counter) {

	uint8_t	port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	_outp (port, data);
}

//! read data from counter
uint8_t x86_pit_read_data (uint16_t counter) {

	uint8_t	port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	return _inp (port);
}

void pit_initialize(){
	dri_register_interrupt(0,(dri_interrupt_handler) x86_pit_isr);
}

void pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode) {

	uint16_t divisor = 1193180 / freq;
	uint8_t ocw=0;

	if (freq==0)
		return;

	//! send operational command
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	x86_pit_send_command (ocw);

	//! set frequency rate
	x86_pit_send_data (divisor & 0xff, 0);
	x86_pit_send_data ((divisor >> 8) & 0xff, 0);

	//! reset tick count
	_pit_ticks=0;
}
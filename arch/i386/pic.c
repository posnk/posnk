/* 
 * arch/i386/pic.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2010       - Created
 * 03-04-2014 - Cleaned up
 */

#include "arch/i386/pic.h"
#include "arch/i386/x86.h"

void i386_pic_send_command (uint8_t cmd, uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? I386_PIC2_REG_COMMAND : I386_PIC1_REG_COMMAND;
	if (picNum > 1)
		return;
	i386_outb (reg, cmd);
}

void i386_pic_send_data (uint8_t data, uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? I386_PIC2_REG_DATA : I386_PIC1_REG_DATA;
	if (picNum > 1)
		return;
	i386_outb (reg, data);
}

uint8_t i386_pic_read_data (uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? I386_PIC2_REG_DATA : I386_PIC1_REG_DATA;
	if (picNum > 1)
		return 0;
	return i386_inb (reg);
}

uint8_t i386_pic_read_cmd (uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? I386_PIC2_REG_COMMAND : I386_PIC1_REG_COMMAND;
	if (picNum > 1)
		return 0;
	return i386_inb (reg);
}


//! Initialize pic
void i386_pic_initialize () {

	uint8_t		icw	= 0;

	//! Begin initialization of PIC

	icw = (icw & ~I386_PIC_ICW1_MASK_INIT) | I386_PIC_ICW1_INIT_YES;
	icw = (icw & ~I386_PIC_ICW1_MASK_IC4) | I386_PIC_ICW1_IC4_EXPECT;

	i386_pic_send_command (icw, 0);
	i386_pic_send_command (icw, 1);

	//! Send initialization control word 2. This is the base addresses of the irq's

	i386_pic_send_data (I386_PIC0_IRQ_BASE, 0);
	i386_pic_send_data (I386_PIC1_IRQ_BASE, 1);

	//! Send initialization control word 3. This is the connection between master and slave.
	//! ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//! ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	i386_pic_send_data (0x04, 0);
	i386_pic_send_data (0x02, 1);

	//! Send Initialization control word 4. Enables i86 mode

	icw = (icw & ~I386_PIC_ICW4_MASK_UPM) | I386_PIC_ICW4_UPM_86MODE;

	i386_pic_send_data (icw, 0);
	i386_pic_send_data (icw, 1);
}

int i386_pic_read_isr( int id ) {
	i386_pic_send_command( 0x0a,id );
	return i386_pic_read_cmd( id );
}

int i386_read_isr() {
	return i386_pic_read_isr(0) | (i386_pic_read_isr(1) << 8);
}

void i386_send_end_of_interrupt(int pic_id){
	i386_pic_send_command(I386_PIC_OCW2_MASK_EOI,pic_id);
}

void i386_interrupt_done (int id)
{
	i386_send_end_of_interrupt(0);
	if (id > 7)
		i386_send_end_of_interrupt(1);
}

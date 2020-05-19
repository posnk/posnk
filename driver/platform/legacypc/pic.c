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

#include "driver/platform/legacypc/pic.h"
#include "arch/i386/x86.h"

static void pic_send_command (uint8_t cmd, uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? PIC2_REG_COMMAND : PIC1_REG_COMMAND;
	if (picNum > 1)
		return;
	i386_outb (reg, cmd);
}

static void pic_send_data (uint8_t data, uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	if (picNum > 1)
		return;
	i386_outb (reg, data);
}

static uint8_t pic_read_data (uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	if (picNum > 1)
		return 0;
	return i386_inb (reg);
}

static uint8_t pic_read_cmd (uint8_t picNum) {
	uint8_t	reg = (picNum==1) ? PIC2_REG_COMMAND : PIC1_REG_COMMAND;
	if (picNum > 1)
		return 0;
	return i386_inb (reg);
}


//! Initialize pic
void pic_initialize () {

	uint8_t		icw	= 0;

	//! Begin initialization of PIC

	icw = (icw & ~PIC_ICW1_MASK_INIT) | PIC_ICW1_INIT_YES;
	icw = (icw & ~PIC_ICW1_MASK_IC4) | PIC_ICW1_IC4_EXPECT;

	pic_send_command (icw, 0);
	pic_send_command (icw, 1);

	//! Send initialization control word 2. This is the base addresses of the irq's

	pic_send_data (PIC0_IRQ_BASE, 0);
	pic_send_data (PIC1_IRQ_BASE, 1);

	//! Send initialization control word 3. This is the connection between master and slave.
	//! ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//! ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	pic_send_data (0x04, 0);
	pic_send_data (0x02, 1);

	//! Send Initialization control word 4. Enables i86 mode

	icw = (icw & ~PIC_ICW4_MASK_UPM) | PIC_ICW4_UPM_86MODE;

	pic_send_data (icw, 0);
	pic_send_data (icw, 1);
}

int pic_read_isr( int id ) {
	pic_send_command( 0x0a,id );
	return pic_read_cmd( id );
}

void pic_send_end_of_interrupt(int pic_id) {
	pic_send_command(PIC_OCW2_MASK_EOI,pic_id);
}


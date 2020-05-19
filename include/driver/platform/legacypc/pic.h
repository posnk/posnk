/* 
 * arch/i386/pic.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2010       - Created
 * 03-04-2014 - Cleaned up
 */

#ifndef __ARCH_PIC_H__
#define __ARCH_PIC_H__
#include <stdint.h>

#define		PIC_OCW2_MASK_L1				1		//00000001	//Level 1 interrupt level
#define		PIC_OCW2_MASK_L2				2		//00000010	//Level 2 interrupt level
#define		PIC_OCW2_MASK_L3				4		//00000100	//Level 3 interrupt level
#define		PIC_OCW2_MASK_EOI				0x20		//00100000	//End of Interrupt command
#define		PIC_OCW2_MASK_SL				0x40		//01000000	//Select command
#define		PIC_OCW2_MASK_ROTATE			0x80		//10000000	//Rotation command
#define		PIC_OCW3_MASK_RIS				1		//00000001
#define		PIC_OCW3_MASK_RIR				2		//00000010
#define		PIC_OCW3_MASK_MODE				4		//00000100
#define		PIC_OCW3_MASK_SMM				0x20		//00100000
#define		PIC_OCW3_MASK_ESMM				0x40		//01000000
#define		PIC_OCW3_MASK_D7				0x80		//10000000
#define		PIC1_REG_COMMAND				0x20			// command register
#define		PIC1_REG_STATUS				0x20			// status register
#define		PIC1_REG_DATA				0x21			// data register
#define		PIC1_REG_IMR				0x21			// interrupt mask register (imr)
#define		PIC2_REG_COMMAND				0xA0			// ^ see above register names
#define		PIC2_REG_STATUS				0xA0
#define		PIC2_REG_DATA				0xA1
#define		PIC2_REG_IMR				0xA1
#define		PIC_ICW1_MASK_IC4				0x1	//00000001	// Expect ICW 4 bit
#define		PIC_ICW1_MASK_SNGL				0x2	//00000010	// Single or Cascaded
#define		PIC_ICW1_MASK_ADI				0x4	//00000100	// Call Address Interval
#define		PIC_ICW1_MASK_LTIM				0x8	//00001000	// Operation Mode
#define		PIC_ICW1_MASK_INIT				0x10	//00010000	// Initialization Command
#define		PIC_ICW1_IC4_EXPECT			1	//1		//Use when setting PIC_ICW1_MASK_IC4
#define		PIC_ICW1_IC4_NO				0	//0
#define		PIC_ICW1_SNGL_YES				2	//10		//Use when setting PIC_ICW1_MASK_SNGL
#define		PIC_ICW1_SNGL_NO				0	//00
#define		PIC_ICW1_ADI_CALLINTERVAL4			4	//100		//Use when setting PIC_ICW1_MASK_ADI
#define		PIC_ICW1_ADI_CALLINTERVAL8			0	//000
#define		PIC_ICW1_LTIM_LEVELTRIGGERED		8	//1000		//Use when setting PIC_ICW1_MASK_LTIM
#define		PIC_ICW1_LTIM_EDGETRIGGERED		0	//0000
#define		PIC_ICW1_INIT_YES				0x10	//10000		//Use when setting PIC_ICW1_MASK_INIT
#define		PIC_ICW1_INIT_NO				0	//00000
#define		PIC_ICW4_MASK_UPM				0x1	//00000001	// Mode
#define		PIC_ICW4_MASK_AEOI				0x2	//00000010	// Automatic EOI
#define		PIC_ICW4_MASK_MS				0x4	//00000100	// Selects buffer type
#define		PIC_ICW4_MASK_BUF				0x8	//00001000	// Buffered mode
#define		PIC_ICW4_MASK_SFNM				0x10	//00010000	// Special fully-nested mode
#define		PIC_ICW4_UPM_86MODE			1	//1		//Use when setting PIC_ICW4_MASK_UPM
#define		PIC_ICW4_UPM_MCSMODE			0	//0
#define		PIC_ICW4_AEOI_AUTOEOI			2	//10		//Use when setting PIC_ICW4_MASK_AEOI
#define		PIC_ICW4_AEOI_NOAUTOEOI			0	//00
#define		PIC_ICW4_MS_BUFFERMASTER			4	//100		//Use when setting PIC_ICW4_MASK_MS
#define		PIC_ICW4_MS_BUFFERSLAVE			0	//000
#define		PIC_ICW4_BUF_MODEYES			8	//1000		//Use when setting PIC_ICW4_MASK_BUF
#define		PIC_ICW4_BUF_MODENO			0	//0000
#define		PIC_ICW4_SFNM_NESTEDMODE			0x10	//10000		//Use when setting PIC_ICW4_MASK_SFNM
#define		PIC_ICW4_SFNM_NOTNESTED			0	//00000

#define		IRQ_BASE					0x20
#define		PIC0_IRQ_BASE				(IRQ_BASE)
#define		PIC1_IRQ_BASE				(IRQ_BASE + 8)

void pic_initialize ();
int pic_read_isr( int id );
void pic_send_end_of_interrupt(int pic_id);

#endif

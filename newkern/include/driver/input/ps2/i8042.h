/**
 * driver/input/ps2/i8042.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-07-2014 - Created
 */

#ifndef __DRIVER_INPUT_PS2_8042_H__
#define __DRIVER_INPUT_PS2_8042_H__

#include <stdint.h>

#define	I8042_DATA_PORT			(0x60)
#define	I8042_CMD_PORT			(0x64)
#define	I8042_STATUS_PORT		(0x64)

#define I8042_STATUS_FLAG_IN_FULL	(1<<0)//1
#define I8042_STATUS_FLAG_OUT_FULL	(1<<1)//2
#define I8042_STATUS_FLAG_SYSTEM	(1<<2)//4
#define I8042_STATUS_FLAG_CMD_DATA	(1<<3)//8
#define I8042_STATUS_FLAG_UNK_1		(1<<4)//10
#define I8042_STATUS_FLAG_AUX_DATA	(1<<5)//20
#define I8042_STATUS_FLAG_TIMEOUT	(1<<6)//40
#define I8042_STATUS_FLAG_PARITY	(1<<7)//80

#define I8042_CMD_READ_RAM(AdDr)	(0x20 | (AdDr & 0x1F))
#define I8042_CMD_WRITE_RAM(AdDr)	(0x60 | (AdDr & 0x1F))

#define I8042_CMD_DISABLE_PORT_2	(0xA7)
#define I8042_CMD_ENABLE_PORT_2		(0xA8)

#define I8042_CMD_TEST_PORT_1		(0xA9)
#define I8042_CMD_TEST_CONTROLLER	(0xAA)
#define I8042_CMD_TEST_PORT_2		(0xAB)

#define I8042_CMD_DIAG_DUMP		(0xAC)

#define I8042_CMD_DISABLE_PORT_1	(0xAD)
#define I8042_CMD_ENABLE_PORT_1		(0xAE)

#define I8042_CMD_READ_GPIO		(0xC0)
#define I8042_CMD_STATUS_GPIO_L		(0xC1)
#define I8042_CMD_STATUS_GPIO_H		(0xC2)

#define I8042_CMD_READ_OUTPORT		(0xD0)
#define I8042_CMD_WRITE_OUTPORT		(0xD1)

#define I8042_CMD_SPOOF_PORT_1		(0xD2)
#define I8042_CMD_SPOOF_PORT_2		(0xD3)

#define I8042_CMD_WRITE_PORT_2		(0xD4)

#define I8042_SYS_RESET			(0xFF)

#define I8042_RAM_CCR			(0x00)

#define I8042_CCR_KBD_IEN		(1<<0)
#define I8042_CCR_AUX_IEN		(1<<1)
#define I8042_CCR_UNK_1			(1<<2)
#define I8042_CCR_NLOCK_IEN		(1<<3)
#define I8042_CCR_KBD_DISABLE		(1<<4)
#define I8042_CCR_AUX_DISABLE		(1<<5)
#define I8042_CCR_TRANSLATE		(1<<6)
#define I8042_CCR_UNK_2			(1<<7)
#define I8042_CCR_UNK			(I8042_CCR_UNK_1 | I8042_CCR_UNK_2)

void i8042_write(int port, uint8_t data);
void ps2kbd_handle_data(uint8_t data);
void ps2aux_handle_data(uint8_t data);
void i8042_enable_aux();
void i8042_init();
#endif

/**
 * driver/input/ps2/i8042.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-07-2014 - Created
 */

#include "driver/input/ps2/i8042.h"
#include "arch/i386/x86.h"
#include "kernel/interrupt.h"
#include <stdint.h>
#include <stddef.h>

inline uint8_t i8042_read_status()
{
	return i386_inb(I8042_STATUS_PORT);
}

inline void i8042_wait_read()
{
	int to=0;
	while ((~i8042_read_status() & I8042_STATUS_FLAG_IN_FULL) && (to++ < 0x100000));
}

inline void i8042_wait_write()
{
	int to=0;
	while ((i8042_read_status() & I8042_STATUS_FLAG_OUT_FULL) && (to++ < 0x100000));
}

inline void i8042_write_command(uint8_t cmd)
{
	i8042_wait_write();
	i386_outb(I8042_CMD_PORT, cmd);
}

inline void i8042_write_data(uint8_t data)
{
	i8042_wait_write();
	i386_outb(I8042_DATA_PORT, data);
}

inline uint8_t i8042_read_data()
{
	i8042_wait_read();
	return i386_inb(I8042_DATA_PORT);
}

inline void i8042_write_ram(uint8_t addr, uint8_t data)
{
	i8042_write_command(I8042_CMD_WRITE_RAM(addr));
	i8042_write_data(data);
}

inline uint8_t i8042_read_ram(uint8_t addr)
{
	i8042_write_command(I8042_CMD_READ_RAM(addr));
	return i8042_read_data();
}

void i8042_write(int port, uint8_t data)
{
	if (port)
		i8042_write_command(I8042_CMD_WRITE_PORT_2);
	i8042_write_data(data);		
}

inline void i8042_toggle_port(int port, int enabled)
{
	if (enabled)
		i8042_write_command(port ? I8042_CMD_ENABLE_PORT_2 : I8042_CMD_ENABLE_PORT_1);
	else
		i8042_write_command(port ? I8042_CMD_DISABLE_PORT_2 : I8042_CMD_DISABLE_PORT_1);
}

inline void i8042_write_ccr(uint8_t ccr)
{
	ccr = (i8042_read_ram(I8042_RAM_CCR) & I8042_CCR_UNK) | (ccr & ~I8042_CCR_UNK);
	i8042_write_ram(I8042_RAM_CCR, ccr);
}

int i8042_self_test()
{
	int n, r;
	for (n = 0; n < 5; n++) {
		i8042_write_command(I8042_CMD_TEST_CONTROLLER);
		r = i8042_read_data();
		if (r == 0x55)
			return 0;
	}
	return -1;
}

void i8042_enable_aux()
{
	i8042_toggle_port(1, 1);
	i8042_write_ram(I8042_RAM_CCR, (i8042_read_ram(I8042_RAM_CCR) | I8042_CCR_AUX_IEN | I8042_CCR_KBD_IEN) & ~I8042_CCR_AUX_DISABLE );
}

int i8042_isr(__attribute__((__unused__)) irq_id_t irq_id, __attribute__((__unused__)) void *context)
{
	int status = i8042_read_status();
	int data = 0;
	//debugcon_aprintf("i8042isr status:%i\n",status);
	if (~status & I8042_STATUS_FLAG_IN_FULL)
		return 1;
	while (status & I8042_STATUS_FLAG_IN_FULL) {
		data = i8042_read_data();
		if (status & I8042_STATUS_FLAG_AUX_DATA) {
			ps2aux_handle_data(data);
			
		} else {
			ps2kbd_handle_data(data);
		}
		status = i8042_read_status();
	}
	return 1;
}

int i8042_isrm(__attribute__((__unused__)) irq_id_t irq_id, __attribute__((__unused__)) void *context)
{
	//int status = i8042_read_status();
	int data = 0;
	//debugcon_aprintf("i8042isr status:%i\n",status);
	//if (~status & I8042_STATUS_FLAG_IN_FULL)
	//	return 1;
	//while (status & I8042_STATUS_FLAG_IN_FULL) {
		data = i8042_read_data();
	//	if (status & I8042_STATUS_FLAG_AUX_DATA) {
			ps2aux_handle_data(data);
			
	//	} else {
			//ps2kbd_handle_data(data);
	//	}
	//	status = i8042_read_status();
	//}
	return 1;
}

int i8042_isrk(__attribute__((__unused__)) irq_id_t irq_id, __attribute__((__unused__)) void *context)
{
	//int status = i8042_read_status();
	int data = 0;
	//debugcon_aprintf("i8042isr status:%i\n",status);
	//if (~status & I8042_STATUS_FLAG_IN_FULL)
	//	return 1;
	//while (status & I8042_STATUS_FLAG_IN_FULL) {
		data = i8042_read_data();
	//	if (status & I8042_STATUS_FLAG_AUX_DATA) {
			//ps2aux_handle_data(data);
			
	//	} else {
			ps2kbd_handle_data(data);
	//	}
	//	status = i8042_read_status();
	//}
	return 1;
}


void i8042_init()
{
	//i8042_self_test();
	interrupt_register_handler(1, &i8042_isrk, NULL);
	interrupt_register_handler(12, &i8042_isrm, NULL);
}

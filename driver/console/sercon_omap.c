/**
 * arch/armv7/loader/sercon.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-03-2014 - Created
 */

#include "driver/uart/uart16c750.h"
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "kernel/console.h"

#define OMAP_UART3_BASE		(0x49020000)

volatile u16c750_general_reg_read  *uart3_read_reg  = (void *) OMAP_UART3_BASE;
volatile u16c750_general_reg_write *uart3_write_reg = (void *) OMAP_UART3_BASE;
volatile u16c750_baud_reg	   *uart3_baud_reg  = (void *) OMAP_UART3_BASE;

void sercon_putc(char out)
{
	while (!(uart3_read_reg->line_status & U16C750_LSR_THLD_EMPTY));
	uart3_write_reg->data_transmit = (uint32_t) out;
}

char sercon_getc()
{
	while (!( uart3_read_reg->line_status & U16C750_LSR_DATA_AVAIL ));
	return (char) uart3_read_reg->data_receive;
}

void sercon_puts(
	__attribute__((__unused__)) int sink,
	__attribute__((__unused__)) int flags,
	const char *string)
{
	while (*string) {
		sercon_putc(*string);
		string++;
	}
}

char debugcon_getc() {
	return sercon_getc();
}

void debugcon_putc(char c){
	sercon_putc(c);
}

void earlycon_init()
{
	/* Do nothing much yet, bootloader has configured UART for us */
	con_register_sink_s( "sercon", 0, sercon_puts );
}

void sercon_omap_init(){};

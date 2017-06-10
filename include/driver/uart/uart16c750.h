/**
 * driver/uart/uart16c750.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-03-2014 - Created
 */

#ifndef __DRIVER_UART_16C750__
#define __DRIVER_UART_16C750__

#include <stdint.h>

#define	U16C750_IER_RRH		(1<<0)
#define	U16C750_IER_TRH		(1<<1)
#define	U16C750_IER_LSTATUS	(1<<2)
#define	U16C750_IER_MSTATUS	(1<<3)
#define	U16C750_IER_SLEEP	(1<<4)
#define	U16C750_IER_LOWPWR	(1<<5)

#define U16C750_FCR_FIFO_EN	(1<<0)
#define U16C750_FCR_RCV_FIFORST	(1<<1)
#define U16C750_FCR_XMT_FIFORST	(1<<2)
#define U16C750_FCR_FIFO_64EN	(1<<5)
#define U16C750_FCR_RCV_TRIG(V)	((V&3)<<6)

#define U16C750_ISR_INTSTATUS	(1<<0)
#define U16C750_ISR_INTPRIO(R)	((R>>1) & 7)
#define U16C750_ISR_FIFO_64EN	(1<<5)
#define U16C750_ISR_FIFO_EN0	(1<<6)
#define U16C750_ISR_FIFO_EN1	(1<<7)

#define U16C750_LCR_WORDLEN(V)	(V&3)
#define U16C750_LCR_STOPBITS	(1<<2)
#define U16C750_LCR_PARITYEN	(1<<3)
#define U16C750_LCR_PARITYEVEN	(1<<4)
#define U16C750_LCR_PARITYSET	(1<<5)
#define U16C750_LCR_SETBREAK	(1<<6)
#define U16C750_LCR_DIVISOREN	(1<<7)

#define U16C750_LSR_DATA_AVAIL	(1<<0)
#define U16C750_LSR_OVERRUN_ERR	(1<<1)
#define U16C750_LSR_PARITY_ERR	(1<<2)
#define U16C750_LSR_FRAMING_ERR	(1<<3)
#define U16C750_LSR_BREAK_INT	(1<<4)
#define U16C750_LSR_THLD_EMPTY	(1<<5)
#define U16C750_LSR_TRANS_EMPTY	(1<<6)
#define U16C750_LSR_FIFO_DERR	(1<<7)

typedef struct {
	uint32_t	data_receive;
	uint32_t	interrupt_en;
	uint32_t	interrupt_st;
	uint32_t	line_control;
	uint32_t	modem_control;
	uint32_t	line_status;
	uint32_t	modem_status;
	uint32_t	scratchpad;
} u16c750_general_reg_read;

typedef struct {
	uint32_t	data_transmit;
	uint32_t	interrupt_en;
	uint32_t	fifo_control;
	uint32_t	line_control;
	uint32_t	modem_control;
	uint32_t	line_status_nx;
	uint32_t	modem_status_nx;
	uint32_t	scratchpad;
} u16c750_general_reg_write;

typedef struct {
	uint32_t	divisor_lsb;
	uint32_t	divisor_msb;
} u16c750_baud_reg;

#endif

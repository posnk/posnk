//
// Created by pbx on 28/12/20.
//

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "kernel/tty.h"

#include "driver/tty/uart/ns16x50.h"

#define UART_16750
#include "driver/tty/uart/ns16x50_regs.h"


uint8_t ns16x50_handle_msr( ns16x50_info_t *uart, int in_tx );

static int fifo_len[] = {
        1,
        16,
        64
};

static int baud_rates[] = {
        0,
        50,
        75,
        110,
        134,
        150,
        200,
        300,
        600,
        1200,
        1800,
        2400,
        4800,
        9600,
        19200,
        38400,
        57600,
        115200
};

static uint8_t reg_read( ns16x50_info_t *uart, int reg )
{
    uint8_t lcr, val;
    int reg_idx = reg & 7u;
    lcr = uart->current_lcr;
    if ( reg & 0x10u )
        uart->reg_write( uart, NS16X50_REG_LCR, lcr | NS16X50_LCR_DLAB );
    val = uart->reg_read( uart, reg_idx );
    if ( reg & 0x10u )
        uart->reg_write( uart, NS16X50_REG_LCR, lcr );
    return val;
}

static void reg_write( ns16x50_info_t *uart, int reg, uint8_t val )
{
    uint8_t lcr;
    int reg_idx = reg & 7u;
    lcr = uart->current_lcr;
    if ( reg & 0x10u )
        uart->reg_write( uart, NS16X50_REG_LCR, lcr | NS16X50_LCR_DLAB );
    uart->reg_write( uart, reg_idx, val );
    if ( reg & 0x10u )
        uart->reg_write( uart, NS16X50_REG_LCR, lcr );
}

uint8_t conv_lcr( int cflag )
{
    int lcr = 0;
    /* Character size */
    switch( cflag & CSIZE ) {
        case CS5:
            lcr |= NS16X50_LCR_WL5;
            break;
        case CS6:
            lcr |= NS16X50_LCR_WL6;
            break;
        case CS7:
            lcr |= NS16X50_LCR_WL7;
            break;
        case CS8:
        default:
            lcr |= NS16X50_LCR_WL8;
            break;
    }

    /* Stop bits */
    if ( cflag & CSTOPB )
        lcr |= NS16X50_LCR_STB;

    /* Parity enable */
    if ( cflag & PARENB )
        lcr |= NS16X50_LCR_PEN;

    /* Parity even/odd */
    if ( ~cflag & PARODD )
        lcr |= NS16X50_LCR_EPS;

    return lcr;
}

void ns16x50_load_divisor( ns16x50_info_t *uart, int baud_rate )
{
    uint32_t div;

    if ( baud_rate > B115200 )
        return;

    baud_rate = baud_rates[ baud_rate ];

    if ( baud_rate == 0 )
        return;

    div = uart->input_clock / ( 16 * baud_rate );

    assert( div <= 0x10000 );

    reg_write( uart, NS16X50_REG_DLL, div & 0xFFu );
    reg_write( uart, NS16X50_REG_DLH, (div >> 8) & 0xFFu );
}

static void set_lcr( ns16x50_info_t *uart, int lcr )
{
    if ( lcr != uart->current_lcr ) {
        uart->current_lcr = lcr;
        reg_write( uart, NS16X50_REG_LCR, lcr );
    }
}

static void set_mcr( ns16x50_info_t *uart, int mcr )
{
    if ( mcr != uart->current_mcr ) {
        uart->current_mcr = mcr;
        reg_write( uart, NS16X50_REG_LCR, mcr );
    }
}

static void set_ier( ns16x50_info_t *uart, int ier )
{
    if ( ier != uart->current_ier ) {
        uart->current_ier = ier;
        reg_write( uart, NS16X50_REG_LCR, ier );
    }
}

void ns16x50_load_termios( ns16x50_info_t *uart )
{
    termios_t *termios = &uart->tty->termios;
    set_lcr( uart, conv_lcr( termios->c_cflag ) );
    ns16x50_load_divisor( uart, termios->c_ispeed );
    uart->current_mcr = 0;
    uart->flags &= ~NS16X50_FLAG_MANUAL_FLOW;
    if ( termios->c_cflag & CRTSCTS ){
        if ( uart->flags & NS16X50_FLAG_AUTO_FLOW ) {
            set_mcr( uart, uart->current_mcr | NS16X50_MCR_AFE );
        } else {
            uart->flags |= NS16X50_FLAG_MANUAL_FLOW;
        }
    }
}


void ns16x50_empty_rxb( ns16x50_info_t *uart )
{
    uint8_t lsr, rd;
    int max;

    max = fifo_len[ uart->mode ];

    while ( max-- ) {
        lsr = reg_read( uart, NS16X50_REG_LSR );

        if ( tty_input_queue_full( uart->tty->device ) ) {
            break;
        }

        if ( lsr & NS16X50_LSR_DR ) {
            /* Move data from the RHR/FIFO to the TTY input buffer */
            rd = reg_read( uart, NS16X50_REG_RHR);
        } else if ( lsr & NS16X50_LSR_ERROR ) {
            /* No data, but an error was found */
            rd = 0;
        } else {
            /* Nothing happened, break */
            break;
        }
        if ( lsr & NS16X50_LSR_ERROR ) {
            if ( lsr & NS16X50_LSR_BI ) {
                tty_input_break( uart->tty->device, rd );
            } else if ( lsr & NS16X50_LSR_PE) {
                tty_input_error( uart->tty->device, rd );
            } else if ( lsr & NS16X50_LSR_FE) {
                tty_input_error( uart->tty->device, rd );
            } else if ( lsr & NS16X50_LSR_OE ) {
                printf( CON_WARN, "tty overflow!");
            }
        } else {
            tty_input_char( uart->tty->device, rd );
        }
    }

    if ( uart->tty->termios.c_cflag & CRTSCTS ) {
        if ( tty_input_queue_full( uart->tty->device ) )
            set_mcr( uart, uart->current_mcr & ~NS16X50_MCR_nRTR );
        else
            set_mcr( uart, uart->current_mcr |  NS16X50_MCR_nRTR );
    }
}

int ns16x50_handle_int( ns16x50_info_t *uart )
{
    int isr;

    isr = reg_read( uart, NS16X50_REG_ISR );

    /* Check if this interrupt was aimed at us */
    if ( isr & NS16X50_ISR_NO_INT )
        return 0;

    do {
        /* Handle the various interrupts */
        switch ( NS16X50_ISR_ID( isr ) ) {
            case NS16X50_ISR_ID_RXRDY:
            case NS16X50_ISR_ID_RXTO:
            case NS16X50_ISR_ID_LSR:
                ns16x50_empty_rxb( uart );
                break;
            case NS16X50_ISR_ID_TXRDY:
                ns16x50_fill_txb( uart );
                break;
            case NS16X50_ISR_ID_MSR:
                ns16x50_handle_msr( uart, 0 );
                break;
            default:
                printf(CON_WARN, "unknown interrupt %i", NS16X50_ISR_ID( isr ));
                break;
        }

        /* Check if there are any more unhandled interrupts */
        isr = reg_read( uart, NS16X50_REG_ISR );

    } while ( ~isr & NS16X50_ISR_NO_INT );

    return 1;
}

uint8_t ns16x50_handle_msr( ns16x50_info_t *uart, int in_tx )
{
    uint8_t msr;
    msr = reg_read( uart, NS16X50_REG_MSR );

    if ( uart->flags & NS16X50_FLAG_MANUAL_FLOW && !in_tx ) {
        if ( ~msr & NS16X50_MSR_CTS ) {
            ns16x50_stop_tx( uart );
        } else if ( !pipe_is_empty( uart->pipe_out ) ){
            ns16x50_start_tx( uart );
        }
    }


}

void ns16x50_fill_txb( ns16x50_info_t *uart )
{
    int txcount = 0, i;
    int s;
    aoff_t rsz;
    char buf;
    uint8_t lsr, msr;

    lsr = reg_read( uart, NS16X50_REG_LSR );

    if ( ~lsr & NS16X50_LSR_THRE )
        return;

    for ( i = 0; i < fifo_len[ uart->mode ]; i++ ) {

        if ( uart->flags & NS16X50_FLAG_MANUAL_FLOW ) {
            msr = ns16x50_handle_msr( uart, 1 );
            if ( ~msr & NS16X50_MSR_CTS ) {
                ns16x50_stop_tx( uart );
                return;
            }
        }

        s = pipe_read(
                /* pipe      */ uart->pipe_out,
                /* buffer    */ &buf,
                /* count     */ sizeof buf,
                /* read_size */ &rsz,
                /* non_block */ 1 );

        if ( s != 0 || rsz <= 0 ) {
            ns16x50_stop_tx( uart );
            return;
        }

        txcount++;
        reg_write( uart, NS16X50_REG_THR, buf );

    }

}

void ns16x50_start_tx( ns16x50_info_t *uart )
{
    /* If the FIFO/TX holding register is empty, fill it */
    ns16x50_fill_txb( uart );

    /* Enable the transmit buffer empty interrupt */
    set_ier( uart, uart->current_ier | NS16X50_IER_ETBEI );
}

void ns16x50_stop_tx( ns16x50_info_t *uart )
{
    //TODO: Disable TBEI?
}

void ns16x50_write( ns16x50_info_t *uart, char chr )
{
    aoff_t wc;
    pipe_write( uart->pipe_out, &chr, 1, &wc, 0 );
    ns16x50_start_tx( uart );
}

void ns16x50_connect( ns16x50_info_t *uart )
{
    set_mcr( uart, uart->current_mcr |  NS16X50_MCR_nDTR );
}

void ns16x50_hangup( ns16x50_info_t *uart )
{
    set_mcr( uart, uart->current_mcr & ~NS16X50_MCR_nDTR );
}

void ns16x50_set_break( ns16x50_info_t *uart, int brk )
{
    int lcr = uart->current_lcr & ~NS16X50_LCR_BREAK;
    if ( brk )
        lcr |= NS16X50_LCR_BREAK;
    set_lcr( uart, lcr );
}

void ns16x50_init(
        ns16x50_info_t *uart,
        void *impl,
        tty_info_t *tty,
        int mode,
        uint32_t input_clock,
        ns16x50_rrd_t reg_read,
        ns16x50_rwr_t reg_write ) {
    memset( uart, 0, sizeof(ns16x50_info_t) );
    uart->impl = impl;
    uart->tty = tty;
    uart->mode = mode;
    uart->reg_read = reg_read;
    uart->reg_write = reg_write;
    uart->pipe_out = pipe_create();

    switch( uart->mode ) {
        case NS16X50_MODE_FIFO64:
            uart->current_fcr |= NS16X50_FCR_64BYTE;
        case NS16X50_MODE_FIFO16:
            uart->current_fcr |= NS16X50_FCR_FIFO_EN;
            break;
        case NS16X50_MODE_NOFIFO:
            break;
    }

    reg_write( )

    assert( uart->pipe_out != NULL );
}
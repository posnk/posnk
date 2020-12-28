//
// Created by pbx on 31/01/21.
//

#include "driver/tty/uart/ns16x50.h"
#include <string.h>
#include <sys/errno.h>
#include <stdint.h>
#include <sys/types.h>
#include "kernel/pipe.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/physmm.h"
#include "kernel/device.h"
#include "kernel/syscall.h"
#include <string.h>
#include <stdint.h>
#include "kernel/tty.h"
#include "arch/i386/x86.h"

#define MAX_UARTS (16)

typedef struct {
    int            refcount;
    ns16x50_info_t nsinfo;
    int            portbase;
} pcuart_info_t;

static pcuart_info_t uarts[MAX_UARTS];
static int           uart_count = 0;

pcuart_info_t *pcuart_get( dev_t dev )
{
    dev_t minor = MINOR(dev);
    if ( minor & 128 )
        minor &= ~128;
    if ( minor < 0 || minor > uart_count )
        return NULL;
    return uarts + minor;
}

void pcuart_open( tty_info_t *info )
{
    pcuart_info_t *uart = pcuart_get(info->device);
    if ( !uart->refcount )
        ns16x50_connect( &uart->nsinfo );
    uart->refcount++;
}

void pcuart_close( tty_info_t *info )
{
    pcuart_info_t *uart = pcuart_get(info->device);
    uart->refcount--;
    if ( !uart->refcount )
        ns16x50_hangup( &uart->nsinfo );
}

int pcuart_putc(dev_t device, char a)
{
    pcuart_info_t *uart = pcuart_get( device );
    ns16x50_write( &uart->nsinfo, a );
    return 0;
}

void pcuart_termios_changed(tty_info_t *info)
{
    pcuart_info_t *uart = pcuart_get(info->device);
    ns16x50_load_termios( &uart->nsinfo );
}

uint8_t pcuart_reg_read( ns16x50_info_t *_uart, int reg )
{
    pcuart_info_t *uart = _uart->impl;
    return i386_inb( uart->portbase + reg );
}

void    pcuart_reg_write( ns16x50_info_t *_uart, int reg, uint8_t data )
{
    pcuart_info_t *uart = _uart->impl;
    i386_outb( uart->portbase + reg, data );
}

static tty_ops_t ops = {
        .write_out = pcuart_putc,
        .open = pcuart_open,
        .close = pcuart_close,
        .termios_changed = pcuart_termios_changed
};

void pcuart_reg( int portbase, int mode)
{
    int minor = uart_count;
    pcuart_info_t *uart = uarts + minor;

    if ( uart_count == MAX_UARTS )
        return;
    uart_count++;

    uart->refcount = 0;
    uart->portbase = portbase;
    ns16x50_init(
            & uart->nsinfo,
            uart,
            tty_get(MAKEDEV(0x79,minor)),
            NS16X50_MODE_FIFO16,
            1843200,
            pcuart_reg_read,
            pcuart_reg_write );

}

void pcuart_init()
{
    tty_register_driver("legacy pc uart", 0x79, MAX_UARTS, &ops);
    pcuart_reg( 0x3f8, NS16X50_MODE_FIFO16 );
}

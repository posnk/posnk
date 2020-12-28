//
// Created by pbx on 31/01/21.
//

#ifndef POSNK_NS16X50_H
#define POSNK_NS16X50_H

#include <stdint.h>
#include "kernel/tty.h"

#define NS16X50_MODE_NOFIFO (0)
#define NS16X50_MODE_FIFO16 (1)
#define NS16X50_MODE_FIFO64 (2)
#define NS16X50_FLAG_AUTO_FLOW (1)
#define NS16X50_FLAG_MANUAL_FLOW (2)

typedef struct ns16x50_info ns16x50_info_t;
typedef uint8_t     (*ns16x50_rrd_t )( ns16x50_info_t *uart, int reg);
typedef void        (*ns16x50_rwr_t)( ns16x50_info_t *uart, int reg, uint8_t val);

struct ns16x50_info {
    void *        impl;
    tty_info_t   *tty;
    pipe_info_t  *pipe_out;
    int           mode;
    int           flags;
    uint32_t      input_clock;
    ns16x50_rrd_t reg_read;
    ns16x50_rwr_t reg_write;
    uint8_t       current_lcr;
    uint8_t       current_mcr;
    uint8_t       current_fcr;
    uint8_t       current_ier;
};

void ns16x50_init(
        ns16x50_info_t *uart,
        void *impl,
        tty_info_t *tty,
        int mode,
        uint32_t input_clock,
        ns16x50_rrd_t reg_read,
        ns16x50_rwr_t reg_write );

void ns16x50_load_divisor( ns16x50_info_t *uart, int baud_rate );

void ns16x50_load_termios( ns16x50_info_t *uart );

void ns16x50_empty_rxb( ns16x50_info_t *uart );

int ns16x50_handle_int( ns16x50_info_t *uart );

void ns16x50_fill_txb( ns16x50_info_t *uart );

void ns16x50_start_tx( ns16x50_info_t *uart );

void ns16x50_stop_tx( ns16x50_info_t *uart );

void ns16x50_write( ns16x50_info_t *uart, char chr );

void ns16x50_connect( ns16x50_info_t *uart );

void ns16x50_hangup( ns16x50_info_t *uart );

void ns16x50_set_break( ns16x50_info_t *uart, int brk );

#endif //POSNK_NS16X50_H

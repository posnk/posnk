/**
 * arch/i386/gdbstub.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-09-2017 - Created
 */

#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "arch/i386/task_context.h"
#include "kernel/earlycon.h"
#include <string.h>
#include "kernel/paging.h"
#include "kdbg/gdb/gdb.h"





void gdb_sendregs ( void )
{
    static char buf[113];

    if ( gdb_debug_user == 0 ) {
        gdb_enc32 ( buf +   0, state->intr_regs.eax );
        gdb_enc32 ( buf +   8, state->intr_regs.ecx );
        gdb_enc32 ( buf +  16, state->intr_regs.edx );
        gdb_enc32 ( buf +  24, state->intr_regs.ebx );
        gdb_enc32 ( buf +  32, state->intr_regs.esp );
        gdb_enc32 ( buf +  40, state->intr_regs.ebp );
        gdb_enc32 ( buf +  48, state->intr_regs.esi );
        gdb_enc32 ( buf +  56, state->intr_regs.edi );
        gdb_enc32 ( buf +  64, state->intr_eip      );
        gdb_enc32 ( buf +  72, 0                    );
        gdb_enc32 ( buf +  80, state->intr_cs       );
        gdb_enc32 ( buf +  88, state->intr_ds       );
        gdb_enc32 ( buf +  96, state->intr_ds       );
        gdb_enc32 ( buf + 104, state->intr_ds       );
    } else {
        gdb_enc32 ( buf +   0, state->user_regs.eax );
        gdb_enc32 ( buf +   8, state->user_regs.ecx );
        gdb_enc32 ( buf +  16, state->user_regs.edx );
        gdb_enc32 ( buf +  24, state->user_regs.ebx );
        gdb_enc32 ( buf +  32, state->user_regs.esp );
        gdb_enc32 ( buf +  40, state->user_regs.ebp );
        gdb_enc32 ( buf +  48, state->user_regs.esi );
        gdb_enc32 ( buf +  56, state->user_regs.edi );
        gdb_enc32 ( buf +  64, state->user_eip      );
        gdb_enc32 ( buf +  72, state->user_eflags   );
        gdb_enc32 ( buf +  80, state->user_cs       );
        gdb_enc32 ( buf +  88, state->user_ss       );
        gdb_enc32 ( buf +  96, state->user_ds       );
        gdb_enc32 ( buf + 104, state->user_ds       );
    }

    buf[112] = 0;

    gdbout_pkt ( buf );
}



void gdb_notif()
{

}
void gdbstub ( void )
{
    state = scheduler_current_task->arch_state;

    for ( ;; ) {
        gdbhandle ( gdbport_in() );
    }
}


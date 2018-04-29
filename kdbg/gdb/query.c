/**
 * kdbg/gdb/query.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#include "kdbg/gdb/gdb.h"

void gdb_query ( char* query )
{
    static char buf[40];

    if ( strcmp ( query, "C" ) == 0 ) {
        earlycon_sprintf ( buf, "QC p%i.%i", 
        	gdb_task->process->pid,
        	gdb_task->tid );
        gdbout_pkt ( buf );
    } else if ( strcmp ( query, "Attached" ) == 0 ) {
        gdbout_pkt ( "1" );
    } else {
        gdblog ( "Unimplemented query %s\n", query );
        gdbout_pkt ( "" );
    }
}

void gdb_qset ( char* query )
{
    {
        gdblog ( "Unimplemented Query %s\n", query );
        gdbout_pkt ( "" );
    }
}


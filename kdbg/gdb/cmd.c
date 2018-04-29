/**
 * kdbg/gdb/cmd.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#include "kdbg/gdb/gdb.h"

void gdbpacket ( char* cmd, int len )
{
    cmd[len] = 0;

    if ( len == 0 )
        return;

    switch ( cmd[0] ) {

    case 'g':
        gdbsendregs();
        break;

    case 'm':
        gdb_readmem ( cmd + 1 );
        break;

    case 'q':
        gdb_query ( cmd + 1 );
        break;

    case 'Q':
        gdb_qset ( cmd + 1 );
        break;

    case '?':
        gdbout_pkt ( "S00" );
        break;

    case '!':

    //TODO: Extended mode
    //TODO: Last Signal
    case 'A':

    //TODO: Run Programs
    case 'B':

    //TODO: Breakpoints
    default:
        gdblog ( "Unimplemented command '%c' %s\n", cmd[0], cmd );
        gdbout_pkt ( "" );
    }
}


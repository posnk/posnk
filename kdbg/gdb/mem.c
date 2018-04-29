/**
 * kdbg/gdb/mem.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#include "kdbg/gdb/gdb.h"

void gdb_readmem ( char* cmd )
{
    uint32_t ptr, sz, p, f;
    ptr = gdb_dec32 ( &cmd );
    cmd++;
    sz = gdb_dec32 ( &cmd );
    gdblog ( "Read memory?!? %x %x\n", ptr, sz );

    for ( p = 0; p < sz; p += 4096 ) {
        int rsz = 4096;
        int psz = sz - p;

        if ( psz < rsz )
            rsz = psz;

        f = paging_get_physical_address ( ( void* ) ( ptr + p ) );
        gdblog ( "Read memory?!? map %x %x\n", ptr + p, f );

        if ( !f )
            sz = p;
    }

    if ( sz > 0 )
        gdbout_pkthex ( ( char* ) ptr, sz );
    else
        gdbout_pkt ( "E01" );
}

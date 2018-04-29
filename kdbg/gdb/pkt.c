/**
 * kdbg/gdb/pkt.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#include "kdbg/gdb/gdb.h"

void gdbout_pkthex ( const char* buffer, int cnt )
{
    char c, v;
    int cksum = 0;
    int p = 0;

    do {
        gdbport_out ( '$' );

        while ( p < cnt ) {
            //TODO: Escapes
            v = buffer[p++];
            c =  hstring[ ( v >> 4 ) & 0xf];
            gdbport_out ( c );
            cksum = ( ( c & 0xFF ) + cksum ) & 0xFF;
            c =  hstring[v & 0xf];
            gdbport_out ( c );
            cksum = ( ( c & 0xFF ) + cksum ) & 0xFF;
        }

        gdbport_out ( '#' );
        gdb_outhex ( cksum );
    } while ( ( gdbport_in() & 0x7f ) != '+' );
}

void gdbout_pkt ( char* buffer )
{
    int cksum = 0;

    do {
        gdbport_out ( '$' );

        while ( *buffer ) {
            //TODO: Escapes
            gdbport_out ( *buffer );
            cksum = ( ( *buffer++ & 0xFF ) + cksum ) & 0xFF;
        }

        gdbport_out ( '#' );
        gdb_outhex ( cksum );
    } while ( ( gdbport_in() & 0x7f ) != '+' );
}

void gdbin_char ( char in )
{
    static char buf[ GDBBUF_SIZE ];
    static int ptr = 0;
    static int state = GDBSTATE_WAITPKT;
    static int cksum = 0;
    static int _cksum = 0;
    int t;
    char c;

    if ( in == '$' ) {
        state = GDBSTATE_INPKT;
        ptr = 0;
        _cksum = 0;
        return;
    } else if ( in == '#' ) {
        state = GDBSTATE_CKSUM0;
        cksum = 0;
        return;
    } else if ( state == GDBSTATE_INPKT ||
                state == GDBSTATE_ESC ) {
        _cksum = ( ( ( ( unsigned int ) in ) & 0xFF ) + _cksum ) & 0xFF;
    }

    switch ( state ) {

    case GDBSTATE_WAITPKT:
        if ( in == '+' )
            gdb_notif();

        break;

    case GDBSTATE_INPKT:
        if ( in == '}' ) {
            state = GDBSTATE_ESC;
        } else if ( ptr != GDBBUF_SIZE )
            buf[ ptr++ ] = in;
        else {
            gdblog ( "Buffer Overflow!\n" );
            goto rerr;
        }

        break;

    case GDBSTATE_ESC:
        in ^= 0x20;

        if ( ptr != GDBBUF_SIZE ) {
            buf[ ptr++ ] = in;
            state = GDBSTATE_INPKT;
        } else {
            gdblog ( "Buffer Overflow!\n" );
            state = GDBSTATE_WAITPKT;
        }

        break;

    case GDBSTATE_CKSUM0:
        t = gdbdec ( in );

        if ( t == -1 ) {
            gdblog ( "Invalid Checksum Char '%c' 0x%x\n", in, in );
            goto rerr;
        }

        cksum = t << 4;
        state = GDBSTATE_CKSUM1;
        break;

    case GDBSTATE_CKSUM1:
        t = gdbdec ( in );

        if ( t == -1 ) {
            gdblog ( "Invalid Checksum Char '%c' 0x%x\n", in, in );
            goto rerr;
        }

        cksum |= t;

        if ( cksum != _cksum ) {
            gdblog ( "Invalid Checksum 0x%x != 0x%x\n", cksum, _cksum );
            goto rerr;
        }

        gdbport_out ( '+' );
        gdbpacket ( buf, ptr );
        state = GDBSTATE_WAITPKT;
        break;

    }

    return;

rerr:
    state = GDBSTATE_WAITPKT;
    gdbport_out ( '-' );

}


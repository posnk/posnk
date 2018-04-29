/**
 * kdbg/gdb/gdb.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-05-2018 - Created
 */
 
#ifndef __gdbstub_h__
#define __gdbstub_h__

#include <stdint.h>
#include "kernel/scheduler.h"

/* Definitions */

#define GDBSTATE_WAITPKT   (0)
#define GDBSTATE_INPKT     (1)
#define GDBSTATE_CKSUM0    (2)
#define GDBSTATE_CKSUM1    (3)
#define GDBSTATE_ESC       (4)

#define GDBBUF_SIZE (4096)

/* Globals */
extern scheduler_task_t *gdb_task;
extern int				 gdb_debug_user;

/* IO Port functions */

char gdbport_in();
int  gdbport_avail();
void gdbport_out ( char c );

/* IO Functions */

int gdblog(const char* str,...);
void gdb_outhex ( uint8_t v );
int gdb_dechex ( char c );
int gdb_ishex ( char c );
uint32_t gdb_dec32 ( char** str );
void gdb_enc32 ( char* buf, uint32_t v );

/* Packet functions */

void gdbout_pkthex ( const char* buffer, int cnt );
void gdbout_pkt ( char* buffer );
void gdbin_char ( char in );

/* Handler functions */
void gdbpacket ( char* cmd, int len );
void gdb_qset  ( char* query );
void gdb_query ( char* query );

/* Platform functions */
void gdb_sendregs ( void );
 
void gdbhandle( char in );
 
#endif

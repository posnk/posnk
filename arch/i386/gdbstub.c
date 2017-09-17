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
 
 char gdbport_in();
 int  gdbport_avail();
 void gdbport_out( char c );
 
 char hstring[17]="0123456789ABCDEF";
 
 void gdbouthbt( uint8_t b )
 {
 	gdbport_out(hstring[(b>>4)&0xF]);
 	gdbport_out(hstring[b&0xF]);
 }
  
 void gdbout_pkt( char *buffer )
 {
 	int cksum = 0;
 	do {
 		gdbport_out('$');
 		while ( *buffer ) {
 			//TODO: Escapes
 			gdbport_out( *buffer );
 			cksum = (( *buffer++ & 0xFF ) + cksum ) & 0xFF;
 		}
 		gdbport_out('#');
 		gdbout_hex( cksum );
 	} while ( (gdbport_in()&0x7f) != '+' )
 }
 
 
 #define GDBBUF_SIZE (4096)
 
 int gdbdec( char c )
 {
 	if ( c >= '0' && c <= '9' )
 		return c - '0';
 	else if ( c >= 'A' && c <= 'F' )
 		return c - 'A';
 	else if ( c >= 'a' && c <= 'f' )
 		return c - 'a';
 	else {
 		gdblog("Invalid text char")
 		return -1;
 	}
 }
 
 void gdbenc32( char *buf, uint32_t v )
 {
 	buf[0] = hstring[ (v >> 28) & 0xF ];
 	buf[1] = hstring[ (v >> 24) & 0xF ];
 	buf[2] = hstring[ (v >> 20) & 0xF ];
 	buf[3] = hstring[ (v >> 16) & 0xF ];
 	buf[4] = hstring[ (v >> 12) & 0xF ];
 	buf[5] = hstring[ (v >>  8) & 0xF ];
 	buf[6] = hstring[ (v >>  4) & 0xF ];
 	buf[7] = hstring[  v        & 0xF ];
 }
 
 
 
 i386_task_context_t *state;
 
 int userdbg;
 
 void gdbsendregs( void )
 {
 	static char buf[113];
 	gdbenc32( buf +   0, state->user_regs.eax );
 	gdbenc32( buf +   8, state->user_regs.ecx );
 	gdbenc32( buf +  16, state->user_regs.edx );
 	gdbenc32( buf +  24, state->user_regs.ebx );
 	gdbenc32( buf +  32, state->user_regs.esp );
 	gdbenc32( buf +  40, state->user_regs.ebp );
 	gdbenc32( buf +  48, state->user_regs.esi );
 	gdbenc32( buf +  56, state->user_regs.edi );
 	gdbenc32( buf +  64, state->user_eip      );
 	gdbenc32( buf +  72, state->user_eflags   );
 	gdbenc32( buf +  80, state->user_cs       );
 	gdbenc32( buf +  88, state->user_ss       );
 	gdbenc32( buf +  96, state->user_ds       );
 	gdbenc32( buf + 104, state->user_es       );
 	buf[112] = 0;
 	gsbout_pkt( buf );
 }
 
 void gdbpacket( char *cmd, int len )
 {
 	if ( len == 0 )
 		return;
 	switch( cmd[0] ) {
 	
 		case 'g':
 			gdbsendregs();
 			break;
 	
 		case '!':
 			//TODO: Extended mode
 		case '?':
 			//TODO: Last Signal
 		case 'A':
 			//TODO: Run Programs 		
 		case 'B':
 			//TODO: Breakpoints
 		default:
 			gdblog("Unimplemented command '%c' %x\n", cmd[0] );
 	}
 }
 
 #define GDBSTATE_WAITPKT   (0)
 #define GDBSTATE_INPKT     (1)
 #define GDBSTATE_CKSUM0    (2)
 #define GDBSTATE_CKSUM1    (3)
 #define GDBSTATE_ESC       (4)
 
 void gdbhandle( char in )
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
 		_cksum = ( ( in & 0xFF ) + cksum ) & 0xFF;
 	}
 	
 	switch ( state ) {
 	
 		case GDBSTATE_WAITPKT:
	 		//TODO: Acknowledges & Notifications
 			break;
 			
 		case GDBSTATE_INPKT:
 			if ( in == '}' ) {
 				state = GDBSTATE_ESC;
 			} else if ( ptr != GDBBUF_SIZE )
 				buf[ ptr++ ] = in;
 			else {
 				gdblog("Buffer Overflow!\n");
 				goto rerr;
 			}
 			break;
 		
 		case GDBSTATE_ESC:
 			in ^= 0x20;
 			if ( ptr != GDBBUF_SIZE ) {
 				buf[ ptr++ ] = in;
 				state = GDBSTATE_INPKT;
 			} else {
 				gdblog("Buffer Overflow!\n");
 				state = GDBSTATE_WAITPKT;
 			}
 			break;
 			
 		case GDBSTATE_CKSUM0:
 			t = gdbdec( in );
 			if ( t == -1 ) {
 				gdblog("Invalid Checksum Char '%c' 0x%x\n", in, in);
 				goto rerr;
 			}
 			cksum = t << 4;
 			state = GDBSTATE_CKSUM1;
 			break;
 			
 		case GDBSTATE_CKSUM1:
 			t = gdbdec( in );
 			if ( t == -1 ) {
 				gdblog("Invalid Checksum Char '%c' 0x%x\n", in, in);
 				goto rerr;
 			}
 			cksum |= t;
 			if ( cksum != _cksum ) {
 				gdblog("Invalid Checksum 0x%x != 0x%x\n", cksum, _cksum);
 				goto rerr;
 			}
 			gdbport_out('+');
 			gdbpacket( buf, datalen );
 			state = GDBSTATE_WAITPKT;
 			break;
 			
 	}
 	
rerr:
	state = GDBSTATE_WAITPKT;
	gdbport_out( '-' );
 	
 }

#include <stdint.h>
#include "kernel/heapmm.h"
#define CON_PRIVATE
#include "kernel/console.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

con_sink_t con_sink_map[ CON_SINK_MAX + 1 ];
static int def_level = CON_INFO;
static int early_sink = 1;

/* Do not call without management lock on the console source map */
static int find_free_sink() {
	int i;
	for ( i = 0; i <= CON_SINK_MAX; i++ ) {
		if ( !con_sink_map[i].name )
			return i;
	}
	return -1;
}

int con_find_sink( const char *name ) {
	int i;

	for ( i = 0; i <= CON_SINK_MAX; i++ ) {

		if ( !con_sink_map[i].name )
			continue;

		if ( strcmp( name, con_sink_map[i].name ) == 0 )
			return i;
	}

	return -1;
}

int  con_get_sink( const char *name ) {
	int sink;

	sink = con_find_sink( name );
	if ( sink >= 0 )
		return sink;

	sink = find_free_sink();
	if ( sink == -1 )
		return -1;

	con_sink_map[sink].log_level = -1;
	con_sink_map[sink].name = name;
	printf( CON_DEBUG, "create sink %s (%i)", con_sink_map[ sink ].name, sink);

	return sink;
}

void con_leave_early() {
	int i;

	for ( i = 0; i <= CON_SINK_MAX; i++ ) {
		if ( con_sink_map[i].flags & CON_SINK_EARLY ) {
			memset( con_sink_map + i, 0, sizeof(con_sink_t) );
			con_def_sinkmap &= ~(1<<i);
		}
	}
	//TODO: Replay history
	early_sink = 0;
}

int  con_register_sink_r( const char *name, int flags, con_puts_r_t puts_r ) {
	int sink;

	sink = con_get_sink( name );

	if ( sink == -1 )
		return -1;

	con_sink_map[sink].puts_r = puts_r;
	con_sink_map[sink].flags = flags;

	if ( early_sink && (~flags & CON_SINK_EARLY) )
		con_leave_early();

	if ( flags & CON_SINK_DEBUG )
		con_sink_map[sink].log_level = CON_DEBUG;

	con_def_sinkmap |= (1<<sink);

	return sink;
}

int  con_register_sink_s( const char *name, int flags, con_puts_s_t puts_s ) {
	int sink;

	sink = con_get_sink( name );

	if ( sink == -1 )
		return -1;

	con_sink_map[sink].puts_s = puts_s;
	con_sink_map[sink].flags = flags;

	if ( early_sink && (~flags & CON_SINK_EARLY) )
		con_leave_early();


	if ( flags & CON_SINK_DEBUG )
		con_sink_map[sink].log_level = CON_DEBUG;

	con_def_sinkmap |= (1<<sink);

	return sink;
}

static char line_buffer[256]; //TODO: Reentrancy!

void con_sink_puts( int sink, int flags, int hnd, int lvl, const char *msg ) {
	const char *dn;
	con_puts_r_t puts_r;
	con_puts_s_t puts_s;
	int log_level;

	log_level = con_sink_map[ sink ].log_level;
	if( log_level == -1 )
		log_level = def_level;

	if ( (~flags & CON_FLAG_ALWAYS) &&
	      log_level < lvl )
	      return;

	puts_r = con_sink_map[ sink ].puts_r;
	puts_s = con_sink_map[ sink ].puts_s;

	if ( puts_r )
		puts_r( sink, flags, hnd, lvl, msg );

	if ( !puts_s )
		return;

	dn = con_get_src_display_name( hnd );

	snprintf( line_buffer, sizeof line_buffer, "%s: %s: %s\n", con_sink_map[ sink ].name, dn, msg );

	puts_s( sink, flags, line_buffer );

}

void con_set_sink_level( const char* name, int level ) {
	int sink;

	sink = con_get_sink( name );

	if ( sink == -1 )
		return;

	con_sink_map[ sink ].log_level = level;
	printf( CON_DEBUG, "set sink %s (%i) to level %x", con_sink_map[ sink ].name, sink, level);
}

void con_set_log_level( int level ) {
	def_level = level;
}

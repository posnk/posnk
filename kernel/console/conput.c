#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "kernel/heapmm.h"
#define CON_PRIVATE
#include "kernel/console.h"
#include "config.h"
#include <string.h>

static void bmputs(
	uint32_t sinkmap,
	int flags,
	int hnd,
	int lvl,
	const char *msg ) {
	int sink;

	for ( sink = 0; sink <= CON_SINK_MAX; sink++ ) {
		if ( sinkmap & ( 1 << sink ) ) {
			con_sink_puts( sink, flags, hnd, lvl, msg );
		}
	}
}

void con_hputs( int hnd, int lvl, const char *msg ) {
	uint32_t lbm, dbm;

	/* Ensure hnd is valid */
	if (
		hnd < 0 ||
		hnd >= con_src_map_size ||
		(~con_src_map[ hnd ].flags & CON_SRC_USED) ) {
		con_hprintf(
			CON_FALLBACK_SRC,
			CON_ERROR,
			"Invalid source referenced: %i",
			hnd );
		hnd = CON_FALLBACK_SRC;
	}

	/* Ensure lvl is valid */
	if ( lvl < 0 || lvl > CON_LEVEL_MAX ){
		con_hprintf(
			CON_FALLBACK_SRC,
			CON_ERROR,
			"Invalid log level: %i",
			lvl );
		hnd = CON_FALLBACK_SRC;
		lvl = CON_ERROR;
	}

	/* First, output the message to any overridden sinks */
	lbm = con_src_map[ hnd ].level_sinkmap[ lvl ];
	/*
	 * These sinks are specifically wired in, so they should not check their
	 * level setting */
	bmputs( lbm, CON_FLAG_ALWAYS, hnd, lvl, msg );

	/* Then, if there are any catchall sinks that were not used, do those */
	dbm = con_src_map[ hnd ].def_sinkmap;
	if ( !dbm )
		dbm = con_def_sinkmap;
	lbm = dbm & ~lbm;
	bmputs( lbm, 0, hnd, lvl, msg );

}

//TODO: Make thread safe
static char line_buffer[256];

void con_vhprintf( int hnd, int lvl, const char *fmt, va_list args ) {


	vsnprintf( line_buffer, sizeof line_buffer, fmt, args );

	con_hputs( hnd, lvl, line_buffer );

}

void con_hprintf( int hnd, int lvl, const char *fmt, ... ) {

	va_list args;
	va_start( args, fmt );

	con_vhprintf( hnd, lvl, fmt, args );

	va_end( args );
}


void con_puts( const char *src, int lvl, const char * msg ) {
	int hnd;

	hnd = con_ref_src( src );

	con_hputs( hnd, lvl, msg );

}

void con_vprintf( const char *src, int lvl, const char *fmt, va_list args ) {

	int hnd;

	hnd = con_ref_src( src );

	vsnprintf( line_buffer, sizeof line_buffer, fmt, args );

	con_hputs( hnd, lvl, line_buffer );

}


void con_printf( const char *src, int lvl, const char *fmt, ... ) {
	va_list args;
	int hnd;

	va_start( args, fmt );

	hnd = con_ref_src( src );

	con_vhprintf( hnd, lvl, fmt, args );

	va_end( args );
}

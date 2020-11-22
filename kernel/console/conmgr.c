#include <stdint.h>
#include "kernel/heapmm.h"
#define CON_PRIVATE
#include "kernel/console.h"
#include "kernel/earlycon.h"
#include "config.h"

uint32_t con_def_sinkmap;

int con_register_src( const char *name ) {
	int hnd;

	/* If the source already exists, return it */
	hnd = con_find_src( name );
	if ( hnd >= 0 )
		return hnd;

	/* If the source does not exist, create it with default settings */
	hnd = con_alloc_src();
	if ( hnd == -1 ) {
		con_hprintf(
			CON_FALLBACK_SRC,
			CON_ERROR,
			"Could not create source \"%s\", it will be set to use"
			"the fallback source.",
			name );
		return CON_FALLBACK_SRC; /* If no source could be allocated,
		           * return the fallback source */
	}

	/* TODO: Check segment of name, and if necessary, copy it */
	con_src_map[ hnd ].name     = name;
	con_src_map[ hnd ].dispname = name;
	con_src_map[ hnd ].def_sinkmap = 0;

	return hnd;
}

int con_ref_src( const char *name ) {
	#ifndef no_auto_src
	return con_register_src( name );
	#else
	int hnd;

	if ( !name )
		return CON_FALLBACK_SRC;

	/* If the already exists, return it */
	hnd = con_find_src( name );
	if ( hnd >= 0 )
		return hnd;

	/* Otherwise, use fallback */
	return CON_FALLBACK_SRC;

	#endif
}

void con_release_src( __attribute__((unused)) int hnd ) {
	/* For now, this is a no-op */
}

void con_init() {

	/* Setup the default sink map  */
	con_def_sinkmap = 1 << CON_FALLBACK_SRC;

	/* Setup the default source map */
	con_src_map[ CON_FALLBACK_SRC ].hnd         = 0;
	con_src_map[ CON_FALLBACK_SRC ].flags       = CON_SRC_USED;
	con_src_map[ CON_FALLBACK_SRC ].name        = "fallback";
	con_src_map[ CON_FALLBACK_SRC ].dispname    = "???";
	con_src_map[ CON_FALLBACK_SRC ].def_sinkmap = 0;

	/* Initialize early-console drivers */
	earlycon_init();

}

const char *con_get_src_display_name( int hnd ) {

	if ( ~con_src_map[ hnd ].flags & CON_SRC_USED )
		hnd = CON_FALLBACK_SRC;

	return con_src_map[ hnd ].dispname;
}

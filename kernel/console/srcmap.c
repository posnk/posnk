#include <stdint.h>
#include <string.h>
#include "kernel/heapmm.h"
#define CON_PRIVATE
#include "kernel/console.h"
#include "config.h"

static con_src_t premem_src_map[ CONFIG_CON_INIT_SRCMAP_SZ ];

con_src_t *con_src_map = premem_src_map;
int con_src_map_size = CONFIG_CON_INIT_SRCMAP_SZ;

/* Do not call without management lock on the console source map */
static int find_free_src() {
	int i;
	for ( i = 0; i < con_src_map_size; i++ ) {
		if ( ~con_src_map[i].flags & CON_SRC_USED )
			return i;
	}
	return -1;
}

int con_find_src( const char *name ) {
	int i;

	for ( i = 0; i < con_src_map_size; i++ ) {

		if ( ~con_src_map[i].flags & CON_SRC_USED )
			continue;

		if ( strcmp( name, con_src_map[i].name ) == 0 )
			return i;
	}

	return -1;
}

/**
 * Grow the source map
 * @return Negative on failure, otherwise 0
 */
static int grow_con_src_map() {
	con_src_t *new_map;
	int new_sz;
	/* Not enough space, grow source map */
	new_sz = con_src_map_size * 2;

	new_map = heapmm_alloc( sizeof(con_src_t) * new_sz );
	if ( !new_map ) {
		//TODO: Handle OOM
		return -1;
	}

	/* Zero the allocated memory and copy over the old data */
	memset( new_map, 0, sizeof(con_src_t) * new_sz );
	memcpy( new_map, con_src_map, sizeof(con_src_t) * con_src_map_size );

	/* Free the old map */
	if ( con_src_map != premem_src_map ) {
		heapmm_free( con_src_map, sizeof( con_src_t ) * con_src_map_size );
	}

	/* Update map */
	con_src_map = new_map;
	con_src_map_size = new_sz;

	return 0;
}

int con_alloc_src() {
	int hnd;

	hnd = find_free_src();

	if ( hnd == -1 ) {
		/* Grow the map and retry */
		if ( grow_con_src_map() ) {
			//TODO: Handle OOM
			return -1;
		}
		/* This will never fail, as the map has just been resized */
		hnd = find_free_src();
	}

	/* Initialize the source structure */
	con_src_map[ hnd ].hnd = hnd;
	con_src_map[ hnd ].flags |= CON_SRC_USED;
	return hnd;

}

#include <stdint.h>
#include <string.h>
#include "kernel/heapmm.h"
#define CON_SRC "console"
#define CON_PRIVATE
#include "kernel/console.h"
#include "config.h"

extern uintptr_t cmdline_parse_hex( const char *str );

static const char *level_names[] = {
	"panic", "error", "warn", "info", "debug", "trace"
};

static char *copy_name( const char *name ) {
	char *name_out;
	size_t len;
	len = strlen(name);
	if ( len > 64 )
		return "badname!!!";
	name_out = heapmm_alloc( len + 1 );
	strcpy( name_out, name );
	return name_out;
}

static int parse_level( const char *level ) {
	int i;
	for ( i = 0; i <= CON_LEVEL_MAX; i++ ) {
		if ( strcmp( level, level_names[i] ) == 0 )
			return i;
	}
	return cmdline_parse_hex( level );
}

static int handle_src_prop(
	__attribute__((unused)) const char *src,
	__attribute__((unused)) const char *prop,
	__attribute__((unused)) const char *value ) {
	return 0;
}

static int handle_sink_prop( const char *src, const char *prop, const char *value ) {

	if ( strcmp( prop, "level" ) == 0 ) {
		con_set_sink_level( copy_name(src), parse_level( value ) );
		return 1;
	}
	return 0;
}

/* syntax con.<src|sink>.<name>.<prop> = <val> */
int con_handle_cmdline( const char *field, const char *value ) {
	char val_work[256];
	char *base, *prop, *src, *sink;

	strncpy(val_work, field, sizeof val_work );

	base = strtok( val_work, "." );
	if ( strcmp( base, "con" ) != 0 )
		return 0;

	base = strtok( NULL, "." );
	if ( base && strcmp( base, "src" ) == 0 ) {
		src = strtok( NULL, "." );
		prop = strtok( NULL, "." );
		if ( !src || !prop )
			return 0;
		printf( CON_TRACE, "src %s property %s val %s", src, prop, value);
		return handle_src_prop( src, prop, value );
	} else if ( base && strcmp( base, "sink" ) == 0 ) {
		sink = strtok( NULL, "." );
		prop = strtok( NULL, "." );
		if ( !sink || !prop )
			return 0;
		printf( CON_TRACE, "sink %s property %s val %s", sink, prop, value);
		return handle_sink_prop( sink, prop, value );
	} else if ( base && strcmp( base, "level" ) == 0 ) {
		printf( CON_INFO, "default loglevel %s",  value);
		con_set_log_level( parse_level( value ) );
		return 1;
	} else
		return 0;
}

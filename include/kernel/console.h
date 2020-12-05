/**
 * kernel/console.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 19-11-2020 - Created
 */

#ifndef __KERNEL_CONSOLE_H__
#define __KERNEL_CONSOLE_H__

#include <stddef.h>
#include <stdarg.h>

/* Console log levels */
#define CON_PANIC     (0)
#define CON_ERROR     (1)
#define CON_WARN      (2)
#define CON_INFO      (3)
#define CON_DEBUG     (4)
#define CON_TRACE     (5)
#define CON_LEVEL_MAX (5)

/* Console source handles */
#define CON_FALLBACK_SRC (0)
#define CON_PREINIT_SRC  (CON_FALLBACK_SRC)
#define CON_UNCONFIG_SRC_NAME ("fallback")
typedef void (*con_puts_r_t)( int sink, int flags, int hnd, int lvl, const char *msg );
typedef void (*con_puts_s_t)( int sink, int flags, const char *msg );
#define CON_SINK_EARLY (1 << 0)
#define CON_SINK_DEBUG (1 << 1)

/* Console private definitions */

#ifdef CON_PRIVATE

#define CON_SRC_USED (1 << 0)

#define CON_FLAG_ALWAYS (1 << 0)

struct con_src {

	/** The source handle used to refer to this source */
	int hnd;

	/** Flags for this source */
	uint32_t flags;

	/** The name of this source */
	const char *name;

	/** The display name of this source */
	const char *dispname;

	/** Routing information for this source */
	uint32_t def_sinkmap;

	/** Fine grained routing and level information for this source */
	uint32_t level_sinkmap[ CON_LEVEL_MAX + 1 ];

};

#define CON_SINK_MAX 31

struct con_sink {
	const char *name;
	int log_level;
	uint32_t flags;
	con_puts_r_t puts_r;
	con_puts_s_t puts_s;
};

int con_alloc_src();
int con_find_src( const char *name );
int con_ref_src( const char *name );

typedef struct con_src con_src_t;
typedef struct con_sink con_sink_t;

extern con_sink_t con_sink_map[];
extern con_src_t *con_src_map;
extern uint32_t con_def_sinkmap;
extern int con_src_map_size;

void con_sink_puts( int sink, int flags, int hnd, int lvl, const char *msg );
#endif

void earlycon_init(); //TODO: Remove

void debugcon_init(); //TODO: Remove

void con_init();

const char *con_get_src_display_name( int hnd );

int  con_register_src( const char *name );

void con_release_src( int hnd );

int  con_get_sink( const char *name );

int  con_register_sink_s( const char *name, int flags, con_puts_s_t puts );
int  con_register_sink_r( const char *name, int flags, con_puts_r_t puts );

void con_set_sink_level( const char *name, int level );

void con_set_log_level( int level );

void con_hputs( int hnd, int lvl, const char *msg );

void con_hprintf( int hnd, int lvl, const char *fmt, ... );

void con_puts( const char *src, int lvl, const char *msg );

void con_printf( const char *src, int lvl, const char *fmt, ... );

void con_vhprintf( int hnd, int lvl, const char *fmt, va_list args );

void con_vprintf( const char *src, int lvl, const char *fmt, va_list args );

int con_handle_cmdline( const char *field, const char *value );

#ifndef CON_SRC
#define CON_SRC (CON_UNCONFIG_SRC_NAME)
#endif
#ifdef CON_USE_HANDLE
static int con_src = CON_PREINIT_SRC;
#define con_src_init()           do { \
                                       con_src = con_register_src(CON_SRC);\
                                 } while(0)
#define printf( lvl, ... )  con_hprintf( con_src, lvl, __VA_ARGS__ )
#define vprintf( lvl, fmt, args )  con_vhprintf( con_src, lvl, fmt, args )
#define printf( lvl, msg )       con_hputs  ( con_src, lvl, msg )
#else
#define con_src_init()
#define puts( lvl, msg )         con_puts  ( CON_SRC, lvl, msg )
#define printf( lvl, ... )  con_printf( CON_SRC, lvl, __VA_ARGS__ )
#define vprintf( lvl, fmt, args )  con_vprintf( CON_SRC, lvl, fmt, args )
#endif

#endif

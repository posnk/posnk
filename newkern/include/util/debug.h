/**
 * util/debug.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-04-2014 - Changed names, added comments
 */
 
 #ifndef __UTIL_DEBUG_H__
 #define __UTIL_DEBUG_H__
 
 #include <stddef.h>
 
 void debug_dump_state(void *state, size_t state_size);
 void debug_postmortem_hook(void *state, size_t state_size, void *instr_addr);
 #endif

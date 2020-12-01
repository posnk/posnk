/*
 * kdbg/dbgapi.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#include <stdint.h>
#include <stddef.h>

void kdbg_initialize();

void dbgapi_set_symtab( const void *_symtab, const void *_strtab, int symcount, uint32_t str_sz );

void dbgapi_register_memuse(void *addr, size_t size);

void dbgapi_unreg_memuse(void *addr, size_t size);

void dbgapi_invoke_kdbg(int crash);

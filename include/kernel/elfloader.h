/*
 * kernel/elfloader.h
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 23-04-2014 - Created
 */

#include "kernel/vfs.h"
#include <string.h>

int elf_verify( const char *header, size_t size );

int elf_load( inode_t *inode );

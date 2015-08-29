/**
 * fs/ext2/math.c
 * 
 * Implements math support functions required for the ext2 filesystem
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-07-2014 - Created
 */

#include <stdint.h>

uint32_t ext2_divup(uint32_t a, uint32_t b)
{
	uint32_t result = a / b;
	if ( a % b ) { result++; }
	return result;
}

uint32_t ext2_roundup(uint32_t a, uint32_t b)
{//XXX: Works only with power-of-two values for B
	b--;
	return (a+b) & ~b;
}

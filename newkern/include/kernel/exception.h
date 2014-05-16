/**
 * kernel/exception.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-04-2014 - Created
 */

#ifndef __KERNEL_EXCEPTION_H__
#define __KERNEL_EXCEPTION_H__

#include <stddef.h>

#define EXCEPTION_DIV_ZERO		0
#define EXCEPTION_DEBUG			1
#define EXCEPTION_OVERFLOW		2
#define EXCEPTION_BOUNDS_CHECK		3
#define EXCEPTION_INVALID_OPCODE	4
#define EXCEPTION_NO_FLOAT_UNIT		5
#define EXCEPTION_DOUBLE_FAULT		6
#define EXCEPTION_SEG_FAULT		7
#define EXCEPTION_STACK_EXCEPTION	8
#define EXCEPTION_PROTECTION_FAULT	9
#define EXCEPTION_PAGE_FAULT		10
#define EXCEPTION_FLOAT_UNIT_ERROR	11

void exception_handle(int exception, void *instr_pointer, void *state, size_t state_size);

#endif

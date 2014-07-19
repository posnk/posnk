/**
 * @file crt/assert.h
 *
 * @brief Implements C style assertions
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * @li 02-04-2014 - Changed names, added comments
 */
 
#ifndef __ASSERT_H__
#define __ASSERT_H__
 
#include <stddef.h>

#include "kernel/earlycon.h" 

#include "kernel/system.h" 

#define softassert(AssertConditionArg) \
do { if (AssertConditionArg) \
	debugcon_printf("\n\nKernel assertion failed at %s:%i %s :\n%s\n", \
		__FILE__, __LINE__, __FUNCTION__, #AssertConditionArg); \
} while (0)

#define hardassert(AssertConditionArg) \
do { if (AssertConditionArg) \
	debugcon_printf("\n\nKernel assertion failed at %s:%i %s :\n%s\nHalting system!", \
		__FILE__, __LINE__, __FUNCTION__, #AssertConditionArg); \
	halt();\
} while (0)


#define assert hardassert

#endif

/**
 * sys/mcontext.h
 *
 * Part of P-OS.
 *
 *
 * Written by Peter Bosch <me@pbx.sh>
 */

#ifndef __SYS_MCONTEXT_H__
#define __SYS_MCONTEXT_H__

#ifdef __i386__
#include <sys/machine/i386/mcontext.h>
#endif

#ifdef __arm__
#include <sys/machine/armv7/mcontext.h>
#endif

#endif

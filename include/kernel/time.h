/**
 * kernel/time.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

 #ifndef __KERNEL_TIME_H__
 #define __KERNEL_TIME_H__

#include <stdint.h>

typedef uint64_t		ticks_t;

typedef uint64_t		ktime_t;

typedef uint64_t                utime_t;

extern ticks_t timer_ticks;

extern ticks_t timer_freq;

extern ticks_t timer_mfreq;

extern ktime_t system_time;

extern ktime_t system_time_micros;

void timer_interrupt(void);

 #endif

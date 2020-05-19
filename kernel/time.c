/* 
 * kernel/time.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 24-05-2014 - Created
 */

#include "kernel/time.h"
#include "kernel/scheduler.h"

ticks_t timer_ticks_m;
ticks_t timer_ticks_s;
ticks_t timer_ticks;

ticks_t timer_freq;

ticks_t timer_mfreq;

ktime_t system_time_micros = 0;

ktime_t system_time = 0;

void timer_interrupt()
{
	timer_ticks++;
	timer_ticks_s++;
	timer_ticks_m++;
	if (timer_ticks_s >= timer_freq){
		system_time++;
		timer_ticks_s = 0;
	}
	if (timer_ticks_m >= timer_mfreq){
		system_time_micros+=1000;
		timer_ticks_m = 0;
	}
}

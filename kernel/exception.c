/**
 * kernel/exception.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-04-2014 - Created
 */

#include <stddef.h>
#include <stdint.h>
#include <signal.h>
#define CON_SRC "exception"
#include "kernel/console.h"
#include "kernel/system.h"
#include "kernel/signals.h"
#include "kernel/scheduler.h"
#include "util/debug.h"

void exception_panic( int sig, void *instr_pointer )
{
	printf(CON_PANIC, "PANIC! Unhandled kernelmode exception: (%i)", sig);
	printf(CON_PANIC, "Exception occurred at 0x%x", instr_pointer);
	printf(CON_PANIC, "Register dump: ");
	debug_dump_state();
	debug_postmortem_hook();
	printf(CON_PANIC, "Halting processor.");
	halt();
}

void exception_handle( int sig, struct siginfo info, void *instr, int forceusr, int kern )
{
	if ((!forceusr) && kern) {
		//TODO: Kernel mode exception
		exception_panic(sig,instr);
	} else {
		printf( CON_WARN, "Userland exception: (%i), sending signal!", sig);
		printf( CON_WARN, "Exception occurred at 0x%x", instr);
		printf( CON_WARN, "Register dump: ");
		debug_dump_state();
		thread_send_signal(scheduler_current_task, sig, info ); //TODO: Look up appropriate signal for exception
	}
}

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
#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/signals.h"
#include "kernel/scheduler.h"
#include "util/debug.h"

void exception_panic( int sig, void *instr_pointer )
{
	panic_printf("PANIC! Unhandled kernelmode exception: (%i)\n",sig);
	earlycon_printf("PANIC! Unhandled kernelmode exception: (%i)\n", sig);
	earlycon_printf("Exception occurred at 0x%x\n", instr_pointer);
	earlycon_printf("Register dump: \n");
	debug_dump_state();
	debug_postmortem_hook();
	earlycon_printf("Halting processor.\n");
	halt();
}
 
void exception_handle( int sig, struct siginfo info, void *instr, int forceusr )
{
	if ((!forceusr) &&((uintptr_t)instr) > 0xC0000000) {
		//TODO: Kernel mode exception
		exception_panic(sig,instr);
	} else {
		debugcon_printf("Userland exception: (%i), sending signal!\n", sig);
		debugcon_printf("Exception occurred at 0x%x\n", instr);
		debugcon_printf("Register dump: \n");
		debug_dump_state();
		process_send_signal(current_process, sig, info ); //TODO: Look up appropriate signal for exception
	}
}

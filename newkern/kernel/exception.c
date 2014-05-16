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
#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/signals.h"
#include "kernel/scheduler.h"
#include "util/debug.h"
 
char * exception_names[] = {
	"Division by zero",
	"Debugger trap",
	"Arithmethic overflow",
	"Bounds check",
	"Invalid opcode",
	"No floating-point unit",
	"Double fault",
	"Segmentation fault",
	"Stack exception",
	"Protection fault",
	"Unresolved page fault",
	"Floating-point unit error"
};

void exception_panic(int exception, void *instr_pointer, void *state, size_t state_size)
{
	earlycon_printf("PANIC! Unhandled kernelmode exception: %s(%i)\n",exception_names[exception],exception);
	earlycon_printf("Exception occurred at 0x%x\n", instr_pointer);
	earlycon_printf("Register dump: \n");
	debug_dump_state(state, state_size);
	debug_postmortem_hook(state, state_size, instr_pointer);
	earlycon_printf("Halting processor.\n");
	halt();
}
 
void exception_handle(int exception, void *instr_pointer, void *state, size_t state_size)
{
	if (((uintptr_t)instr_pointer) > 0xC0000000) {
		//TODO: Kernel mode exception
		exception_panic(exception, instr_pointer, state, state_size);
	} else {
		process_send_signal(scheduler_current_task, SIGSEGV); //TODO: Look up appropriate signal for exception
	}
}

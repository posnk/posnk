/**
 * kdbg/kdbg.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#include <stdint.h>
#include "kdbg/stacktrc.h"
#include "kdbg/kdbgmm.h"
#include "kdbg/kdbgio.h"
#include "kdbg/dbgapi.h"
#include "kdbg/heapdbg.h"
#include "kernel/system.h"
#include "kernel/heapmm.h"
#include <string.h>

void kdbg_shell();
void kdbg_initialize()
{
	size_t initial_heap = 8192;	
	kdbg_init_memuse();
	initial_heap = heapmm_request_core((void *)0xE8000000, initial_heap);

	if (initial_heap == 0) {
		earlycon_puts("FAIL\nCould not allocate first page of heap!\n");
		for(;;); //TODO: PANIC!!!		
	}

	kdbgmm_init((void *)0xE8000000, initial_heap);
}

void kdbg_enter()
{
#ifdef ARCH_I386
	asm("cli;");
#endif
#ifndef ARCH_I386
//#warn Unimplemented arch-specific code
#endif
}

void kdbg_exit()
{

}
void kdbg_dump_processes();
void kdbg_attach_process(int pid);
void dbgapi_invoke_kdbg( __attribute__((unused)) int crash)
{
	kdbg_enter();
	kdbg_shell();
	kdbg_exit();
}

char buf[80];

void kdbg_shell()
{
	for (;;) {
		kdbg_printf("debug#");
		kdbg_gets(buf, 80);
		if (!strncmp(buf, "memuse", 6)) {
			kdbg_print_memuse_brdr((void*)kdbg_parsehex(buf));
		} 
		if (!strncmp(buf, "smu", 3)) {
			kdbg_enable_memuse();
		} else if (!strncmp(buf, "exit", 4)) {
			kdbg_printf("Bye\n");
			return;
		} else if (!strncmp(buf, "attach", 6)) {
			kdbg_attach_process(kdbg_parsedec(buf));
		} else if (!strncmp(buf, "shutdown", 8)) {
			shutdown();
		} else if (!strncmp(buf, "ps", 2)) {
			kdbg_dump_processes();
		} else {
			kdbg_printf("Unknown command: %s\n", buf);
		}
	}
}

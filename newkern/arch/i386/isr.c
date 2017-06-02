/**
 * arch/i386/isr.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-04-2014 - Created
 */

#include <stdint.h>
#include <string.h>
#include "arch/i386/pic.h"
#include "arch/i386/x86.h"
#include "arch/i386/isr_entry.h"
#include "kernel/exception.h"
#include "kernel/earlycon.h"
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/system.h"
#include "kernel/signals.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/interrupt.h"
#include "kernel/time.h"
#include "util/debug.h"


void *i386_get_page_fault_addr() {
	uint32_t cr2;
	asm volatile("mov %%cr2, %0": "=r"(cr2));
	return (void *) cr2;
}
//void i386_register_hardware_isr(int irq,void *isr){
//}

void i386_exception_handle( i386_isr_stack_t *stack )
{
	int				sig;
	struct siginfo	info;
	memset ( &info, 0, sizeof( struct siginfo ) );
	switch ( stack->int_id ) {
		case I386_EXCEPTION_DIV_ZERO:
			sig = SIGFPE;
			info.si_code = FPE_INTDIV;
			break;
		case I386_EXCEPTION_OVERFLOW:
			sig = SIGFPE;
			info.si_code = FPE_INTOVF;
			break;
		case I386_EXCEPTION_DEBUG:
			sig = SIGTRAP;
			info.si_code = TRAP_TRACE;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_BREAKPOINT:
			sig = SIGTRAP;
			info.si_code = TRAP_BRKPT;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_BOUNDS_CHECK:
			sig = SIGILL;
			info.si_code = ILL_ILLOPN;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_INVALID_OPCODE:
			sig = SIGILL;
			info.si_code = ILL_ILLOPC;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_NO_COPROCESSOR:
			sig = SIGILL;
			info.si_code = ILL_COPROC;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_DOUBLE_FAULT:
			sig = SIGILL;
			info.si_code = ILL_BADSTK;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_COPROCESSOR_OVR:
			sig = SIGSEGV;
			info.si_code = SEGV_MAPERR;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_SEG_FAULT:
		case I386_EXCEPTION_STACK_EXCEPTION:
			sig = SIGILL;
			info.si_code = ILL_ILLADR;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_GP_FAULT:
			sig = SIGILL;
			info.si_code = ILL_PRVOPC;
			info.si_addr = (void*) stack->eip;
			break;
		case I386_EXCEPTION_COPROCESSOR_ERR:
			sig = SIGFPE;
			info.si_code = FPE_FLTINV;
			break;
		default:
			sig = SIGILL;
			info.si_addr = (void*) stack->eip;
			info.si_code = ILL_ILLTRP;
			break;
	};

	exception_handle( sig, info, (void*) stack->eip );
	
}

/**
 * Interrupt handler!
 */

void sercon_isr();

void i386_handle_interrupt( i386_isr_stack_t *stack )
{	

	int int_id = stack->int_id;

	if ( stack->cs == 0x2B ) {
		/* We came from userland */
		i386_user_enter( stack );
	}
	i386_kern_enter( stack );
	if (int_id == 0x80) {
		/* System call */
		syscall_dispatch((void *)stack->regs.eax, (void *) stack->eip);
	} else if (int_id == 32) {
		//debugcon_printf("tick!\n");
		i386_interrupt_done (int_id - 32);
		timer_interrupt();
	} else if (int_id == 35) {
		i386_interrupt_done (int_id - 32);
		sercon_isr();
	} else if (int_id > 31) {
		/* Hardware Interrupt */
		//earlycon_printf("Unhandled hardware interrupt %i\n", int_id - 32);
		if ( int_id == (39) ) {
			if ( !( i386_read_isr() & 0x80 ) )
				return;
		}
		i386_interrupt_done (int_id - 32);
		interrupt_dispatch(int_id - 32);
	} else if (int_id == I386_EXCEPTION_PAGE_FAULT) {
		/* Dispatch to page fault handler */
		paging_handle_fault(
			i386_get_page_fault_addr(), 
			(void *)stack->eip,
			stack->error_code & 1,
			stack->error_code & 2,
			stack->error_code & 4);
	} else if (	(int_id == I386_EXCEPTION_NO_COPROCESSOR) || 
				(int_id == I386_EXCEPTION_INVALID_OPCODE)) {
		if (!i386_fpu_handle_ill())
			i386_exception_handle( stack );
	} else {
		/* Handle regular exceptions */
		i386_exception_handle( stack );
	} 

	do_signals();
	if ( stack->cs == 0x2B ) {
		/* We came from userland */
		i386_user_exit( stack );
	}
}

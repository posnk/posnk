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
#include "arch/i386/x86.h"
#include "arch/i386/isr_entry.h"
#include "driver/platform/platform.h"
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
#include <assert.h>
/*
	uint32_t				ds;
	i386_pusha_registers_t	regs;
	uint32_t				int_id;
	uint32_t				error_code;
	uint32_t				eip;
	uint32_t				cs;
	uint32_t				eflags;
	uint32_t				esp;
	uint32_t				ss;*/
void dumpisrstack( i386_isr_stack_t *stack ) {
	int pid = -1;
	i386_pusha_registers_t	*regs = &stack->regs;
	debugcon_printf("\n\nEAX: 0x%X EBX: 0x%X ECX: 0x%X EDX: 0x%X\n",
		regs->eax, regs->ebx, regs->ecx, regs->edx);
	if ( stack->cs == 0x2B )
		debugcon_printf("\n\nSS: 0x%x\tESP:0x%x\n",
						stack->ss, stack->esp );
	debugcon_printf("ESP: 0x%x EBP: 0x%X ESI: 0x%X EDI: 0x%X\n",
		regs->esp, regs->ebp, regs->esi, regs->edi);
	debugcon_printf("DS: 0x%x\t CS: 0x%x\t EIP: 0x%x\t EFLAGS:0x%x\n",
					stack->ds, stack->cs, stack->eip, stack->eflags );
	debugcon_printf("int: %i\t ECODE: 0x%x sctp:%x cpp: %x\n",
					stack->int_id, stack->error_code,
					scheduler_current_task,
					scheduler_current_task->process
					);
	if ( scheduler_current_task->process )
		pid = scheduler_current_task->process->pid;
	debugcon_printf("tid:%x\n pid:%x\n\n\n",
					scheduler_current_task->tid,pid);
}

/**
 * Reads the faulting address from CR2.
 */
void *i386_get_page_fault_addr() {
	uint32_t cr2;
	asm volatile("mov %%cr2, %0": "=r"(cr2));
	return (void *) cr2;
}

/**
 * Handle a CPU exception.
 * This transforms the architectural exception into the appropriate
 * POSIX signal and passes it on to the portable kernel.
 */
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
	dumpisrstack( stack );
	exception_handle( sig, info, (void*) stack->eip,0,stack->cs == 0x2B );

}

/**
 * Interrupt handler!
 */
void i386_handle_interrupt( i386_isr_stack_t *stack )
{
	uint32_t scpf;
	int s;
	int int_id = stack->int_id, hw_int;

	s = disable();

	/* Check if the interrupt came from ring 3 */
	if ( stack->cs == 0x2B ) {

		assert( stack->eip < 0xc0000000u );

		/* If it did, we need to record the userland task state */
		i386_user_enter( stack );

	} else
		assert( stack->eip >= 0xc0000000u );

	/* Record the interrupted state */
	i386_kern_enter( stack );

	/* Disambiguate the various types of interrupts we can take */
	if (int_id == 0x80) {

		/* Legacy system call
		 * For a legacy system call, all arguments are contained in a struct
		 * passed by reference through the EAX register. */
		syscall_dispatch((void *)stack->regs.eax, (void *) stack->eip);

	} else if (int_id == 0x81) {
		/* System call ( new ABI )*/

		/* The new syscall ABI passes 6 arguments by register, and one by
		 * stack. We need to verify that the user has at least one word pushed
		 * onto their stack before proceeding */
		if ( !procvmm_check( (uint32_t *) stack->esp, sizeof(uint32_t) ) ) {

			/* The address was not present. Handle the fault as a normal PF */
			paging_handle_fault(
				(void *)stack->esp,
				(void *)stack->eip,
				0,
				0,
				1);

		} else {

			/* We can access the memory, proceed to fetch the stack argument */
			scpf = *( (uint32_t *) stack->esp );

			/* and call the dispatcher with the arguments from the stack and
			 * registers */
			stack->regs.eax =
				syscall_dispatch_new(
						stack->regs.eax,
						stack->regs.ecx,
						stack->regs.edx,
						stack->regs.esi,
						stack->regs.edi,
						stack->regs.ebx,
						scpf );

			/* Pass the errno back through ECX */
			stack->regs.ecx = syscall_errno;

		}


	} else if ( int_id == 2 || int_id >= 32 ) {
		/* Hardware interrupt (NMI or vectored) */

		/* Ask the platform driver what hardware interrupt was fired */
		hw_int = platform_get_interrupt_id ( int_id );

		/* If the interrupt controller did not see the interrupt, it is called
		 * a spurious interrupt. This can happen due to inaccurate
		 * end-of-interrupt signalling or due to logic glitches in hardware.
		 * We need to make sure we don't try to act on the spurious interrupt
		 * as if it was real. */

		if ( hw_int != INT_SPURIOUS ) {
			/* If it was a real interrupt, dispatch it to the driver manager */
			interrupt_dispatch( hw_int );
		} else {
			//TODO: Track/report spurious interrupts
		}

		/* After the driver has cleared the interrupt, we need to clear the
		 * pending interrupt from the interrupt controllers to prevent it
		 * re-firing immediately */
		platform_end_of_interrupt( int_id, hw_int );

	} else if (int_id == I386_EXCEPTION_PAGE_FAULT) {
		/* Page faults are in many ways a special case */

		/* Unlike most other exceptions they are often not fatal, and are used
		 * for demang paging. For this reason, there is a separate code path
		 * dedicated to page fault handling */

		paging_handle_fault(
			i386_get_page_fault_addr(),
			(void *)stack->eip,
			stack->error_code & 1,
			stack->error_code & 2,
			stack->error_code & 4);

	} else if (	(int_id == I386_EXCEPTION_NO_COPROCESSOR) ||
				(int_id == I386_EXCEPTION_INVALID_OPCODE)) {
		/* Another pair of exceptions with special cased behaviour are the
		 * #NM and #UD faults. These are generated when an instruction is
		 * executed which the processor does not currently support. This can
		 * have two possible causes (in a modern system): the processor does not
		 * support the instruction, or, the instruction set extension was
		 * disabled. The latter is done to implement lazy context switching for
		 * the FPU/SIMD units: when a context switch occurs, the extensions are
		 * disabled until another program tries using them. When that happens
		 * this exception will fire and the kernel only then performs the
		 * context switch for the extended registers. */

		/* Check for and possibly resolve lazy-switch exceptions */
		if (!i386_fpu_handle_ill()) {
			/* The exception was a real one, invoke the fault handler */
			i386_exception_handle( stack );
		}
	} else {
		/* Handle regular exceptions */
		i386_exception_handle( stack );
	}

	/* Process signal dispatch for the current process */
	do_signals();

	/* Invoke the scheduler, which will make sure we have not exceeded our
	 * timeslice and that no higher-priority tasks have become available */
	schedule();

	if ( stack->cs == 0x2B ) {
		/* We came from userland */
		i386_user_exit( stack );
	} else {
		restore(s);
		if ( stack->cs != 0x2B )
			assert ( stack->eip >= (unsigned) 0xc0000000 );
	}
}

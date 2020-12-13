/**
 * arch/armv7/isr.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 15-03-2014 - Created
 */

#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "arch/armv7/exception.h"
#include "arch/armv7/cpu.h"
#include "arch/armv7/mmu.h"
#include "arch/armv7/taskctx.h"
#include "kernel/exception.h"
#include "kernel/earlycon.h"
#include "kernel/syscall.h"
#include "kernel/paging.h"
#include "driver/platform/platform.h"
#include "kernel/interrupt.h"
#include "kernel/console.h"

/**
 * When compiling using a linux toolchain we need to provide a raise() function
 * to handle division by zero errors
 */
int raise( __attribute__((unused)) int sig)
{
	int div_by_zero = 0;
	assert(div_by_zero);
	return 0;
}

void armv7_user_enter( armv7_exception_state_t *state ) {
	armv7_task_context_t *tctx = scheduler_current_task->arch_state;

	if ( tctx == 0 ) {
		printf(CON_ERROR, "entry to kernel without tctx");
		return;
	}

	tctx->user_pc = state->exc_lr;
	tctx->user_sp = state->usr_sp;
	tctx->user_lr = state->usr_lr;
	tctx->user_sr = state->usr_psr;
	memcpy( tctx->user_regs, state->usr_regs, sizeof tctx->user_regs );
}


void armv7_user_exit( armv7_exception_state_t *state ) {
	armv7_task_context_t *tctx = scheduler_current_task->arch_state;

	if ( tctx == 0 ) {
		printf(CON_ERROR, "exit from kernel without tctx");
		return;
	}

	state->exc_lr  = tctx->user_pc;
	state->usr_sp  = tctx->user_sp;
	state->usr_lr  = tctx->user_lr;
	state->usr_psr = tctx->user_sr;
	memcpy( state->usr_regs, tctx->user_regs, sizeof state->usr_regs );
}

/**
 * Interrupt handler!
 */

void armv7_handle_abort(uint32_t vec_id, armv7_exception_state_t *state)
{
	int sig;
	struct siginfo	info;

	uint32_t fault_addr, fault_status, is_user;

	if ( vec_id == VEC_DATA_ABORT ) {
		fault_addr = armv7_mmu_data_abort_addr();
		fault_status = armv7_mmu_data_abort_status();
	} else {
		fault_addr = armv7_mmu_pf_abort_addr();
		fault_status = armv7_mmu_pf_abort_status();
	}
	memset ( &info, 0, sizeof( struct siginfo ) );

	is_user = (state->usr_psr & PSR_MODE) == PSR_MODE_USR;
//	earlycon_printf("pagefault at 0x%x access to 0x%x fs:0x%x\n",state->exc_lr, fault_addr, fault_status);

	sig = SIGILL;
	info.si_addr = (void*) state->exc_lr;
	info.si_code = ILL_ILLTRP;

	switch (ARMV7_FSR_FS(fault_status)) {
		/* Alignment fault */
		case ARMV7_FS_ALIGNMENT_FAULT:
			sig = SIGBUS;
			info.si_code = BUS_ADRALN;
			info.si_addr = (void*) state->exc_lr; // TODO: Is this actually the correct addr
			break;

		case ARMV7_FS_ICACHE_MAINT_FAULT:
			//TODO: Interpret the cause of this fault
			break;
		/* External aborts and parity errors */
		case ARMV7_FS_EXT_SABORT_L1_TWALK:
		case ARMV7_FS_EXT_SABORT_L2_TWALK:
		case ARMV7_FS_SYNC_PARERR_L1_TWALK:
		case ARMV7_FS_SYNC_PARERR_L2_TWALK:
			//TODO: DIE!
			break;

		/* Access flag fault: ACCESS flag not set*/
		case ARMV7_FS_ACCESS_FLAG_FAULT_L1:
		case ARMV7_FS_ACCESS_FLAG_FAULT_L2:
		/* Translation fault: Entry marked as FAULT type */
		case ARMV7_FS_TRANSLATION_FAULT_L1:
		case ARMV7_FS_TRANSLATION_FAULT_L2:
			paging_handle_fault( 	(void *) fault_addr,
						(void *) state->exc_lr,
						0, 				/* PRESENT */
						fault_status & ARMV7_FSR_WNR, 	/* WRITE */
						is_user ) ; 			/* USER */
			return;

		/* Domain fault: Tried to access a domain marked as NO ACCESS */
		case ARMV7_FS_DOMAIN_FAULT_L1:
		case ARMV7_FS_DOMAIN_FAULT_L2:
		/* Permission fault: Tried to access a page in a manner not allowed by AS */
		case ARMV7_FS_PERM_FAULT_L1:
		case ARMV7_FS_PERM_FAULT_L2:
			paging_handle_fault( 	(void *) fault_addr,
						(void *) state->exc_lr,
						1, 				/* PRESENT */
						fault_status & ARMV7_FSR_WNR, 	/* WRITE */
						is_user ); 			/* USER */
			return;

		case ARMV7_FS_DEBUG_EVENT:
			sig = SIGTRAP;
			info.si_code = TRAP_TRACE;
			info.si_addr = (void*) state->exc_lr;
			break;
		case ARMV7_FS_EXT_SABORT:
		case ARMV7_FS_TLB_ABORT:
		case ARMV7_FS_SYNC_PARERR:
		case ARMV7_FS_EXT_AABORT:
		case ARMV7_FS_ASYNC_PARERR:
			//TODO: DIE!
			break;
	}
	exception_handle( sig, info, (void*) state->exc_lr, 0, is_user );
}

void armv7_interrupt(int int_chan, armv7_exception_state_t *state)
{
	int pl = platform_get_interrupt_id ( int_chan );
//	earlycon_printf("int: %i\n", pl);
	interrupt_dispatch( pl );
	platform_end_of_interrupt( int_chan, pl );
}

void armv7_exception_handler(uint32_t vec_id, armv7_exception_state_t *state)
{
	int irq, sig;
	struct siginfo	info;
	int is_user = (state->usr_psr & PSR_MODE) == PSR_MODE_USR;
	memset ( &info, 0, sizeof( struct siginfo ) );

	if ( is_user )
		armv7_user_enter( state );
	printf(CON_TRACE,"Exception %i at pc: %08X sp: %08X sr: %08X", vec_id, state->exc_lr, state->usr_sp, state->usr_psr);

	switch (vec_id) {
		case VEC_DATA_ABORT:
		case VEC_PREFETCH_ABORT:
			//earlycon_printf("exception %i at 0x%x\n", vec_id, state->exc_lr);
			armv7_handle_abort(vec_id, state);
			break;
		case VEC_FIQ:
			armv7_interrupt( 1, state );
			break;
		case VEC_IRQ:
			armv7_interrupt( 0, state );
			break;
		case VEC_SUPERVISOR_CALL:
			/* System call */
			//TODO: ARM FAST SYSCALL Support using r1-r4 for args
			syscall_dispatch(	(void *) state->usr_regs[0],
						(void *) state->exc_lr );
			break;
		case VEC_UNDEFINED:
			//TODO: Support emulation of missing processor features
		default:
			printf( CON_INFO,
				"exception %i at 0x%x\n", vec_id, state->exc_lr);
			sig = SIGILL;
			info.si_addr = (void*) state->exc_lr;
			info.si_code = ILL_ILLTRP;
			exception_handle( sig, info, (void*) state->exc_lr, 0, is_user );
			break;
	}

	/* Process signal dispatch for the current process */
	do_signals();

	/* Invoke the scheduler, which will make sure we have not exceeded our
	 * timeslice and that no higher-priority tasks have become available */
	schedule();

	if ( is_user )
		armv7_user_exit( state );
}

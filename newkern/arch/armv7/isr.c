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

#include <stdint.h>
#include <assert.h>
#include "arch/armv7/exception.h"
#include "arch/armv7/cpu.h"
#include "arch/armv7/mmu.h"
#include "arch/i386/x86.h"
#include "kernel/exception.h"
#include "kernel/earlycon.h"
#include "kernel/syscall.h"
#include "kernel/paging.h"

/**
 * When compiling using a linux toolchain we need to provide a raise() function
 * to handle division by zero errors
 */
int raise(int sig)
{
	int div_by_zero = 0;
	assert(div_by_zero);
	return 0;
}


/**
 * Interrupt handler!
 */

void armv7_handle_abort(uint32_t vec_id, armv7_exception_state_t *state)
{
	uint32_t fault_addr, fault_status, is_user;
	
	if ( vec_id == VEC_DATA_ABORT ) {
		fault_addr = armv7_mmu_data_abort_addr();
		fault_status = armv7_mmu_data_abort_status();
	} else {
		fault_addr = armv7_mmu_pf_abort_addr();
		fault_status = armv7_mmu_pf_abort_status();
	}

	is_user = (state->usr_psr & PSR_MODE) == PSR_MODE_USR;

	switch (ARMV7_FSR_FS(fault_status)) {
		/* Alignment fault */
		case ARMV7_FS_ALIGNMENT_FAULT:	
			exception_handle(EXCEPTION_SEG_FAULT,  
					(void *) state->exc_lr,
					(void *) state,
					sizeof(armv7_exception_state_t));
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
						(void *) state,
						sizeof(armv7_exception_state_t),
						0, 				/* PRESENT */
						fault_status & ARMV7_FSR_WNR, 	/* WRITE */
						is_user ) ; 			/* USER */
			break;

		/* Domain fault: Tried to access a domain marked as NO ACCESS */
		case ARMV7_FS_DOMAIN_FAULT_L1:
		case ARMV7_FS_DOMAIN_FAULT_L2:
		/* Permission fault: Tried to access a page in a manner not allowed by AS */
		case ARMV7_FS_PERM_FAULT_L1:
		case ARMV7_FS_PERM_FAULT_L2:
			paging_handle_fault( 	(void *) fault_addr,
						(void *) state->exc_lr,
						(void *) state,
						sizeof(armv7_exception_state_t),
						1, 				/* PRESENT */
						fault_status & ARMV7_FSR_WNR, 	/* WRITE */
						is_user ); 			/* USER */
			break;
		
		case ARMV7_FS_DEBUG_EVENT:
			exception_handle(EXCEPTION_DEBUG,  
					(void *) state->exc_lr,
					(void *) state,
					sizeof(armv7_exception_state_t));
			break;
		case ARMV7_FS_EXT_SABORT:
		case ARMV7_FS_TLB_ABORT:
		case ARMV7_FS_SYNC_PARERR:
		case ARMV7_FS_EXT_AABORT:
		case ARMV7_FS_ASYNC_PARERR:
			//TODO: DIE!
			break;
	}
}

void armv7_interrupt(int int_chan, armv7_exception_state_t *state)
{
	int pl = platform_get_interrupt_id ( int_chan );
	interrupt_dispatch( pl );
	platform_end_of_interrupt( int_chan, pl );
}

void armv7_exception_handler(uint32_t vec_id, armv7_exception_state_t *state)
{
	int irq;
	switch (vec_id) {
		case VEC_DATA_ABORT:
		case VEC_PREFETCH_ABORT:
			earlycon_printf("exception %i at 0x%x\n", vec_id, state->exc_lr);
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
			earlycon_printf("exception %i at 0x%x\n", vec_id, state->exc_lr);
			exception_handle(EXCEPTION_INVALID_OPCODE, 
					(void *) state->exc_lr,
					(void *) state,
					sizeof(armv7_exception_state_t));
			break;
	}
}

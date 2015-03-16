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
#include "arch/armv7/exception.h"
#include "arch/armv7/mmu.h"
#include "arch/i386/x86.h"
#include "kernel/exception.h"
#include "kernel/earlycon.h"
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/system.h"
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include "kernel/interrupt.h"
#include "kernel/time.h"
#include "util/debug.h"

/**
 * Interrupt handler!
 */

void armv7_handle_abort(uint32_t vec_id, armv7_exception_state_t *state)
{
	uint32_t fault_addr, fault_status;
	if ( vec_id == VEC_DATA_ABORT ) {
		fault_addr = armv7_mmu_data_abort_addr();
		fault_status = armv7_mmu_data_abort_status();
	} else {
		fault_addr = armv7_mmu_pf_abort_addr();
		fault_status = armv7_mmu_pf_abort_status();
	}
	switch (ARMV7_FSR_FS(fault_status)) {
#define ARMV7_FS_ALIGNMENT_FAULT	(0x01)
#define ARMV7_FS_ICACHE_MAINT_FAULT	(0x04)
#define ARMV7_FS_EXT_SABORT_L1_TWALK	(0x0C)
#define ARMV7_FS_EXT_SABORT_L2_TWALK	(0x0E)
#define ARMV7_FS_SYNC_PARERR_L1_TWALK	(0x1C)
#define ARMV7_FS_SYNC_PARERR_L2_TWALK	(0x1E)
#define ARMV7_FS_TRANSLATION_FAULT_L1	(0x05)
#define ARMV7_FS_TRANSLATION_FAULT_L2	(0x07)
#define ARMV7_FS_ACCESS_FLAG_FAULT_L1	(0x03)
#define ARMV7_FS_ACCESS_FLAG_FAULT_L2	(0x06)
#define ARMV7_FS_DOMAIN_FAULT_L1	(0x09)
#define ARMV7_FS_DOMAIN_FAULT_L2	(0x0B)
#define ARMV7_FS_PERM_FAULT_L1		(0x0D)
#define ARMV7_FS_PERM_FAULT_L2		(0x0F)
#define ARMV7_FS_DEBUG_EVENT		(0x02)
#define ARMV7_FS_EXT_SABORT		(0x08)
#define ARMV7_FS_TLB_ABORT		(0x10)
#define ARMV7_FS_SYNC_PARERR		(0x19)
#define ARMV7_FS_EXT_AABORT		(0x16)
#define ARMV7_FS_ASYNC_PARERR		(0x18)
	}
		//paging_handle_fault(i386_get_page_fault_addr(), (void *)instr_ptr, &registers, sizeof(i386_pusha_registers_t)
		//		, error_code & 1, error_code & 2, error_code & 4);
}

void armv7_exception_handler(uint32_t vec_id, armv7_exception_state_t *state)
{
	sercon_printf("exception %i at 0x%x", vec_id, state->exc_lr);
	switch (vec_id) {
		case VEC_DATA_ABORT:
		case VEC_PREFETCH_ABORT:
			armv7_handle_abort(vec_id, state);
			break;
		case VEC_FIQ:
			//TODO: Implement hardware interrupt handling
			break;
		case VEC_IRQ:
			//TODO: Implement hardware interrupt handling
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
			exception_handle(EXCEPTION_INVALID_OPCODE, 
					(void *) state->exc_lr,
					(void *) state,
					sizeof(armv7_exception_state_t));
			break;
	}
}

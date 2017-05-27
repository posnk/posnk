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
#include "arch/i386/pic.h"
#include "arch/i386/x86.h"
#include "arch/i386/isr_entry.h"
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

int i386_exception_table[] = 
{
	EXCEPTION_DIV_ZERO,
	EXCEPTION_DEBUG,
	-1,
	EXCEPTION_DEBUG,	
	EXCEPTION_OVERFLOW,
	EXCEPTION_BOUNDS_CHECK,
	EXCEPTION_INVALID_OPCODE,
	EXCEPTION_NO_FLOAT_UNIT,
	EXCEPTION_DOUBLE_FAULT,
	EXCEPTION_FLOAT_UNIT_ERROR,
	EXCEPTION_SEG_FAULT,
	EXCEPTION_SEG_FAULT,
	EXCEPTION_STACK_EXCEPTION,
	EXCEPTION_PROTECTION_FAULT,
	EXCEPTION_PAGE_FAULT,
	-1,
	EXCEPTION_FLOAT_UNIT_ERROR	
};



void *i386_get_page_fault_addr() {
	uint32_t cr2;
	asm volatile("mov %%cr2, %0": "=r"(cr2));
	return (void *) cr2;
}
//void i386_register_hardware_isr(int irq,void *isr){
//}

/**
 * Interrupt handler!
 */

void sercon_isr();

/*
;   SS         
;   ESP        <-- ESP before interrupt
;   EFLAGS
;   CS
;   EIP   	   <-- ESP at entry
;   EAX        <-------------------\
;---Switched                       |
;   ESP         -------------------+
;   EIP                            |
;   0x0BADC0DE <+- ESP at pusha    |
;   EAX  28     |                  |
;   ECX  24     |                  |
;   EDX  20     |                  |
;   EBX  16     |                  |
;   ESP  12  ---/ 1)               / 2)
;   EBP  8
;   ESI  4
;   EDI  0     <-- ESP a fter pusha
;   DS
;   interrupt_no
2    0    0    2    4    6
0010 0000 0000 0010 0100 0110   
            IO ODIT SZ A  P C
*/

void i386_handle_interrupt(
	uint32_t 								int_id, 
	__attribute__((__unused__)) uint32_t	data_segment, 
	i386_pusha_registers_t					registers, 
	uint32_t								error_code,
	uint32_t								instr_ptr)
{	
	i386_isr_stack_t *isrstack = (i386_isr_stack_t *) registers.esp;
	if ( isrstack->cs == 0x2B ) {
		/* We came from userland */
		i386_user_enter( registers, data_segment );
	}
	if (int_id == 0x80) {
		/* System call */
		syscall_dispatch((void *)registers.eax, (void *) instr_ptr);
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
		paging_handle_fault(i386_get_page_fault_addr(), (void *)instr_ptr, &registers, sizeof(i386_pusha_registers_t)
				, error_code & 1, error_code & 2, error_code & 4);
	} else if ((int_id == I386_EXCEPTION_NO_COPROCESSOR) || (int_id == I386_EXCEPTION_INVALID_OPCODE)) {
		if (!i386_fpu_handle_ill())
			exception_handle(i386_exception_table[int_id], (void *)instr_ptr, &registers, sizeof(i386_pusha_registers_t));
	} else {
		/* Handle regular exceptions */
		exception_handle(i386_exception_table[int_id], (void *)instr_ptr, &registers, sizeof(i386_pusha_registers_t));
	} 
	process_handle_signals();
}

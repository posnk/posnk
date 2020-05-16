/**
 * arch/i386/isr_entry.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 02-04-2014 - Created
 */

#ifndef __ARCH_I386_ISR_ENTRY_H__
#define __ARCH_I386_ISR_ENTRY_H__

#include <stdint.h>
#include "arch/i386/x86.h"

/**
 * The stack as seen by the kernel i386_handle_interrupt function
 */
struct i386_isr_stack {
    /**
     * The DS register value before this interrupt was taken
     */
	uint32_t				ds;

    /**
     * The general purpose register set as pushed by pusha after int entry
     */
	i386_pusha_registers_t	regs;

    /**
     * The interrupt vector number that was taken
     */
	uint32_t				int_id;

    /**
     * The error code pushed by the CPU, or a magic value if none was pushed
     */
	uint32_t				error_code;
    
    /**
     * The instruction pointer before taking the interrupt
     */
	uint32_t				eip;

    /**
     * The CS selector before taking the interrupt
     */
	uint32_t				cs;
    
    /**
     * The flags register before taking the interrupt
     */
	uint32_t				eflags;

    /**
     * The user stack pointer before taking the interrupt, only valid
     * if interrupt came from a higher ring.
     */
	uint32_t				esp;

    /**
     * The user stack segment before taking the interrupt, only valid
     * if interrupt came from a higher ring.
     */
	uint32_t				ss;
}  __attribute__((packed));

typedef struct i386_isr_stack i386_isr_stack_t;

void i386_kern_enter ( i386_isr_stack_t *stack );
void i386_user_enter ( i386_isr_stack_t *stack );
void i386_user_exit ( i386_isr_stack_t *stack );

void i386_fork_exit( void );

 void i386_isr_entry_0();
 void i386_isr_entry_1();
 void i386_isr_entry_3();
 void i386_isr_entry_4();
 void i386_isr_entry_5();
 void i386_isr_entry_6();
 void i386_isr_entry_7();
 void i386_isr_entry_8();
 void i386_isr_entry_9();
 void i386_isr_entry_10();
 void i386_isr_entry_11();
 void i386_isr_entry_12();
 void i386_isr_entry_13();
 void i386_isr_entry_14();
 void i386_isr_entry_16();
 void i386_isr_entry_32();
 void i386_isr_entry_33();
 void i386_isr_entry_34();
 void i386_isr_entry_35();
 void i386_isr_entry_36();
 void i386_isr_entry_37();
 void i386_isr_entry_38();
 void i386_isr_entry_39();
 void i386_isr_entry_40();
 void i386_isr_entry_41();
 void i386_isr_entry_42();
 void i386_isr_entry_43();
 void i386_isr_entry_44();
 void i386_isr_entry_45();
 void i386_isr_entry_46();
 void i386_isr_entry_47();
 void i386_isr_entry_80h();
 void i386_isr_entry_81h();
 
 #endif

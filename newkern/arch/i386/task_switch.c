/* 
 * arch/i386/task_switch.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 */
#include <string.h>
#include <stdint.h>
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "kernel/earlycon.h"
#include "arch/i386/isr_entry.h"
#include "arch/i386/task_context.h"
#include "arch/i386/protection.h"
#include "arch/i386/x86.h"

uint32_t i386_get_return_address ( void );

void i386_do_context_switch(	uint32_t esp,
								uint32_t ebp, 
								uint32_t eip, 
								physaddr_t page_dir);

void i386_kern_enter ( i386_isr_stack_t *stack )
{

	i386_task_context_t *tctx = scheduler_current_task->arch_state;

	if ( tctx == 0 )
		return;
	
	tctx->intr_regs		= stack->regs;
	tctx->intr_eip		= stack->eip;
	tctx->intr_cs		= stack->cs;
	tctx->intr_ds		= stack->ds;
	/* The ESP in the pusha structure is the ISR stack, not the user stack */
	tctx->intr_regs.esp = ( (uint32_t) stack ) + sizeof( i386_isr_stack_t ) - 8; 
  
}

void i386_user_enter ( i386_isr_stack_t *stack )
{

	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if ( tctx == 0 )
		return;
	tctx->intr_regs		= stack->regs;
	tctx->user_regs		= stack->regs;
	/* The ESP in the pusha structure is the ISR stack, not the user stack */
	tctx->intr_regs.esp = stack->esp;
	tctx->user_regs.esp = stack->esp;
	tctx->user_eip		= stack->eip;
	tctx->user_ss		= stack->ss;
	tctx->user_cs		= stack->cs;
	tctx->user_ds		= stack->ds;
 
}

void i386_user_exit ( i386_isr_stack_t *stack )
{

	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if ( tctx == 0 )
		return;
	/*This will corrupt the ESP value on the stack, but it is ignored by POPA*/
	stack->regs			= tctx->user_regs;
	/* Restore DS */
	stack->ds			= tctx->user_ds;

	/* Restore User Stacks */
	stack->eip			= tctx->user_eip;
	stack->esp			= tctx->user_regs.esp;
	stack->ss			= tctx->user_ss;
	stack->cs			= tctx->user_cs;
 
}

/**
 * Switch to new task
 * @param new_task	The target of the switch
 */
void scheduler_switch_task(scheduler_task_t *new_task)
{
	i386_task_context_t *tctx;
	i386_task_context_t *nctx;
	volatile uint32_t esp, ebp, eip;
	/* Disable interrupts */
	asm ("cli;");
	/* Store stack and base pointer */
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp)); 
	/* Store instruction pointer */
	eip = i386_get_return_address();
	/*-----------------SWITCH BOUNDARY---------------*/
	if (eip == 0xFFFDCAFE) {
		/* Back from switch */
		return;
	}
	tctx = scheduler_current_task->arch_state;
	nctx = new_task->arch_state;

	tctx->kern_eip = eip;
	tctx->kern_esp = esp;
	tctx->kern_ebp = ebp;
	/* Preset kernel state to new task */
	paging_active_dir = new_task->page_directory;	
	scheduler_current_task = new_task;
	/* Now its time for some magic */
	i386_fpu_on_cs();
	i386_do_context_switch(
				nctx->kern_esp,
				nctx->kern_ebp,
				nctx->kern_eip,
				paging_get_physical_address(
					new_task->page_directory->content)
				);
}

int scheduler_init_task(scheduler_task_t *new_task) {
	new_task->arch_state = heapmm_alloc(sizeof(i386_task_context_t));
	if (!new_task->arch_state)
		return ENOMEM;
	memset(new_task->arch_state, 0, sizeof(i386_task_context_t));
	return 0;
}

int scheduler_fork_to(scheduler_task_t *new_task)
{
	i386_task_context_t *tctx;
	i386_task_context_t *nctx;
	volatile uint32_t esp, ebp, eip;
	asm ("cli;");
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp)); 
	new_task->page_directory = paging_create_dir();
	eip = i386_get_return_address();
	if (eip == 0xFFFDCAFE) {
		/* Back from switch, we are the child */
		return 0;
	} 
	nctx = (i386_task_context_t *) new_task->arch_state;
	tctx = (i386_task_context_t *) scheduler_current_task->arch_state;

	i386_fpu_fork();

	tctx->kern_eip = eip;
	tctx->kern_esp = esp;
	tctx->kern_ebp = ebp;	

	memcpy(nctx, tctx, sizeof(i386_task_context_t));

	return 1;
}
void i386_cs_debug_attach(uint32_t esp, uint32_t ebp, uint32_t eip, physaddr_t page_dir);
void debug_attach_task(process_info_t *new_task)
{
	i386_task_context_t *nctx;
	nctx = new_task->arch_state;
	i386_cs_debug_attach(
				nctx->kern_esp,
				nctx->kern_ebp,
				nctx->kern_eip,
				paging_get_physical_address(new_task->page_directory->content));	
}



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

void i386_user_enter ( i386_pusha_registers_t regs, uint32_t ds )
{

	i386_isr_stack_t *isrstack = (i386_isr_stack_t *) regs.esp;
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if ( tctx == 0)
		return;
	tctx->user_regs		= regs;
	/** The ESP in the pusha structure is the ISR stack, not the user stack */
	tctx->user_regs.esp = isrstack->esp;
	tctx->user_eip		= isrstack->eip;
	tctx->user_ss		= isrstack->ss;
	tctx->user_cs		= isrstack->cs;
	tctx->user_ds		= ds;
 
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

	new_task->arch_state_pre_signal = heapmm_alloc(sizeof(i386_task_context_t));
	if (!new_task->arch_state_pre_signal)
		return ENOMEM;
	memset(new_task->arch_state_pre_signal, 0, 
			sizeof(i386_task_context_t));

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

	tctx = (i386_task_context_t *)
		scheduler_current_task->arch_state_pre_signal;
	nctx = (i386_task_context_t *) 
		new_task->arch_state_pre_signal;

	memcpy(nctx, tctx, sizeof(i386_task_context_t));

	return 1;
}

/**	
 * Push data onto process stack
 */
int process_push_user_data(void *data, size_t size)
{
	i386_task_context_t *tctx = (i386_task_context_t *)
			scheduler_current_task->arch_state;
	uintptr_t new_esp = (uintptr_t) tctx->user_regs.esp - size;
	if (!procvmm_check((void *) new_esp, size))
		return 0;
	memcpy((void *) new_esp, data, size);
	tctx->user_regs.esp = (uint32_t) new_esp;
	return 1;
}

/**
 * Jump into signal handler.
 */
int scheduler_invoke_signal_handler(int signal)
{
	i386_task_context_t *tctx;
	i386_task_context_t *sctx;
	volatile uint32_t esp, ebp, eip;
	asm ("cli;");
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp)); 
	eip = i386_get_return_address();
	if (eip == 0xFFFDCAFE) {
		/* Back from switch */
		debugcon_printf("Back from sighandler\n");
		return 1;
	}
	/* Havent switched yet */
	
	/* Store FPU state */
	i386_fpu_sigenter();

	/* Back up task state */
	tctx = scheduler_current_task->arch_state;
	sctx = scheduler_current_task->arch_state_pre_signal;
	memcpy(sctx, tctx, sizeof(i386_task_context_t));

	/* Store context switch state */
	sctx->kern_eip = eip;
	sctx->kern_esp = esp;
	sctx->kern_ebp = ebp;

	/* Create a fake call frame */
	debugcon_printf("Pushing signal number\n");
	if (!process_push_user_data(&(signal), 4))
		return 0;//Parameter: signal number 
	debugcon_printf("Pushing return addr\n");
	if (!process_push_user_data(&(scheduler_current_task->signal_handler_exit_func), 4))
		return 0;//Return address: sigreturn stub

	/* Set up user state */
	tctx->user_eip = (uint32_t)
						scheduler_current_task->signal_handler_table[signal];
	/* Now its time for some magic */
	debugcon_printf("Calling installed signal handler\n");

	i386_protection_user_call( tctx->user_eip, tctx->user_regs.esp );
	return 1;
}

void scheduler_exit_signal_handler()
{
	i386_task_context_t *tctx;
	i386_task_context_t *sctx;

	tctx = scheduler_current_task->arch_state;
	sctx = scheduler_current_task->arch_state_pre_signal;
	memcpy(tctx, sctx, sizeof(i386_task_context_t));

	i386_fpu_sigexit();
	debugcon_printf("sigexit switch\n");
	i386_do_context_switch( tctx->kern_esp, tctx->kern_ebp, tctx->kern_eip,
				paging_get_physical_address(scheduler_current_task->page_directory->content));	
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



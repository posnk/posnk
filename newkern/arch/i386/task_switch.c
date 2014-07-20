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
#include "arch/i386/task_context.h"
#include "arch/i386/protection.h"

uint32_t i386_get_return_address ( void );

void i386_do_context_switch(uint32_t esp, uint32_t ebp, uint32_t eip, physaddr_t page_dir);

void scheduler_switch_task(scheduler_task_t *new_task)
{
	volatile uint32_t esp, ebp, eip;
	asm ("cli;");
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp)); 
	eip = i386_get_return_address();
	if (eip == 0xFFFDCAFE) {
		/* Back from switch */
		process_handle_signals();
		return;
	}
	/* Havent switched yet */
	if (scheduler_current_task->arch_state == 0) {
		scheduler_current_task->arch_state = (i386_task_context_t *) heapmm_alloc(sizeof(i386_task_context_t));
		//earlycon_printf("arch_s:%x\n",scheduler_current_task->arch_state);
	}
	((i386_task_context_t *)scheduler_current_task->arch_state)->eip = eip;
	((i386_task_context_t *)scheduler_current_task->arch_state)->esp = esp;
	((i386_task_context_t *)scheduler_current_task->arch_state)->ebp = ebp;
	/* Preset kernel state to new task */
	paging_active_dir = new_task->page_directory;	
	scheduler_current_task = new_task;
	/* Now its time for some magic */
	i386_fpu_on_cs();
	i386_do_context_switch(((i386_task_context_t *)new_task->arch_state)->esp,
				((i386_task_context_t *)new_task->arch_state)->ebp,
				((i386_task_context_t *)new_task->arch_state)->eip,
				paging_get_physical_address(new_task->page_directory->content));
}

int process_push_user_data(void *data, size_t size)
{
	uintptr_t new_esp = (uintptr_t) (((i386_task_context_t *)scheduler_current_task->arch_state)->esp - size);
	if (!procvmm_check((void *) new_esp, size))
		return 0;
	memcpy((void *) new_esp, data, size);
	((i386_task_context_t *)scheduler_current_task->arch_state)->esp = (uint32_t) new_esp;
	return 1;
}

int scheduler_invoke_signal_handler(int signal)
{
	volatile uint32_t esp, ebp, eip;
	void *stack_copy;
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
	stack_copy = heapmm_alloc(0x4000);
	memcpy(stack_copy, (void *)0xBFFFD000, 0x4000);
	scheduler_current_task->isr_stack_pre_signal = stack_copy;
	scheduler_current_task->isr_stack_pre_signal_size = 0x4000;
	if (scheduler_current_task->arch_state == 0) {
		scheduler_current_task->arch_state = (i386_task_context_t *) heapmm_alloc(sizeof(i386_task_context_t));
		memset(scheduler_current_task->arch_state, 0, sizeof(i386_task_context_t));
	}
	if (scheduler_current_task->arch_state_pre_signal == 0) {
		scheduler_current_task->arch_state_pre_signal = (i386_task_context_t *) heapmm_alloc(sizeof(i386_task_context_t));
	}
	((i386_task_context_t *)scheduler_current_task->arch_state_pre_signal)->eip = eip;
	((i386_task_context_t *)scheduler_current_task->arch_state_pre_signal)->esp = esp;
	((i386_task_context_t *)scheduler_current_task->arch_state_pre_signal)->ebp = ebp;
	((i386_task_context_t *)scheduler_current_task->arch_state)->esp = 0xBFBFFFFF; //TODO: Use current user stack pointer
	debugcon_printf("Pushing signal number\n");
	if (!process_push_user_data(&(signal), 4))
		return 0;//Parameter: signal number 
	debugcon_printf("Pushing return addr\n");
	if (!process_push_user_data(&(scheduler_current_task->signal_handler_exit_func), 4))
		return 0;//Return address: sigreturn stub
	/* Now its time for some magic */
	debugcon_printf("Calling installed signal handler\n");
	i386_protection_user_call((uint32_t) scheduler_current_task->signal_handler_table[signal], ((i386_task_context_t *)scheduler_current_task->arch_state)->esp);
	return 1;
}

void scheduler_exsig(){
	memcpy((void *)0xBFFFD000, scheduler_current_task->isr_stack_pre_signal, 0x4000);
	heapmm_free(scheduler_current_task->isr_stack_pre_signal, scheduler_current_task->isr_stack_pre_signal_size);
	heapmm_free(scheduler_current_task->arch_state, sizeof(i386_task_context_t));
	scheduler_current_task->arch_state = scheduler_current_task->arch_state_pre_signal;
	scheduler_current_task->arch_state_pre_signal = 0;
	scheduler_current_task->isr_stack_pre_signal = 0;
	scheduler_current_task->isr_stack_pre_signal_size = 0;
	i386_fpu_on_cs();
	i386_do_context_switch(((i386_task_context_t *)scheduler_current_task->arch_state)->esp,
				((i386_task_context_t *)scheduler_current_task->arch_state)->ebp,
				((i386_task_context_t *)scheduler_current_task->arch_state)->eip,
				paging_get_physical_address(scheduler_current_task->page_directory->content));	
}
void i386_cs_debug_attach(uint32_t esp, uint32_t ebp, uint32_t eip, physaddr_t page_dir);
void debug_attach_task(process_info_t *new_task)
{
	i386_cs_debug_attach(((i386_task_context_t *)new_task->arch_state)->esp,
				((i386_task_context_t *)new_task->arch_state)->ebp,
				((i386_task_context_t *)new_task->arch_state)->eip,
				paging_get_physical_address(new_task->page_directory->content));	
}

void scheduler_exit_signal_handler()
{
	uintptr_t init_sp, new_sp;
	init_sp = new_sp = 0xBFFFC7FF;
	*((int *)init_sp) = 0;
	debugcon_printf("newstk:%x\n",init_sp);
	asm ("movl %%esp, %%eax; movl %1, %%esp; call scheduler_exsig;movl %%eax, %%esp;"
	     :"=r"(init_sp)        /* output */
	     :"r"(new_sp)         /* input */
	     :"%eax"         /* clobbered register */
	     );
}

int scheduler_fork_to(scheduler_task_t *new_task)
{
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
	new_task->arch_state = (i386_task_context_t *) heapmm_alloc(sizeof(i386_task_context_t));
	if (scheduler_current_task->arch_state == 0) {
		scheduler_current_task->arch_state = (i386_task_context_t *) heapmm_alloc(sizeof(i386_task_context_t));
		memset(scheduler_current_task->arch_state, 0, sizeof(i386_task_context_t));
		//earlycon_printf("arch_s:%x\n",scheduler_current_task->arch_state);
	}
	memset(new_task->arch_state, 0, sizeof(i386_task_context_t));
	((i386_task_context_t *)scheduler_current_task->arch_state)->eip = eip;
	((i386_task_context_t *)scheduler_current_task->arch_state)->esp = esp;
	((i386_task_context_t *)scheduler_current_task->arch_state)->ebp = ebp;
	((i386_task_context_t *)new_task->arch_state)->eip = eip;
	((i386_task_context_t *)new_task->arch_state)->esp = esp;
	((i386_task_context_t *)new_task->arch_state)->ebp = ebp;
	return 1;
}



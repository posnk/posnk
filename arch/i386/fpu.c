/**
 * arch/i386/fpu.c
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
#include "arch/i386/task_context.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/earlycon.h"
#include "util/debug.h"

uint8_t i386_fpu_cs_buf[512] __attribute__((aligned(16)));

scheduler_task_t *i386_fpu_thread = NULL;

int i386_fpu_enabled = 0;

void i386_fpu_initialize()
{
	i386_fpu_enabled = 1;
}

void i386_enable_fpu()
{
	uint32_t temp;
	asm volatile("clts;mov %%cr4, %0" : "=r"(temp));
	temp |= 3 << 9;
	asm volatile("mov %0, %%cr4" :: "r"(temp));
}

void i386_disable_fpu()
{
	uint32_t temp;
	asm volatile("mov %%cr0, %0" : "=r"(temp));
	temp |= 1 << 3;
	asm volatile("mov %0, %%cr0" :: "r"(temp));
}

void i386_init_fpu()
{
	uint32_t c = 0x37F;
	asm volatile("fninit; fldcw %0":: "m"(c));
}

void i386_fpu_save(i386_task_context_t *context)
{
	asm volatile("fxsave %0" : "=m" (i386_fpu_cs_buf));
	memcpy(context->fpu_state, i386_fpu_cs_buf, 512);
}

void i386_fpu_load(i386_task_context_t *context)
{
	memcpy(i386_fpu_cs_buf, context->fpu_state, 512);
	asm volatile("fxrstor %0" : "=m" (i386_fpu_cs_buf));
}

void i386_fpu_on_cs()
{
	if (i386_fpu_enabled)
		i386_disable_fpu();
}

void i386_fpu_sigenter()
{
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if (!i386_fpu_enabled)
		return;
	if (i386_fpu_thread != scheduler_current_task )
		return;
	i386_fpu_save(tctx);
}

void i386_fpu_fork()
{
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if (!i386_fpu_enabled)
		return;
	if (i386_fpu_thread != scheduler_current_task)
		return;
	i386_fpu_save(tctx);
}

void i386_fpu_sigexit()
{
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	if (!i386_fpu_enabled)
		return;
	if (i386_fpu_thread != scheduler_current_task)
		return;
	i386_fpu_load(tctx);
}

void i386_fpu_del_task(process_info_t *task) {
	if ( task == i386_fpu_thread )
		i386_fpu_thread=NULL;
}

int i386_fpu_handle_ill()
{
	if(!i386_fpu_enabled)
		return 0;
	i386_enable_fpu();
	if (i386_fpu_thread == scheduler_current_task)
		return 1;
	if (i386_fpu_thread) {
		i386_fpu_save((i386_task_context_t *)i386_fpu_thread->arch_state);
	}
	i386_fpu_thread = scheduler_current_task;
	if (!((i386_task_context_t *)i386_fpu_thread->arch_state)->fpu_used) {
		i386_init_fpu();
		((i386_task_context_t *)i386_fpu_thread->arch_state)->fpu_used = 1;
		return 1;		
	}
	i386_fpu_load((i386_task_context_t *)i386_fpu_thread->arch_state);
	return 1;
}

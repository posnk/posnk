#include "arch/i386/protection.h"
#include "arch/i386/task_context.h"
#include "kernel/scheduler.h"
#include "kernel/earlycon.h"
#include <string.h>

/**
 * This symbol directly references the GDT TSS descriptor
 */
extern i386_gdt_descriptor_t i386_tss_descriptor;

/**
 * The actual task state segment
 */
i386_tss_entry_t i386_tss_entry;

/**
 * Updates the global Task State Selector with new values
 * @param base  The base of the first TSS entry
 * @param limit The limit of the first TSS entry
 * @param flags The descriptor flags
 * @param gran  The granularity flags
 */
void i386_set_tss_descriptor(
	uint32_t base, uint32_t limit, uint8_t flags, uint8_t gran ){
	memset(&i386_tss_descriptor,0,sizeof(i386_gdt_descriptor_t));
	i386_tss_descriptor.baseLo
		= (base >> I386_GDT_BASE_LOW_SHIFT)	& I386_GDT_BASE_LOW_MASK;
	i386_tss_descriptor.baseMid
		= (base >> I386_GDT_BASE_MID_SHIFT)	& I386_GDT_BASE_MID_MASK;
	i386_tss_descriptor.baseHi
		= (base >> I386_GDT_BASE_HIGH_SHIFT) & I386_GDT_BASE_HIGH_MASK;
	i386_tss_descriptor.limit
		= (limit >> I386_GDT_LIMIT_LOW_SHIFT) & I386_GDT_LIMIT_LOW_MASK;
	i386_tss_descriptor.granularity  = gran;
	i386_tss_descriptor.granularity |=
		((limit >> I386_GDT_LIMIT_HIGH_SHIFT) & I386_GDT_LIMIT_HIGH_MASK);
	i386_tss_descriptor.flags		= flags;
}

/**
 * Reloads the task register so the CPU will re-read the TSS
 * Implemented as a pure assembly function
 */
void i386_tss_flush();

/**
 * Initializes the CPU side privilege model
 */
void i386_protection_init()
{
	uint32_t base, limit;

	/* Compute the base and limit values for the TSS entry */
	base = (uint32_t) &i386_tss_entry;
	limit = base + sizeof(i386_tss_entry_t);

	/* Load the TSS descriptor into the GDT */
	i386_set_tss_descriptor(base, limit, 0x89, 0x00);

	/* Zero out the new TSS entry */
	memset(&i386_tss_entry, 0, sizeof(i386_tss_entry_t));

	/* And setup the values, we do not use hardware task switching so only the
	 * SS0, ESP0, CS and SS values are required. These are used by the CPU to
	 * load the ring 0 stack segment and pointer. */
	i386_tss_entry.ss0	= 0x20;
	i386_tss_entry.esp0	= 0xBFFFDFFF;

	//TODO: Are these actually needed?
	i386_tss_entry.cs	= 0x28 | I386_RPL_USERMODE_MASK;
	i386_tss_entry.ss	=
		i386_tss_entry.ds =
		i386_tss_entry.es =
		i386_tss_entry.fs =
		i386_tss_entry.gs = 0x30 | I386_RPL_USERMODE_MASK;

	/* Have the processor load the new TSS values */
	i386_tss_flush();
}

/**
 * Update the processor TSS to reference the correct ring 0 stack for
 * the current task. This should be called every time the kernel transitions to
 * a less privileged ring after a task switch.
 */
void i386_tss_update( void )
{
	i386_task_context_t *tctx;
	tctx = (i386_task_context_t *) scheduler_current_task->arch_state;
	i386_tss_entry.esp0	= tctx->tss_esp;
}

/**
 * Invoke code at ring 3 privilege level.
 * @param entry The address of the instructions to execute
 * @param stack The initial stack pointer used to execute the code.
 */
void process_user_call(void *entry, void *stack)
{
	disable();
	i386_tss_update();
	i386_protection_user_call((uint32_t) entry, (uint32_t) stack);
}

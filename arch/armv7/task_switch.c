/*
 * arch/armv7/task_switch.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 19-03-2015 - Created
 */
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "kernel/earlycon.h"
#include "arch/armv7/cswitch.h"
#include "arch/armv7/taskctx.h"
#include "arch/armv7/cpu.h"
#include "arch/armv7/exception.h"

typedef struct {uint32_t entries[10];} taskstate;

/**
 * Switch to new task
 * @param new_task	The target of the switch
 */
void scheduler_switch_task( scheduler_task_t *new_task )
{
	int s;
	armv7_task_context_t *tctx;
	armv7_task_context_t *nctx;

	/* Stop interrupts, we are going to do critical stuff here */
	/* Said cricical stuff will only be CPU local so this solution should work*/
	s = disable();

	tctx = scheduler_current_task->arch_state;
	nctx = new_task->arch_state;

	/* If we want to switch to the current task, NOP */
	if ( new_task != scheduler_current_task ) {

		if ( new_task->process && (TASK_GLOBAL & ~new_task->flags) ) {

			/* Switch page tables */
			paging_switch_dir( new_task->process->page_directory );
		}

		/* Update task pointer */
		scheduler_current_task = new_task;

		/* Switch kernel threads */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
		armv7_context_switch( nctx->kern_sp, &tctx->kern_sp );
#pragma GCC diagnostic pop

	}

	/* Restore interrupt flag */
	restore( s );
}

void process_load_exec_state( void *entry, void *stack ) {

	armv7_task_context_t *tctx = scheduler_current_task->arch_state;

	assert( tctx != NULL );

	memset( tctx->user_regs, 0, sizeof tctx->user_regs );
	tctx->user_sp       = (uint32_t) stack;
	tctx->user_pc       = (uint32_t) entry;
	tctx->user_sr       = PSR_MODE_USR;
	tctx->user_lr       = 0;
}


/**
 * Initialize arch state for the task
 */
int scheduler_init_task(scheduler_task_t *new_task) {

	/* Allocate CPU state struct */
	new_task->arch_state = heapmm_alloc(sizeof(armv7_task_context_t));
	if (!new_task->arch_state)
		return ENOMEM;

	/* Clear it */
	memset(new_task->arch_state, 0, sizeof(armv7_task_context_t));

	return 0;
}

size_t scheduler_get_state_size( ) {
	return sizeof(armv7_task_context_t);
}

int disable() {
	int s;
	s = armv7_get_mode() & (PSR_IRQ_MASK | PSR_FIQ_MASK);
	armv7_disable_ints();
	return s == 0;
}

int enable() {
	int s;
	s = armv7_get_mode() & (PSR_IRQ_MASK | PSR_FIQ_MASK);
	armv7_disable_ints();
	return s == 0;
}

void restore(int s) {
	if ( s )
		armv7_enable_ints();
	else
		armv7_disable_ints();
}

/**
 * Allocate a new kernel stack
 */
int scheduler_alloc_kstack(scheduler_task_t *task)
{
	physaddr_t frame;
	void *guard;

	/* Allocate the stack from the kheap */
	task->kernel_stack = heapmm_alloc_alligned
		( CONFIG_KERNEL_STACK_SIZE + PHYSMM_PAGE_SIZE * 2
		, PHYSMM_PAGE_SIZE );
	if ( !task->kernel_stack )
		return -1;

	/* Unmap and release the lowest frame of the stack */
	/* to guard against stack overflow */
	guard = task->kernel_stack;
	frame = paging_get_physical_address( guard );
	printf( CON_TRACE, "Setting up stack overflow guard: %x to %x",
	        guard, guard + PHYSMM_PAGE_SIZE );
	paging_unmap( guard );
	physmm_free_frame( frame );

	guard = task->kernel_stack + CONFIG_KERNEL_STACK_SIZE + PHYSMM_PAGE_SIZE;
	frame = paging_get_physical_address( guard );
	printf( CON_TRACE, "Setting up stack underflow guard: %x to %x",
	        guard, guard + PHYSMM_PAGE_SIZE );
	paging_unmap( guard );
	physmm_free_frame( frame );

	return 0;
}

/**
 * Free the kernel stack used by a task
 */
int scheduler_free_kstack(scheduler_task_t *task)
{
	physaddr_t frame;

	/* Allocate a frame to restore the guard area */
	frame = physmm_alloc_frame();
	if ( frame == PHYSMM_NO_FRAME )
		return -1;

	/* Map the guard area back into RAM */
	paging_map( task->kernel_stack, frame, PAGING_PAGE_FLAG_RW );

	/* Allocate a frame to restore the guard area */
	frame = physmm_alloc_frame();
	if ( frame == PHYSMM_NO_FRAME )
		return -1;

	/* Map the guard area back into RAM */
	paging_map(
		task->kernel_stack + CONFIG_KERNEL_STACK_SIZE + PHYSMM_PAGE_SIZE
		,frame, PAGING_PAGE_FLAG_RW );

	/* Free the heap space used by the stack */
	heapmm_free( task->kernel_stack,
				CONFIG_KERNEL_STACK_SIZE + 2 * PHYSMM_PAGE_SIZE);
	return 0;
}

/**
 * Free the architecture specific parts of a task
 */
int scheduler_free_task(scheduler_task_t *new_task) {

	/* Free the architecture specific state */
	heapmm_free(new_task->arch_state,sizeof(armv7_task_context_t));

	/* Release its stack */
	if ( new_task->kernel_stack )
		return scheduler_free_kstack( new_task );

	return 0;
}

/**
 * Spawn a new task
 */
int scheduler_do_spawn( scheduler_task_t *new_task, void *callee, void *arg, int s )
{
	armv7_task_context_t *tctx;
	armv7_task_context_t *nctx;
	int s2;
	armv7_cswitch_stack_t *nstate;

	/* Allocate its kernel stack */
	if ( scheduler_alloc_kstack( new_task ) )
		return ENOMEM;

	/* Disable interrupts and get interrupt flag */
	s2 = disable();

	nctx = new_task->arch_state;
	tctx = scheduler_current_task->arch_state;

	/* Copy process state for userland fork */
	memcpy( nctx, tctx, sizeof( armv7_task_context_t ) );

	/* Set up kernel state */
	nctx->kern_sp = ( uint32_t ) new_task->kernel_stack;
	nctx->kern_sp += CONFIG_KERNEL_STACK_SIZE + PHYSMM_PAGE_SIZE;

	/* Push the new task state */
	nctx->kern_sp -= sizeof( armv7_cswitch_stack_t );
	nstate = ( armv7_cswitch_stack_t * ) nctx->kern_sp;
	memset( nstate, 0, sizeof( armv7_cswitch_stack_t ) );

	/* Set the return address */
	/* Creates a fake user-to-kernel interrupt entry that solely invokes
	 * i386_user_exit before using iret to return to ring 3 in the newly
	 * created process.
	 *
	 * This is done to simplify logic elsewhere in the kernel: because of
	 * this code, all kernel<>user mode code can assume there is an
	 * interrupt entry structure at top of stack.
	 */
	nstate->lr = (uint32_t) armv7_spawnentry;

	/* Load the arguments for the entry shim */
	nstate->regs[0] = (uint32_t) callee; /* R4 */
	nstate->regs[1] = (uint32_t) arg;    /* R5 */
	nstate->regs[2] = (uint32_t) s;      /* R6 */

	restore(s2);

	/* Switch to new process ? */

	return 0;

}

void debug_attach_task(process_info_t *new_task)
{

	//TODO: Implement debugger attachment


}


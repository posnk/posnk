/*
 * arch/i386/task_switch.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 * 27-08-2017 - Re-implemented context switching
 */
#include <string.h>
#include <stdint.h>
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#define CON_SRC ("i386_process")
#include "kernel/console.h"
#include "arch/i386/isr_entry.h"
#include "arch/i386/task_context.h"
#include "arch/i386/protection.h"
#include "arch/i386/x86.h"
#include <assert.h>


/**
 * Called from interrupt handlers to update the kernel state
 * This function records the following register values:
 *	PUSHA set, EIP, CS, DS, ESP
 */
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

/**
 * Called from interrupt handlers to update the user state
 * This function records the following register values:
 *	PUSHA set, EIP, CS, DS, ESP, SS, EFLAGS
 */
void i386_user_enter ( i386_isr_stack_t *stack )
{

	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	assert( tctx != NULL );

	tctx->intr_regs		= stack->regs;
	/* The ESP in the pusha structure is the ISR stack, not the user stack */
	tctx->intr_regs.esp = stack->esp;

	tctx->user_regs		= stack->regs;
	tctx->user_regs.esp     = stack->esp;
	tctx->user_eip		= stack->eip;
	tctx->user_ss		= stack->ss;
	tctx->user_cs		= stack->cs;
	tctx->user_ds		= stack->ds;
	tctx->user_eflags	= stack->eflags;

}


void process_load_exec_state( void *entry, void *stack ) {

	i386_task_context_t *tctx = scheduler_current_task->arch_state;

	assert( tctx != NULL );

	tctx->user_regs.eax = 0;
	tctx->user_regs.edx = 0;
	tctx->user_regs.ecx = 0;
	tctx->user_regs.ebx = 0;
	tctx->user_regs.esi = 0;
	tctx->user_regs.edi = 0;
	tctx->user_regs.ebp = 0;
	tctx->user_regs.esp = (uint32_t) stack;
	tctx->user_eip      = (uint32_t) entry;
	tctx->user_ds       = 0x33;
	tctx->user_ss       = 0x33;
	tctx->user_cs       = 0x2B;
	tctx->user_eflags   = (1<<9);//IF Set
}

void scheduler_get_mcontext( const scheduler_task_t *_task,
                              mcontext_t *ctx )
{

	i386_task_context_t *tctx;

	assert( _task != NULL );
	tctx = _task->arch_state;
	assert( tctx != NULL );

	ctx->reg_eflags = tctx->user_eflags;
	ctx->reg_cs     = tctx->user_cs;
	ctx->reg_eip    = tctx->user_eip;
	ctx->reg_ss     = tctx->user_ss;
	ctx->reg_esp    = tctx->user_regs.esp;
	ctx->reg_ebp    = tctx->user_regs.ebp;
	ctx->reg_ds     = tctx->user_ds;
	ctx->reg_esi    = tctx->user_regs.esi;
	ctx->reg_edi    = tctx->user_regs.edi;
	ctx->reg_eax    = tctx->user_regs.eax;
	ctx->reg_ebx    = tctx->user_regs.ebx;
	ctx->reg_ecx    = tctx->user_regs.ecx;
	ctx->reg_edx    = tctx->user_regs.edx;
	ctx->using_fpu  = tctx->fpu_used;
	//TODO: Handle lazy FPU here
	memcpy( ctx->reg_xsave, tctx->fpu_state, sizeof ctx->reg_xsave );
}

void scheduler_set_mcontext( scheduler_task_t *_task,
                             const mcontext_t *ctx )
{

	i386_task_context_t *tctx;

	assert( _task != NULL );
	tctx = _task->arch_state;
	assert( tctx != NULL );

	tctx->user_eflags   = ctx->reg_eflags;
	tctx->user_cs       = ctx->reg_cs;
	tctx->user_eip      = ctx->reg_eip;
	tctx->user_ss       = ctx->reg_ss;
	tctx->user_regs.esp = ctx->reg_esp;
	tctx->user_regs.ebp = ctx->reg_ebp;
	tctx->user_ds       = ctx->reg_ds;
	tctx->user_regs.esi = ctx->reg_esi;
	tctx->user_regs.edi = ctx->reg_edi;
	tctx->user_regs.eax = ctx->reg_eax;
	tctx->user_regs.ebx = ctx->reg_ebx;
	tctx->user_regs.ecx = ctx->reg_ecx;
	tctx->user_regs.edx = ctx->reg_edx;
	tctx->fpu_used    = ctx->using_fpu;
	//TODO: Handle lazy FPU here
	memcpy( tctx->fpu_state, ctx->reg_xsave, sizeof ctx->reg_xsave );

}

/**
 * This function is called before returning to user mode
 * It will update the state information on the stack from the thread state.
 */
void i386_user_exit ( i386_isr_stack_t *stack )
{

	i386_task_context_t *tctx = scheduler_current_task->arch_state;

	if ( tctx == 0 ) {
		printf(CON_ERROR, "exit from kernel without tctx");
		return;
	}

	/*This will corrupt the ESP value on the stack, but it is ignored by POPA*/
	stack->regs			= tctx->user_regs;
	/* Restore DS */
	stack->ds			= tctx->user_ds;

	/* Restore User Stacks */
	stack->eip			= tctx->user_eip;
	stack->esp			= tctx->user_regs.esp;
	stack->ss			= tctx->user_ss;
	stack->cs			= tctx->user_cs;
	stack->eflags		        = tctx->user_eflags;

	/* Update the TSS to point to the correct supervisor stack for this task */
 	i386_tss_update();

	assert ( ( stack->cs & 3 ) == 3 );
	assert ( stack->eip < (unsigned) 0xc0000000 );
}

void i386_fpu_del_task(scheduler_task_t *task);

/**
 * Switch to new task
 * @param new_task	The target of the switch
 */
void scheduler_switch_task( scheduler_task_t *new_task )
{
	int s;
	i386_task_context_t *tctx;
	i386_task_context_t *nctx;

	/* Stop interrupts, we are going to do critical stuff here */
	/* Said cricical stuff will only be CPU local so this solution should work*/
	s = disable();

	tctx = scheduler_current_task->arch_state;
	nctx = new_task->arch_state;

	/* If we want to switch to the current task, NOP */
	if ( new_task != scheduler_current_task ) {

		//csstack_t *ess = (void*)(nctx->kern_esp);

		if ( new_task->process && (TASK_GLOBAL & ~new_task->flags) ) {

			/* Switch page tables */
			paging_switch_dir( new_task->process->page_directory );
		}

		/* Update task pointer */
		scheduler_current_task = new_task;

		/* Flag context switch to the FPU */
		i386_fpu_on_cs();

		/* Switch kernel threads */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
		i386_context_switch( nctx->kern_esp, &tctx->kern_esp );
#pragma GCC diagnostic pop

	}

	/* Restore interrupt flag */
	restore( s );
}

/**
 * Initialize arch state for the task
 */
int scheduler_init_task(scheduler_task_t *new_task) {

	/* Allocate CPU state struct */
	new_task->arch_state = heapmm_alloc(sizeof(i386_task_context_t));
	if (!new_task->arch_state)
		return ENOMEM;

	/* Clear it */
	memset(new_task->arch_state, 0, sizeof(i386_task_context_t));

	return 0;
}

size_t scheduler_get_state_size( ) {
	return sizeof(i386_task_context_t);
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

	/* Notify the lazy FPU code about the tasks EOL */
	i386_fpu_del_task( new_task );

	/* Free the architecture specific state */
	heapmm_free(new_task->arch_state,sizeof(i386_task_context_t));

	/* Release its stack */
	if ( new_task->kernel_stack )
		return scheduler_free_kstack( new_task );

	return 0;
}

static void push_32( uint32_t *ptr, uint32_t val ) {
	*ptr -= 4;
	*((uint32_t *)*ptr) = ( uint32_t ) val;
}

/**
 * Spawn a new task
 */
int scheduler_do_spawn( scheduler_task_t *new_task, void *callee, void *arg, int s )
{
	i386_task_context_t *tctx;
	i386_task_context_t *nctx;
	int s2;
	struct csstack *nstate;

	/* Allocate its kernel stack */
	if ( scheduler_alloc_kstack( new_task ) )
		return ENOMEM;

	/* Disable interrupts and get interrupt flag */
	s2 = disable();

	nctx = (i386_task_context_t *) new_task->arch_state;
	tctx = (i386_task_context_t *) scheduler_current_task->arch_state;

	/* Handle FPU lazy switching */
	i386_fpu_fork();

	/* Copy process state for userland fork */
	memcpy( nctx, tctx, sizeof( i386_task_context_t ) );

	/* Set up kernel state */
	nctx->kern_esp = ( uint32_t ) new_task->kernel_stack;
	nctx->kern_esp += CONFIG_KERNEL_STACK_SIZE + PHYSMM_PAGE_SIZE;
	nctx->tss_esp  = nctx->kern_esp;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

	/* Push the arguments for the entry shim */
	push_32( &nctx->kern_esp, (uint32_t) s );

	/* Push the arguments for the entry point */
	push_32( &nctx->kern_esp, (uint32_t) arg );

	/* Push the arguments for the entry shim */
	push_32( &nctx->kern_esp, (uint32_t) callee );

	/* Push the return address */
	/* Creates a fake user-to-kernel interrupt entry that solely invokes
	 * i386_user_exit before using iret to return to ring 3 in the newly
	 * created process.
	 *
	 * This is done to simplify logic elsewhere in the kernel: because of
	 * this code, all kernel<>user mode code can assume there is an
	 * interrupt entry structure at top of stack.
	 */
	push_32( &nctx->kern_esp, (uint32_t) i386_fork_exit );

#pragma GCC diagnostic pop

	/* Push the new task state */
	nctx->kern_esp -= sizeof( csstack_t );
	nstate = ( struct csstack * ) nctx->kern_esp;
	memset( nstate, 0, sizeof( csstack_t ) );

	nstate->eip = ( uint32_t ) &scheduler_spawnentry;
	nstate->regs.ebp = 0xCAFE57AC;

	restore(s2);

	/* Switch to new process ? */

	return 0;

}

/**
 * Kernel task debugger attachment routine
 * This function splices the current call stack onto the tasks stack
 */
void i386_cs_debug_attach(uint32_t esp, uint32_t ebp, uint32_t eip, physaddr_t page_dir);
void debug_attach_task(process_info_t *new_task)
{
	i386_task_context_t *nctx;
	scheduler_task_t *thetask = (void*) new_task->tasks.next;
	nctx = thetask->arch_state;
	assert(!"IMPL");
	i386_cs_debug_attach(
				nctx->user_regs.esp,
				nctx->user_regs.ebp,
				nctx->user_eip,
				paging_get_physical_address(new_task->page_directory->content));
	//TODO: Implement
}



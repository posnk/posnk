/* 
 * arch/i386/signal.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-05-2017 - Split off from task_switch.c
 */
#include <string.h>
#include <stdint.h>
#include "kernel/heapmm.h"
#include "kernel/paging.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "kernel/earlycon.h"
#include "kernel/signals.h"
#include "arch/i386/isr_entry.h"
#include "arch/i386/task_context.h"
#include "arch/i386/protection.h"
#include "arch/i386/x86.h"

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

typedef struct {
	i386_task_context_t	ctx;
	sigrestore_info_t	restore;
} i386_sigrest_t;

/**
 * Jump into signal handler.
 * TODO: Use stack for signal exit state
 * Push sigexit block
 * Push ptr to sigexit block
 * Push args
 */
int invoke_signal_handler( int signal, siginfo_t *info, void *context )
{
	process_info_t *ctask;
	i386_task_context_t *tctx;
	i386_sigrest_t restore;
	struct sigaction act;
	stack_t *altstack;
	void *sigret, *usrinfo;

	ctask = scheduler_current_task;
	act = ctask->signal_actions[signal];
	altstack = &ctask->signal_altstack;
	
	/* Store FPU state */
	i386_fpu_sigenter();

	/* Back up task state */
	tctx = ctask->arch_state;
	restore.ctx = *tctx;

	/* Set up restore block */
	restore.restore.sr_sig  = signal;
	restore.restore.sr_mask = ctask->signal_mask;
	restore.restore.sr_flags = 0;

	/* If not using siginfo, do not actually push the info */
	if ( !( act.sa_flags & SA_SIGINFO ) ) {
		info	= NULL;
		context	= NULL;
	}

	/* If SA_NODEFER is not set, add current signal to mask */
	if ( !( act.sa_flags & ( SA_RESETHAND | SA_NODEFER ) ) ) 
		sigaddset( &ctask->signal_mask, signal );

	/* If SA_RESETHAND is set, reset the handler to SIG_DFL */
	if ( act.sa_flags & SA_RESETHAND )
		ctask->signal_actions[signal].sa_handler = SIG_DFL;

	/* Apply sa_mask */
	ctask->signal_mask |= act.sa_mask;

	/* If SA_ONSTACK is set and there is an available alternate stack, use it*/
	if ( ( act.sa_flags & SA_ONSTACK ) && 
		 !( altstack->ss_flags & ( SS_DISABLE | SS_ONSTACK ) ) ) {
		
		/* Start stack at the altstack top */
		tctx->user_regs.esp = ( uint32_t ) altstack->ss_sp + altstack->ss_size;
	
		/* Mark altstack as being used */
		altstack->ss_flags |= SS_ONSTACK;

		/* Mark altstack usage in restore block */
		restore.restore.sr_flags |= SA_ONSTACK;

	}

	/* Create a fake call frame */
	debugcon_printf("Pushing signal number\n");

	if (!process_push_user_data( info, sizeof( struct siginfo ) ) )
		return 0;

	/* Get start address of siginfo */
	usrinfo = ( void * ) tctx->user_regs.esp;

	if (!process_push_user_data(&restore, sizeof( i386_sigrest_t ) ) )
		return 0;

	/* Get start address of sigret */
	sigret = ( void * ) tctx->user_regs.esp;

	if (!process_push_user_data(&sigret, 4))
		return 0;//Parameter: sigret
	if (!process_push_user_data(&sigret, 4))
		return 0;//Parameter: dummy ( to patch shift by ret )
	if (!process_push_user_data(&context, 4)) 
		return 0;//Parameter: context
	if (!process_push_user_data(&usrinfo, 4))
		return 0;//Parameter: info
	
	if (!process_push_user_data(&signal, 4))
		return 0;//Parameter: signal number 

	debugcon_printf("Pushing return addr\n");
	if (!process_push_user_data(&(ctask->signal_handler_exit_func), 4))
		return 0;//Return address: sigreturn stub

	/* Set up user state */
	if ( act.sa_flags & SA_SIGINFO )
		tctx->user_eip = (uint32_t) act.sa_sigaction;
	else
	
		tctx->user_eip = (uint32_t) act.sa_handler;

	return 1;
}

void exit_signal_handler( void *ctx )
{
	i386_sigrest_t *sres;
	i386_task_context_t *tctx;
	i386_task_context_t *sctx;

	sres = ctx;

	sctx = &sres->ctx;
	tctx = scheduler_current_task->arch_state;

	/* Restore signal mask */
	scheduler_current_task->signal_mask = sres->restore.sr_mask;

	/* If we were the outermost handler on the alternative stack, release it */
	if ( sres->restore.sr_flags & SA_ONSTACK )
		scheduler_current_task->signal_altstack.ss_flags &= ~SS_ONSTACK;

	/* Do not trust userland to edit kernel stack pointer and instr pointer */
	sctx->kern_eip = tctx->kern_eip;
	sctx->kern_esp = tctx->kern_esp;
	sctx->kern_ebp = tctx->kern_ebp;

	/* Do not trust userland to hand correct segments to prevent priv escal */
	sctx->user_cs = tctx->user_cs;
	sctx->user_ds = tctx->user_ds;
	sctx->user_ss = tctx->user_ss;

	memcpy(tctx, sctx, sizeof(i386_task_context_t));

	i386_fpu_sigexit();

}

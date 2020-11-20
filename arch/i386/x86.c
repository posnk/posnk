#include "arch/i386/x86.h"
#include "arch/i386/task_context.h"
#define CON_SRC ("i386_process")
#include "kernel/console.h"
#include "kernel/scheduler.h"
#include "kdbg/dbgapi.h"
#include <stddef.h>

uint8_t i386_inb(uint16_t port)
{
	unsigned char ret;
	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
}

void i386_outb(uint16_t port,uint8_t value)
{
	asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

uint16_t i386_inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %%dx,%%ax":"=a" (ret):"d" (port));
	return ret;
}

void i386_outw(uint16_t port,uint16_t value)
{
	asm volatile ("outw %%ax,%%dx": :"d" (port), "a" (value));
}

uint32_t i386_inl(uint16_t port)
{
	uint32_t ret;
	asm volatile ("inl %%dx,%%eax":"=a" (ret):"d" (port));
	return ret;
}

void i386_outl(uint16_t port,uint32_t value)
{
	asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}

void halt()
{
	__asm__("cli;hlt");
}

void wait_int()
{
	__asm__("sti;hlt");
}


void debug_dump_state()
{
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	i386_pusha_registers_t *regs = (i386_pusha_registers_t *) &tctx->user_regs;
	printf(CON_ERROR, " User Registers: EIP: 0x%x\n",tctx->user_eip);
	printf(CON_ERROR, "EAX: 0x%X EBX: 0x%X ECX: 0x%X EDX: 0x%X",
		regs->eax, regs->ebx, regs->ecx, regs->edx);
	printf(CON_ERROR, "ESP: 0x%X EBP: 0x%X ESI: 0x%X EDI: 0x%X",
		regs->esp, regs->ebp, regs->esi, regs->edi);
	printf(CON_ERROR, " Interrupt Registers: EIP: 0x%x\n",tctx->intr_eip);
	regs = (i386_pusha_registers_t *) &tctx->intr_regs;
	printf(CON_ERROR, "EAX: 0x%X EBX: 0x%X ECX: 0x%X EDX: 0x%X",
		regs->eax, regs->ebx, regs->ecx, regs->edx);
	printf(CON_ERROR, "ESP: 0x%X EBP: 0x%X ESI: 0x%X EDI: 0x%X",
		regs->esp, regs->ebp, regs->esi, regs->edi);

}

void debug_postmortem_hook()
{
	i386_task_context_t *tctx = scheduler_current_task->arch_state;
	uint32_t eip = (uint32_t) tctx->intr_eip;
	i386_pusha_registers_t *regs = (i386_pusha_registers_t *) &tctx->intr_regs;
	asm("movl %0, %%esp;push %1;push %2;mov %%esp, %%ebp;cli;push $1;call dbgapi_invoke_kdbg"::"r"(regs->esp), "r"(eip), "r"(regs->ebp));

	dbgapi_invoke_kdbg(1);

}

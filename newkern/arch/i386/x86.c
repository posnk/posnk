#include "arch/i386/x86.h"
#include "kernel/earlycon.h"
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

void debug_dump_state(void *state, size_t state_size)
{	
	i386_pusha_registers_t *regs = (i386_pusha_registers_t *) state;
	if (state_size != sizeof(i386_pusha_registers_t)) {
		earlycon_puts("CORRUPT STATE STRUCT\n");
		return;
	}
	earlycon_printf("EAX: 0x%X EBX: 0x%X ECX: 0x%X EDX: 0x%X\n",
		regs->eax, regs->ebx, regs->ecx, regs->edx);
	earlycon_printf("ESP: 0x%X EBP: 0x%X ESI: 0x%X EDI: 0x%X\n",
		regs->esp, regs->ebp, regs->esi, regs->edi);
		
}

void debug_postmortem_hook(void *state, size_t state_size, void *instr_addr)
{
	uint32_t eip = (uint32_t) instr_addr;
	i386_pusha_registers_t *regs = (i386_pusha_registers_t *) state;
	if (state_size != sizeof(i386_pusha_registers_t)) {
		earlycon_puts("CORRUPT STATE STRUCT\n");
		return;
	}
	asm("movl %0, %%esp;push %1;push %2;mov %%esp, %%ebp;cli;push $1;call dbgapi_invoke_kdbg"::"r"(regs->esp), "r"(eip), "r"(regs->ebp));
	
	dbgapi_invoke_kdbg(1);
	
}

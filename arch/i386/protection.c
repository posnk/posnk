#include "arch/i386/protection.h"
#include "arch/i386/task_context.h"
#include "kernel/scheduler.h"
#include "kernel/earlycon.h"
#include <string.h>

extern i386_gdt_descriptor_t i386_tss_descriptor;

i386_tss_entry_t i386_tss_entry;

void i386_set_tss_descriptor(uint32_t base, uint32_t limit, uint8_t flags,uint8_t granularity){
	memset(&i386_tss_descriptor,0,sizeof(i386_gdt_descriptor_t));
	i386_tss_descriptor.baseLo		= (base >> I386_GDT_BASE_LOW_SHIFT)	& I386_GDT_BASE_LOW_MASK;
	i386_tss_descriptor.baseMid		= (base >> I386_GDT_BASE_MID_SHIFT)	& I386_GDT_BASE_MID_MASK;
	i386_tss_descriptor.baseHi		= (base >> I386_GDT_BASE_HIGH_SHIFT)	& I386_GDT_BASE_HIGH_MASK;
	i386_tss_descriptor.limit		= (limit >> I386_GDT_LIMIT_LOW_SHIFT)	& I386_GDT_LIMIT_LOW_MASK;
	i386_tss_descriptor.granularity		= granularity | ((limit >> I386_GDT_LIMIT_HIGH_SHIFT) & I386_GDT_LIMIT_HIGH_MASK);
	i386_tss_descriptor.flags		= flags;
}

void i386_tss_flush();

void i386_protection_init()
{
   uint32_t base = (uint32_t) &i386_tss_entry;
   uint32_t limit = base + sizeof(i386_tss_entry_t);
   i386_set_tss_descriptor(base, limit, 0x89, 0x00);
   memset(&i386_tss_entry, 0, sizeof(i386_tss_entry_t));
   i386_tss_entry.ss0	= 0x20;
   i386_tss_entry.esp0	= 0xBFFFDFFF;
   i386_tss_entry.cs	= 0x28 | I386_RPL_USERMODE_MASK;
   i386_tss_entry.ss	= i386_tss_entry.ds = i386_tss_entry.es = i386_tss_entry.fs = i386_tss_entry.gs = 0x30 | I386_RPL_USERMODE_MASK;
   i386_tss_flush();
}

void i386_tss_update( void )
{
	i386_task_context_t *tctx;
	tctx = (i386_task_context_t *) scheduler_current_task->arch_state;
	debugcon_printf("switching esp0=%x\n",tctx->tss_esp);
	i386_tss_entry.esp0	= tctx->tss_esp;
}

void process_user_call(void *entry, void *stack)
{
	i386_tss_update();
	i386_protection_user_call((uint32_t) entry, (uint32_t) stack);
}

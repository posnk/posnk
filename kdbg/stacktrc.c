/**
 * kdbg/stacktrc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */
#include "kdbg/stacktrc.h"
#include "kdbg/kdbgio.h"
#include "kdbg/kdbgmm.h"
#include "kernel/elf.h"

#ifdef ARCH_I386
#include "arch/i386/task_context.h"
#include "arch/i386/isr_entry.h"
extern char i386_strtab;
extern char i386_symtab;
extern char i386_symtab_end;
Elf32_Sym *kdbg_symtab = NULL;//&i386_symtab;
#endif

char *kdbg_symbol_name(uintptr_t addr)
{
	/*int i,f = 0;
	ptrdiff_t ssz = &i386_symtab_end - &i386_symtab;
	int sc = ssz / sizeof ( Elf32_Sym );
	//kdbg_printf("%i %i\n", sc,ssz);
	for ( i = 0; i < sc; i++ ) {
//	kdbg_printf("%x %i\n", kdbg_symtab[i].st_value,i);
		if ( kdbg_symtab[i].st_value <= addr &&
		     (kdbg_symtab[i].st_value +
		      kdbg_symtab[i].st_size) > addr )
			f = i;
	}
	if ( f != 0 )
		return (char *) (kdbg_symtab[f].st_name + &i386_strtab);*/
	return "";
}

llist_t *kdbg_do_calltrace()
{
#ifdef ARCH_I386
	int lvl = 0;
	kdbg_calltrace_t *entry;
#endif
	llist_t *t_list = kdbgmm_alloc(sizeof(llist_t));
	llist_create(t_list);
#ifdef ARCH_I386
	uintptr_t c_ebp, c_eip = (uintptr_t)&kdbg_do_calltrace;
	__asm__("movl %%ebp, %[fp]" :  /* output */ [fp] "=r" (c_ebp));
	for (;;) {
		entry = kdbgmm_alloc(sizeof(kdbg_calltrace_t));
		entry->frame_addr = c_ebp;
		entry->func_addr = c_eip;
		llist_add_end(t_list, (llist_t *) entry);
		if (c_ebp == 0xCAFE57AC)
			return t_list;
		c_eip = *((uintptr_t *) (c_ebp + 4));
		c_ebp = *((uintptr_t *)  c_ebp);
		lvl++;
		if (lvl > 5)
			return t_list;

	}
#endif
	return t_list;
}

void kdbg_pt_calltrace(scheduler_task_t *t)
{
#ifdef ARCH_I386
	struct csstack *nstate;
	i386_isr_stack_t *isrst;
	kdbg_calltrace_t aentry;
	i386_task_context_t *nctx;
	kdbg_calltrace_t *entry = &aentry;
	//llist_create(t_list);
	uintptr_t c_ebp, c_eip, l_ebp;
	nctx = (i386_task_context_t *) t->arch_state;
	nstate = ( struct csstack * ) nctx->kern_esp;
	c_ebp = nstate->regs.ebp;
	c_eip = nstate->eip;
	for (;;) {
		//entry = kdbgmm_alloc(sizeof(kdbg_calltrace_t));
		entry->frame_addr = c_ebp;
		entry->func_addr = c_eip;
		//llist_add_end(t_list, (llist_t *) entry);
		if (c_ebp == 0xCAFE8007) {
			kdbg_printf("       Kernel Entry\n");
			return;
		} else if (c_ebp == 0xCAFE57AC) {	
/*
Interrupt Entry: EBP set to CAFE57AC
Interrupt Stack
	uint32_t				ds;
	i386_pusha_registers_t	regs;
	uint32_t				int_id;
	uint32_t				error_code;
	uint32_t				eip;
	uint32_t				cs;
	uint32_t				eflags;
	uint32_t				esp;            | Ring0->Ring 3 Only!
	uint32_t				ss;             |
	
	
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
*/
			kdbg_printf("       Interrupt In:%s\n",
						kdbg_symbol_name(entry->func_addr));
			isrst = (i386_isr_stack_t*) (l_ebp+12);
			c_eip = isrst->eip;
			c_ebp = isrst->regs.ebp;
			l_ebp =(uintptr_t)isrst + sizeof(i386_isr_stack_t)-24;
			kdbg_printf("           ds: 0x%x\tint: 0x%x\terr: 0x%x\n",
							isrst->ds,
							isrst->int_id,
							isrst->error_code );
			kdbg_printf("           eip:0x%x\tcs: 0x%x\t flags: 0x%x\n",
                            c_eip,
							isrst->cs,
							isrst->eflags);
			kdbg_printf("           edi:0x%x\tesi: 0x%x\t ebp: 0x%x\n",
                            isrst->regs.edi,
                            isrst->regs.esi,
                            isrst->regs.ebp);
			kdbg_printf("           esp:0x%x\tebx: 0x%x\t edx: 0x%x\n",
                            isrst->regs.esp,
                            isrst->regs.ebx,
                            isrst->regs.edx);
			kdbg_printf("           ecx:0x%x\teax: 0x%x\n",
                            isrst->regs.ecx,
                            isrst->regs.eax);
			if ( isrst->cs == 0x2B ) {
				kdbg_printf("           ss: 0x%x\tesp:0x%x\n",
						isrst->ss, isrst->esp );
				kdbg_printf("        USER CODE\n");
				return;          
			}
		} else {
			l_ebp = c_ebp;
			if ( c_ebp < 0xc0000000 ) {
				kdbg_printf("        STACK CORRUPT\n");
				return;          
			}
			c_eip = *((uintptr_t *) (c_ebp + 4));
			c_ebp = *((uintptr_t *)  c_ebp);
			kdbg_printf("       0x%x %s() 0x%x\n", 
						entry->func_addr,
						kdbg_symbol_name(entry->func_addr),
						entry->frame_addr);
		}
	}
#endif
}


void kdbg_p_calltrace()
{
#ifdef ARCH_I386
	kdbg_calltrace_t aentry;
	kdbg_calltrace_t *entry = &aentry;
	//llist_create(t_list);
	uintptr_t c_ebp, c_eip = (uintptr_t)&kdbg_p_calltrace;
	__asm__("movl %%ebp, %[fp]" :  /* output */ [fp] "=r" (c_ebp));
	for (;;) {
		//entry = kdbgmm_alloc(sizeof(kdbg_calltrace_t));
		entry->frame_addr = c_ebp;
		entry->func_addr = c_eip;
		//llist_add_end(t_list, (llist_t *) entry);
		if (c_ebp == 0xCAFE57AC)
			return;
		c_eip = *((uintptr_t *) (c_ebp + 4));
		c_ebp = *((uintptr_t *)  c_ebp);
		kdbg_printf("       0x%x () 0x%x\n",entry->func_addr, entry->frame_addr);

	}
#endif
}

void kdbg_free_calltrace(llist_t *st)
{
	llist_t *a;
	for (a = st->next; a != st; a = st->next) {
		llist_unlink(a);
		kdbgmm_free(a, sizeof(kdbg_calltrace_t));
	}
	kdbgmm_free(st, sizeof(llist_t));
}

void kdbg_print_calltrace(llist_t *st){
	llist_t *a;
	kdbg_calltrace_t *entry;
	for (a = st->prev; a != st; a = a->prev) {
		entry = (kdbg_calltrace_t *) a;
		kdbg_printf("       0x%x () 0x%x\n",entry->func_addr, entry->frame_addr);

	}
}

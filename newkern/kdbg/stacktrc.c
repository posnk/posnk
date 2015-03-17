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

llist_t *kdbg_do_calltrace()
{
#ifdef ARCH_I386
	int lvl = 0;
	kdbg_calltrace_t *entry;
#endif
	llist_t *t_list = kdbgmm_alloc(sizeof(llist_t));
	llist_create(t_list);
#ifdef ARCH_I386
	uintptr_t c_ebp, c_eip = &kdbg_do_calltrace;
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

void kdbg_p_calltrace()
{
#ifdef ARCH_I386
	int lvl = 0;
	kdbg_calltrace_t aentry;
	kdbg_calltrace_t *entry = &aentry;
	//llist_create(t_list);
	uintptr_t c_ebp, c_eip = &kdbg_p_calltrace;
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

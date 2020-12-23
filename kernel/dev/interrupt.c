/**
 * kernel/interrupt.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-07-2014 - Created
 */

#include "kernel/interrupt.h"
#include "kernel/heapmm.h"
#define CON_SRC "irqmgr"
#include "kernel/console.h"

llist_t	interrupt_handler_lists[INTERRUPT_IRQ_COUNT];

void interrupt_dispatch(irq_id_t irq_id)
{
	llist_t *_e;
	interrupt_handler_t *e;
	llist_t *list = &(interrupt_handler_lists[irq_id]);
	for (_e = list->next; _e != list; _e = _e->next) {
		e = (interrupt_handler_t *) _e;
		if (e->handler(irq_id, e->context))
			return;
	}
	printf(CON_TRACE, "unhandled IRQ%i!", irq_id);
}

void interrupt_register_handler(irq_id_t irq_id, irq_handler_t handler, void *context)
{
	interrupt_handler_t *desc = heapmm_alloc(sizeof(interrupt_handler_t));
	if (!desc) {
		printf(CON_ERROR, "could not register interrupt, out of heap!");
		return;
	}
	desc->handler = handler;
	desc->context = context;
	llist_add_end(&(interrupt_handler_lists[irq_id]), (llist_t *) desc);
	printf(CON_INFO, "registered handler for IRQ%i!", irq_id);
}

void interrupt_init()
{
	int _t;
	for (_t = 0; _t < INTERRUPT_IRQ_COUNT; _t++)
		llist_create(&(interrupt_handler_lists[_t]));
}

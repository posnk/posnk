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
#include "kernel/earlycon.h"

llist_t	interrupt_handler_lists[INTERRUPT_IRQ_COUNT];

void interrupt_dispatch(irq_id_t irq_id)
{
	llist_t *_e;
	interrupt_handler_t *e;
	llist_t *list = &(interrupt_handler_lists[irq_id]);
	for (_e = list->next; _e != list; _e = _e->next) {
		e = (interrupt_handler_t *) _e;
		if (e->handler(irq_id))
			return;
	}
	debugcon_printf("warning: unhandled IRQ%i!\n", irq_id);
}

void interrupt_register_handler(irq_id_t irq_id, irq_handler_t handler)
{
	interrupt_handler_t *desc = heapmm_alloc(sizeof(interrupt_handler_t));
	if (!desc) {
		debugcon_printf("error: could not register interrupt, out of heap!\n");
		return;
	}
	desc->handler = handler;
	llist_add_end(&(interrupt_handler_lists[irq_id]), (llist_t *) desc);
	debugcon_printf("info: registered handler for IRQ%i!\n", irq_id);
}

void interrupt_init()
{
	int _t;
	for (_t = 0; _t < INTERRUPT_IRQ_COUNT; _t++)
		llist_create(&(interrupt_handler_lists[_t]));
}

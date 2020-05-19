/**
 * kernel/interrupt.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-07-2014 - Created
 */

#ifndef __KERNEL_INTERRUPT_H__
#define __KERNEL_INTERRUPT_H__

#include <stdint.h>
#include "util/llist.h"
#include "config.h"
#ifdef ARCH_I386
#define INTERRUPT_IRQ_COUNT	(17)
#endif
#ifdef ARCH_ARMV7
#define INTERRUPT_IRQ_COUNT	(256)
#endif
typedef uint8_t irq_id_t;
typedef int (*irq_handler_t)(irq_id_t, void*);

typedef struct interrupt_handler_s interrupt_handler_t;

struct interrupt_handler_s {
	llist_t		 link;
	irq_handler_t	 handler;
	void 		*context;
};

void interrupt_dispatch(irq_id_t irq_id);

void interrupt_register_handler(irq_id_t irq_id, irq_handler_t handler, void *context);

void interrupt_init(void);

#endif

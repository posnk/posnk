/* 
 * arch/i386/protection.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2010       - Created
 * 02-04-2014 - Cleaned up
 */

#ifndef __ARCH_I386_PROTECTION_H__
#define __ARCH_I386_PROTECTION_H__
#include <stdint.h>

#define	I386_GDT_ACCESSED_BIT		1<<0
#define I386_GDT_READABLE_BIT		1<<1
#define I386_GDT_WRITABLE_BIT		1<<1
#define I386_GDT_CONFORMING_BIT		1<<2
#define I386_GDT_GROW_DIRECTION_BIT	1<<2
#define I386_GDT_EXECUTABLE_BIT		1<<3
#define I386_GDT_NON_SYSTEM_BIT		1<<4
#define I386_GDT_SUPERVISOR_MASK	0<<5
#define I386_GDT_DRIVER_0_MASK		1<<5
#define I386_GDT_DRIVER_1_MASK		2<<5
#define I386_GDT_USERMODE_MASK		3<<5
#define I386_GDT_PRESENT_BIT		1<<7
#define I386_GDT_OS_DEFINED_BIT		1<<4
#define I386_GDT_RESERVED_BIT		1<<5
#define I386_GDT_32_BIT_SEGMENT		1<<6
#define I386_GDT_GRANULARITY_4K_BIT	1<<7
#define I386_GDT_LIMIT_HIGH_MASK	0x0F
#define I386_GDT_LIMIT_HIGH_SHIFT	0x10
#define I386_GDT_LIMIT_LOW_MASK		0xFFFF
#define I386_GDT_LIMIT_LOW_SHIFT	0x00
#define I386_GDT_BASE_LOW_MASK		0xFFFF
#define I386_GDT_BASE_LOW_SHIFT		0x00
#define I386_GDT_BASE_MID_MASK		0xFF
#define I386_GDT_BASE_MID_SHIFT		0x10
#define I386_GDT_BASE_HIGH_MASK		0xFF
#define I386_GDT_BASE_HIGH_SHIFT	0x18
#define I386_GDT_MAX_DESCRIPTORS	0x06 //<-- Change to 0x06 for TSS.
#define I386_GDT_SELECTOR_MULTIPLIER	0x08
#define I386_RPL_SUPERVISOR_MASK	0x00
#define I386_RPL_DRIVER_0_MASK		0x01
#define I386_RPL_DRIVER_1_MASK		0x02
#define I386_RPL_USERMODE_MASK		0x03

typedef struct i386_gdt_descriptor {
	uint16_t		limit;
	uint16_t		baseLo;
	uint8_t			baseMid;
	uint8_t			flags;
	uint8_t			granularity;
	uint8_t			baseHi;
} i386_gdt_descriptor_t;

typedef struct i386_gdtr {
	uint16_t		m_limit;
	uint32_t		m_base;
} i386_gdt_pointer_t;

typedef struct i386_tss_entry
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // Unused...
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         // The value to load into ES when we change to kernel mode.
   uint32_t cs;         // The value to load into CS when we change to kernel mode.
   uint32_t ss;         // The value to load into SS when we change to kernel mode.
   uint32_t ds;         // The value to load into DS when we change to kernel mode.
   uint32_t fs;         // The value to load into FS when we change to kernel mode.
   uint32_t gs;         // The value to load into GS when we change to kernel mode.
   uint32_t ldt;        // Unused...
   uint16_t trap;
   uint16_t iomap_base;
} i386_tss_entry_t;

void i386_protection_init();

void i386_tss_update( void );

void i386_protection_user_call(uint32_t eip, uint32_t esp);

void process_user_call(void *entry, void *stack);

#endif

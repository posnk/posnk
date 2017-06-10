#include <string.h>
#include "arch/i386/idt.h"
#include "arch/i386/x86.h"
#include "arch/i386/isr_entry.h"

#define I386_KERNEL_CODE_SEGMENT 0x18

#define ADD_ISR_ENTRY(NO)	i386_idt_set_descriptor(NO, &i386_isr_entry_ ## NO, I386_IDT_32BIT_INTERRUPT, I386_KERNEL_CODE_SEGMENT);

i386_idt_descriptor_t			i386_idt[I386_IDT_MAX_DESCRIPTORS];
i386_idtr_t				i386_idt_pointer;

void i386_load_idt(){
	asm volatile("lidt %0"::"m" (i386_idt_pointer));
}

void i386_isr_entry_undefined(){
	
	for (;;);
}

void i386_idt_set_descriptor(uint8_t id, void *isr, uint8_t flags, uint32_t segment){
	i386_idt_descriptor_t *idt_d = &i386_idt[id];
	uint32_t isr_i = (uint32_t) isr;
	memset(idt_d, 0, sizeof(i386_idt_descriptor_t));
	idt_d->offsetLo		= (uint16_t) ( isr_i       & 0xFFFF);	
	idt_d->offsetHi		= (uint16_t) ((isr_i >> 16) & 0xFFFF);
	idt_d->segmentOrTSS	= segment;
	idt_d->flags		= flags;
}

void i386_idt_initialize(){
	int c;
	i386_idt_pointer.m_base = (uint32_t) i386_idt;
	i386_idt_pointer.m_limit =(I386_IDT_MAX_DESCRIPTORS * sizeof(i386_idt_descriptor_t)) - 1;
	for (c = 0;c < I386_IDT_MAX_DESCRIPTORS;c++)
		i386_idt_set_descriptor((uint8_t) c, &i386_isr_entry_undefined, I386_IDT_32BIT_INTERRUPT, I386_KERNEL_CODE_SEGMENT);

	ADD_ISR_ENTRY(0)
	ADD_ISR_ENTRY(1)
	ADD_ISR_ENTRY(3)
	ADD_ISR_ENTRY(4)
	ADD_ISR_ENTRY(5)
	ADD_ISR_ENTRY(6)
	ADD_ISR_ENTRY(7)
	ADD_ISR_ENTRY(8)
	ADD_ISR_ENTRY(9)
	ADD_ISR_ENTRY(10)
	ADD_ISR_ENTRY(11)
	ADD_ISR_ENTRY(12)
	ADD_ISR_ENTRY(13)
	ADD_ISR_ENTRY(14)
	ADD_ISR_ENTRY(16)

	ADD_ISR_ENTRY(32)
	ADD_ISR_ENTRY(33)
	ADD_ISR_ENTRY(34)
	ADD_ISR_ENTRY(35)
	ADD_ISR_ENTRY(36)
	ADD_ISR_ENTRY(37)
	ADD_ISR_ENTRY(38)
	ADD_ISR_ENTRY(39)
	ADD_ISR_ENTRY(40)
	ADD_ISR_ENTRY(41)
	ADD_ISR_ENTRY(42)
	ADD_ISR_ENTRY(43)
	ADD_ISR_ENTRY(44)
	ADD_ISR_ENTRY(45)
	ADD_ISR_ENTRY(46)
	ADD_ISR_ENTRY(47)

	i386_idt_set_descriptor(0x80, &i386_isr_entry_80h, 0xEE, I386_KERNEL_CODE_SEGMENT);

	i386_load_idt();
}


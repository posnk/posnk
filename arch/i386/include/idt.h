#ifndef __ATL_IDT_H__
#define __ATL_IDT_H__
#include <cstdint.h>
#include "x86.h"
#include "dri.h"
#ifdef __cplusplus
extern "C"{
#endif

#define IDT_PRESENT_BIT			1<<7
#define IDT_SUPERVISOR_MASK		0<<5
#define IDT_DRIVER_0_MASK		1<<5
#define IDT_DRIVER_1_MASK		2<<5
#define IDT_USERMODE_MASK		3<<5
#define IDT_SYSTEM_SEGMENT_BIT	1<<4
#define IDT_32_BIT_GATE_BIT		1<<3
#define IDT_RESERVED_BIT		1<<2
#define IDT_NOT_TASK_GATE		1<<1
#define IDT_NOT_INTERRUPT_GATE  1<<0
#define IDT_TRAP_GATE_MASK		IDT_NOT_TASK_GATE | IDT_NOT_INTERRUPT_GATE
#define IDT_TASK_GATE_MASK		IDT_NOT_INTERRUPT_GATE
#define IDT_INTERRUPT_GATE_MASK IDT_NOT_TASK_GATE
#define IDT_32BIT_INTERRUPT		0x8E
#define IDT_BS_32				IDT_PRESENT_BIT | IDT_SUPERVISOR_MASK | IDT_32_BIT_GATE_BIT | IDT_RESERVED_BIT | IDT_INTERRUPT_GATE_MASK
#define IDT_32BIT_TRAP			IDT_PRESENT_BIT | IDT_SUPERVISOR_MASK | IDT_32_BIT_GATE_BIT | IDT_RESERVED_BIT | IDT_TRAP_GATE_MASK
#define IDT_MAX_DESCRIPTORS		256

typedef struct idt_descriptor_s {
	uint16_t		offsetLo;
	uint16_t		segmentOrTSS;
	uint8_t			reserved;
	uint8_t			flags;
	uint16_t		offsetHi;
} idt_descriptor;

typedef struct idtr_s {
	uint16_t		m_limit;
	uint32_t		m_base;
} idt_pointer;
void unhandled_interrupt(registers_t regs);
void x86_load_idt();
void x86_idt_initialize();
void x86_idt_set_descriptor(uint8_t id, void *isr, uint8_t flags, uint32_t segment);
void isr_syscall();
void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
void isr32();
void isr33();
void isr34();
void isr35();
void isr36();
void isr37();
void isr38();
void isr39();
void isr40();
void isr41();
void isr42();
void isr42();
void isr43();
void isr44();
void isr45();
void isr46();
void isr47();
#ifdef __cplusplus
}
#endif

#endif
#ifndef __HAL_X86_H__
#define __HAL_X86_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

#define I386_FLAGS_CF			(1<<0)
#define I386_FLAGS_PF			(1<<2)
#define I386_FLAGS_AF			(1<<4)
#define I386_FLAGS_ZF			(1<<6)
#define I386_FLAGS_SF			(1<<7)
#define I386_FLAGS_TF			(1<<8)
#define I386_FLAGS_IF			(1<<9)
#define I386_FLAGS_DF			(1<<10)
#define I386_FLAGS_OF			(1<<11)
#define I386_FLAGS_IOPL(Flg)	((Flg>>12)&3)

#define I386_EXCEPTION_DIV_ZERO		0
#define I386_EXCEPTION_DEBUG		1
#define I386_EXCEPTION_BREAKPOINT	3
#define I386_EXCEPTION_OVERFLOW		4
#define I386_EXCEPTION_BOUNDS_CHECK	5
#define I386_EXCEPTION_INVALID_OPCODE	6
#define I386_EXCEPTION_NO_COPROCESSOR	7
#define I386_EXCEPTION_DOUBLE_FAULT	8
#define I386_EXCEPTION_COPROCESSOR_OVR	9
#define I386_EXCEPTION_INVALID_TSS	10
#define I386_EXCEPTION_SEG_FAULT	11
#define I386_EXCEPTION_STACK_EXCEPTION	12
#define I386_EXCEPTION_GP_FAULT		13
#define I386_EXCEPTION_PAGE_FAULT	14
#define I386_EXCEPTION_COPROCESSOR_ERR	16

struct i386_pusha_registers
{
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
} __attribute__((packed));

typedef struct i386_pusha_registers i386_pusha_registers_t;

/* Port IO C wrappers */

uint8_t i386_inb(uint16_t port);
void i386_outb(uint16_t port,uint8_t data);

uint16_t i386_inw(uint16_t port);
void i386_outw(uint16_t port,uint16_t value);

uint32_t i386_inl(uint16_t port);
void i386_outl(uint16_t port,uint32_t value);

void i386_fpu_initialize();

void i386_fpu_on_cs();
void i386_fpu_sigenter();
void i386_fpu_sigexit();
int i386_fpu_handle_ill();

#ifdef __cplusplus
}
#endif
#endif

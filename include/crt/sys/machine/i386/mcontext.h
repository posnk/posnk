/**
 * sys/machine/i386/mcontext.h
 *
 * Part of P-OS.
 *
 *
 * Written by Peter Bosch <me@pbx.sh>
 */

#ifndef __SYS_MCONTEXT_I386_H__
#define __SYS_MCONTEXT_I386_H__

/**
 * Structure containing the architecture specific state
 */
struct mcontext {

	uint32_t       reg_cs;
	uint32_t       reg_eip;
	uint32_t       reg_ss;
	uint32_t       reg_ds;
	uint32_t       reg_edi;
	uint32_t       reg_esi;
	uint32_t       reg_ebp;
	uint32_t       reg_esp;
	uint32_t       reg_ebx;
	uint32_t       reg_edx;
	uint32_t       reg_ecx;
	uint32_t       reg_eax;
	uint32_t       reg_eflags;
	uint8_t        reg_xsave[512];
	int            using_fpu;

};

typedef struct mcontext mcontext_t;

#endif

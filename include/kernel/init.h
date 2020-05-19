/**
 * kernel/init.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 */

#ifndef __KERNEL_INIT_H__
#define __KERNEL_INIT_H__

void arch_init_early();
void arch_init_late();
void kmain();
void kinit_start_uinit( void );
void kinit_start_idle_task( void );

#endif

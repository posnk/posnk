/**
 * arch/armv7/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2014 - Created
 */

#include <stdint.h>

void sercon_puts(const char *string);
void sercon_init();

void halt()
{
	for(;;);
}

void armv7_init( void )
{

	sercon_init();

	sercon_printf("posnk kernel built on %s %s\n", __DATE__,__TIME__);

	halt();
	
}

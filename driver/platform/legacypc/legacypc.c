/**
 * @file driver/platform/legacypc/legacypc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include "driver/platform/legacypc/legacypc.h"
#include "driver/platform/legacypc/pit.h"
#include "driver/platform/legacypc/pic.h"

#include "kernel/interrupt.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/time.h"
#include <string.h>
#include <assert.h>

static int pc_systick_isr( irq_id_t id, void *context )
{
	assert ( id == INT_PC_SYSTIMER );

	timer_interrupt();

	return 1;
}

void legacypc_init()
{

	/* Initialize the interrupt controllers */
	pic_initialize();


	/* Initialize the system timer */
	pit_setup(1000, 0, PIT_OCW_MODE_RATEGEN);

	interrupt_register_handler( INT_PC_SYSTIMER, &pc_systick_isr, NULL);	

}

/**
 * @brief Get the interrupt number for a incoming interrupt.
 * The x86 processors have interrupt vectors passed directly by the interrupt
 * controllers, hence we do not need to ask the PIC what interrupt came in.
 * @param int_channel The vector number for x86
 */
int platform_get_interrupt_id( int int_channel )
{
	if ( int_channel == INT_VECTOR_NMI )
		return INT_NMI;

	if ( int_channel == INT_VECTOR_PIC0_LOWPRI ) {
		/* If the master PIC gets a spurious IRQ, it will acknowledge with its
		 * lowest priority vector. This means that if we see that vector we 
		 * have to disambiguate between spurious and real interrupts. */

		/* If the interrupt was real, the bit for it in the PIC0 interrupt 
		 * status register should be set. */
		if ( ~pic_read_isr(0) & 0x80 ) {

			/* It was not, this interrupt was spurious */
			return INT_SPURIOUS;
		}
	}

	if ( int_channel == INT_VECTOR_PIC1_LOWPRI ) {
		/* If the slave PIC gets a spurious IRQ, it will acknowledge with its
		 * lowest priority vector. This means that if we see that vector we 
		 * have to disambiguate between spurious and real interrupts. */

		/* If the interrupt was real, the bit for it in the PIC1 interrupt 
		 * status register should be set. */
		if ( ~pic_read_isr(1) & 0x80 ) {

			/* It was not, this interrupt was spurious */
		
			/* Because all slave interrupts go through the master PIC first 
			 * we need to signal an EOI for the master PIC */
			pic_send_end_of_interrupt(0);

			return INT_SPURIOUS;
		}
	}


	return int_channel - INT_VECTOR_START;
}

void platform_end_of_interrupt( int int_channel, int int_id )
{
	/* x86 NMI channel does not run through the legacy PICs */
	if ( int_id == INT_NMI || int_id == INT_SPURIOUS )
		return;

	/* If the interrupt came from the slave PIC, we have to signal the 
	 * end-of-interrupt to it as well as the master PIC. Because of the way
	 * interrupts work, resetting the master before the slave would result in
	 * the interrupt immediately refiring on the master end. This is why we 
	 * reset the slave before the master */
	if ( int_id >= INT_PIC1_START )
		pic_send_end_of_interrupt(1);

	pic_send_end_of_interrupt(0);
}


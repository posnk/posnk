#define PORT 0x2f8   /* COM1 */
#include "driver/console/vgacon/vgacon.h"
#include <string.h>
#include <stdint.h> 
#include <sys/types.h>
#include "kernel/tty.h"

int sserial_received() {
   return i386_inb(PORT + 5) & 1;
}
 
void sercon_isr() {
   while (sserial_received() != 0)
	tty_input_char(0x0D00, i386_inb(PORT));
}

int sis_transmit_empty() {
   return i386_inb(PORT + 5) & 0x20;
}
 
int sercon_putc(__attribute__((__unused__)) dev_t dev, char a) {
   while (sis_transmit_empty() == 0);
 
   i386_outb(PORT,a);
   return 0;
}

void sercon_init() {
   i386_outb(PORT + 1, 0x01);    // Disable all interrupts
   i386_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   i386_outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   i386_outb(PORT + 1, 0x00);    //                  (hi byte)
   i386_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   i386_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   i386_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   tty_register_driver("sercon", 13, 1, &sercon_putc);

}

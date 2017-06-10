#define PORT 0x3f8   /* COM1 */
#include "driver/console/vgacon/vgacon.h"
#include <string.h>
#include <stdint.h> 

void debugcon_init() {
   i386_outb(PORT + 1, 0x00);    // Disable all interrupts
   i386_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   i386_outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   i386_outb(PORT + 1, 0x00);    //                  (hi byte)
   i386_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   i386_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   i386_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int debugcon_have_data() {
   return i386_inb(PORT + 5) & 1;
}
 
char debugcon_getc() {
   while (debugcon_have_data() == 0);
 
   return i386_inb(PORT);
}

int is_transmit_empty() {
   return i386_inb(PORT + 5) & 0x20;
}
 
void debugcon_putc(char a) {
   while (is_transmit_empty() == 0);
 
   i386_outb(PORT,a);
}

void debugcon_aputs(const char *str)
{
	int i = 0;
   	while (str[i])
   	{
		debugcon_putc(str[i++]);
   	}
}

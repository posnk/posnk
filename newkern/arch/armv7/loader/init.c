/**
 * arch/armv7/loader/init.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-03-2014 - Created
 */

#include <stdint.h>

/*
 * The CPU must be in SVC (supervisor) mode with both IRQ and FIQ interrupts disabled.
 * The MMU must be off, i.e. code running from physical RAM with no translated addressing.
 * Data cache must be off
 * Instruction cache may be either on or off
 * CPU register 0 must be 0
 * CPU register 1 must be the ARM Linux machine type
 * CPU register 2 must be the physical address of the parameter list
 * Our assembly stub has prepared 4K stack for us.
 */
/*
 * 0x90000000   --------------------- END OF RAM 
 *
 * 0x82000000	| posnk kernel[text]|
 *		|-------------------|
 * 0x8????000	|   	vmpos	    |
 * 	|	|-------------------|
 * 0x80008000	|   armv7_loader    |
 * 	|	|-------------------|
 * 0x80000000	| BOOTLOADER, ATAGS |
 * 		--------------------- START OF RAM
 */

void sercon_puts(const char *string);
void sercon_init();

#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009

/* structures for each atag */
struct atag_header {
        uint32_t size; /* length of tag in words including this header */
        uint32_t tag;  /* tag type */
};

struct atag_core {
        uint32_t flags;
        uint32_t pagesize;
        uint32_t rootdev;
};

struct atag_mem {
        uint32_t     size;
        uint32_t     start;
};

struct atag_videotext {
        uint8_t              x;
        uint8_t               y;
        uint16_t             video_page;
        uint8_t               video_mode;
        uint8_t               video_cols;
        uint16_t             video_ega_bx;
        uint8_t               video_lines;
        uint8_t               video_isvga;
        uint16_t             video_points;
};

struct atag_ramdisk {
        uint32_t flags;
        uint32_t size;
        uint32_t start;
};

struct atag_initrd2 {
        uint32_t start;
        uint32_t size;
};

struct atag_serialnr {
        uint32_t low;
        uint32_t high;
};

struct atag_revision {
        uint32_t rev;
};

struct atag_videolfb {
        uint16_t        lfb_width;
        uint16_t        lfb_height;
        uint16_t        lfb_depth;
        uint16_t        lfb_linelength;
        uint32_t        lfb_base;
        uint32_t        lfb_size;
        uint8_t               red_size;
        uint8_t               red_pos;
        uint8_t               green_size;
        uint8_t               green_pos;
        uint8_t               blue_size;
        uint8_t              blue_pos;
        uint8_t          rsvd_size;
        uint8_t           rsvd_pos;
};

struct atag_cmdline {
        char    cmdline[1];
};

struct atag {
        struct atag_header hdr;
        union {
                struct atag_core         core;
                struct atag_mem          mem;
                struct atag_videotext    videotext;
                struct atag_ramdisk      ramdisk;
                struct atag_initrd2      initrd2;
                struct atag_serialnr     serialnr;
                struct atag_revision     revision;
                struct atag_videolfb     videolfb;
                struct atag_cmdline      cmdline;
        } u;
};


#define tag_next(t)     ((struct atag *)((uint32_t *)(t) + (t)->hdr.size))
#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)
static struct atag *params; /* used to point at the current tag */


void armv7_entry(uint32_t unused_reg, uint32_t mach_type, uint32_t atag_addr)
{
	struct atag *params = (struct atag *) atag_addr;

	sercon_init();
	sercon_puts("Hello, ARM World!\n");
	sercon_printf("Parameters passed to us:\nr0: %x, r1: %x, r2:%x\n", unused_reg, mach_type, atag_addr);

	for (;;) {
		switch (params->hdr.tag) {

			case ATAG_CORE:
				sercon_printf("ATAG_CORE\n");
				sercon_printf("    flags=0x%x\n", params->u.core.flags);
				sercon_printf("    pagesize=0x%x\n", params->u.core.pagesize);
				sercon_printf("    rootdev=0x%x\n", params->u.core.rootdev);
				break;

			case ATAG_MEM:
				sercon_printf("ATAG_MEM\n");
				sercon_printf("    size=0x%x\n", params->u.mem.size);
				sercon_printf("    start=0x%x\n", params->u.mem.start);
				break;

			case ATAG_VIDEOTEXT:
				sercon_printf("ATAG_VIDEOTEXT\n");
				sercon_printf("    x=%x\n", (int)params->u.videotext.x);
				sercon_printf("    y=%x\n", (int)params->u.videotext.y);
				sercon_printf("    video_page=0x%x\n", (int)params->u.videotext.video_page);
				sercon_printf("    video_mode=0x%x\n", (int)params->u.videotext.video_mode);
				sercon_printf("    video_cols=0x%x\n", (int)params->u.videotext.video_cols);
				sercon_printf("    video_ega_bx=0x%x\n", (int)params->u.videotext.video_ega_bx);
				sercon_printf("    video_lines=0x%x\n", (int)params->u.videotext.video_lines);
				sercon_printf("    video_isvga=0x%x\n", (int)params->u.videotext.video_isvga);
				sercon_printf("    video_points=0x%x\n", (int)params->u.videotext.video_points);
				break;

			case ATAG_RAMDISK:
				sercon_printf("ATAG_RAMDISK\n");
				sercon_printf("    flags=0x%x\n", params->u.ramdisk.flags);
				sercon_printf("    size=0x%x\n", params->u.ramdisk.size);
				sercon_printf("    start=0x%x\n", params->u.ramdisk.start);
				break;

			case ATAG_INITRD2:
				sercon_printf("ATAG_INITRD2\n");
				sercon_printf("    start=0x%x\n", params->u.initrd2.start);
				sercon_printf("    size=0x%x\n", params->u.initrd2.size);
				break;

			case ATAG_SERIAL:
				sercon_printf("ATAG_SERIAL\n");
				sercon_printf("    low=0x%x\n", params->u.serialnr.low);
				sercon_printf("    high=0x%x\n", params->u.serialnr.high);
				break;

			case ATAG_REVISION:
				sercon_printf("ATAG_REVISION\n");
				sercon_printf("    rev=0x%x\n", params->u.revision.rev);
				break;

			case ATAG_VIDEOLFB:
				sercon_printf("ATAG_VIDEOLFB\n");
				sercon_printf("    lfb_width=0x%x\n", (int) params->u.videolfb.lfb_width);
				sercon_printf("    lfb_height=0x%x\n", (int) params->u.videolfb.lfb_height);
				sercon_printf("    lfb_depth=0x%x\n", (int) params->u.videolfb.lfb_depth);
				sercon_printf("    lfb_linelength=0x%x\n", (int) params->u.videolfb.lfb_linelength);
				sercon_printf("    lfb_base=0x%x\n", params->u.videolfb.lfb_base);
				sercon_printf("    lfb_size=0x%x\n", params->u.videolfb.lfb_size);
				sercon_printf("    red_size=0x%x\n", (int)params->u.videolfb.red_size);
				sercon_printf("    red_pos=0x%x\n", (int)params->u.videolfb.red_pos);
				sercon_printf("    green_size=0x%x\n", (int)params->u.videolfb.green_size);
				sercon_printf("    green_pos=0x%x\n", (int)params->u.videolfb.green_pos);
				sercon_printf("    blue_size=0x%x\n", (int)params->u.videolfb.blue_size);
				sercon_printf("    blue_pos=0x%x\n", (int)params->u.videolfb.blue_pos);
				sercon_printf("    rsvd_size=0x%x\n", (int)params->u.videolfb.rsvd_size);
				sercon_printf("    rsvd_pos=0x%x\n", (int)params->u.videolfb.rsvd_pos);
				break;

			case ATAG_NONE:
				sercon_printf("ATAG_NONE\n");
				goto outofloop;
		}
		params = tag_next(params);
	}
outofloop:
	for (;;);
}

/**
 * arch/armv7/atags.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 08-03-2015 - Created
 */

#include "arch/armv7/atags.h"
#include "kernel/physmm.h"
#include "config.h"

uint32_t armv7_atag_initrd_pa = 0xFFFFFFFF;
uint32_t armv7_atag_initrd_sz = 0x0;
char *armv7_atag_cmdline = "";

int sercon_printf(const char* str,...);

#ifndef CONFIG_DEBUG_ATAGS

int armv7_atag_debug_noop(const char* str,...)
{
	return 0;
}
#define armv7_atag_debug	armv7_atag_debug_noop

#else

#define armv7_atag_debug	sercon_printf

#endif

void armv7_atag_mem(uint32_t _start, uint32_t size)
{
	physaddr_t start, end;
	start = (physaddr_t) _start;
	end = start + (physaddr_t) size;
	sercon_printf("physmm: registering ram area 0x%x-0x%x\n", start, end);
	physmm_free_range( start, end );
}

void armv7_parse_atags( void * atag_addr )
{	
	struct atag *params = (struct atag *) atag_addr;

	for (;;) {
		switch (params->hdr.tag) {

			case ATAG_CORE:
				armv7_atag_debug("ATAG_CORE\n");
				armv7_atag_debug("    flags=0x%x\n", params->u.core.flags);
				armv7_atag_debug("    pagesize=0x%x\n", params->u.core.pagesize);
				armv7_atag_debug("    rootdev=0x%x\n", params->u.core.rootdev);
				break;

			case ATAG_MEM:
				armv7_atag_debug("ATAG_MEM\n");
				armv7_atag_debug("    size=0x%x\n", params->u.mem.size);
				armv7_atag_debug("    start=0x%x\n", params->u.mem.start);
				armv7_atag_mem(params->u.mem.start, params->u.mem.size);
				break;

			case ATAG_VIDEOTEXT:
				armv7_atag_debug("ATAG_VIDEOTEXT\n");
				armv7_atag_debug("    x=%x\n", (int)params->u.videotext.x);
				armv7_atag_debug("    y=%x\n", (int)params->u.videotext.y);
				armv7_atag_debug("    video_page=0x%x\n", (int)params->u.videotext.video_page);
				armv7_atag_debug("    video_mode=0x%x\n", (int)params->u.videotext.video_mode);
				armv7_atag_debug("    video_cols=0x%x\n", (int)params->u.videotext.video_cols);
				armv7_atag_debug("    video_ega_bx=0x%x\n", (int)params->u.videotext.video_ega_bx);
				armv7_atag_debug("    video_lines=0x%x\n", (int)params->u.videotext.video_lines);
				armv7_atag_debug("    video_isvga=0x%x\n", (int)params->u.videotext.video_isvga);
				armv7_atag_debug("    video_points=0x%x\n", (int)params->u.videotext.video_points);
				break;

			case ATAG_RAMDISK:
				armv7_atag_debug("ATAG_RAMDISK\n");
				armv7_atag_debug("    flags=0x%x\n", params->u.ramdisk.flags);
				armv7_atag_debug("    size=0x%x\n", params->u.ramdisk.size);
				armv7_atag_debug("    start=0x%x\n", params->u.ramdisk.start);
				break;

			case ATAG_INITRD2:
				armv7_atag_debug("ATAG_INITRD2\n");
				armv7_atag_debug("    start=0x%x\n", params->u.initrd2.start);
				armv7_atag_debug("    size=0x%x\n", params->u.initrd2.size);
				armv7_atag_initrd_pa = params->u.initrd2.start;
				armv7_atag_initrd_sz = params->u.initrd2.size;
				break;

			case ATAG_SERIAL:
				armv7_atag_debug("ATAG_SERIAL\n");
				armv7_atag_debug("    low=0x%x\n", params->u.serialnr.low);
				armv7_atag_debug("    high=0x%x\n", params->u.serialnr.high);
				break;

			case ATAG_REVISION:
				armv7_atag_debug("ATAG_REVISION\n");
				armv7_atag_debug("    rev=0x%x\n", params->u.revision.rev);
				break;

			case ATAG_VIDEOLFB:
				armv7_atag_debug("ATAG_VIDEOLFB\n");
				armv7_atag_debug("    lfb_width=0x%x\n", (int) params->u.videolfb.lfb_width);
				armv7_atag_debug("    lfb_height=0x%x\n", (int) params->u.videolfb.lfb_height);
				armv7_atag_debug("    lfb_depth=0x%x\n", (int) params->u.videolfb.lfb_depth);
				armv7_atag_debug("    lfb_linelength=0x%x\n", (int) params->u.videolfb.lfb_linelength);
				armv7_atag_debug("    lfb_base=0x%x\n", params->u.videolfb.lfb_base);
				armv7_atag_debug("    lfb_size=0x%x\n", params->u.videolfb.lfb_size);
				armv7_atag_debug("    red_size=0x%x\n", (int)params->u.videolfb.red_size);
				armv7_atag_debug("    red_pos=0x%x\n", (int)params->u.videolfb.red_pos);
				armv7_atag_debug("    green_size=0x%x\n", (int)params->u.videolfb.green_size);
				armv7_atag_debug("    green_pos=0x%x\n", (int)params->u.videolfb.green_pos);
				armv7_atag_debug("    blue_size=0x%x\n", (int)params->u.videolfb.blue_size);
				armv7_atag_debug("    blue_pos=0x%x\n", (int)params->u.videolfb.blue_pos);
				armv7_atag_debug("    rsvd_size=0x%x\n", (int)params->u.videolfb.rsvd_size);
				armv7_atag_debug("    rsvd_pos=0x%x\n", (int)params->u.videolfb.rsvd_pos);
				break;

			case ATAG_CMDLINE:
				armv7_atag_debug("ATAG_CMDLINE\n");
				armv7_atag_debug("    cmdline=\"%s\"\n", params->u.cmdline.cmdline);
				armv7_atag_cmdline = params->u.cmdline.cmdline;
				break;

			case ATAG_NONE:
				armv7_atag_debug("ATAG_NONE\n");
				return;
		}
		params = tag_next(params);
	}

}

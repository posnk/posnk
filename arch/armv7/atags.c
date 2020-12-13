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

#define CON_SRC "atags"
#include "kernel/console.h"
#include "arch/armv7/atags.h"
#include "kernel/physmm.h"
#include "config.h"

uint32_t armv7_atag_initrd_pa = 0xFFFFFFFF;
uint32_t armv7_atag_initrd_sz = 0x0;
char *armv7_atag_cmdline = "";

void armv7_atag_mem(uint32_t _start, uint32_t size)
{
	physaddr_t start, end;
	start = (physaddr_t) _start;
	end = start + (physaddr_t) size;
	printf(CON_INFO,"registering ram area 0x%x-0x%x\n", start, end);
	physmm_free_range( start, end );
}

void armv7_parse_atags( void * atag_addr )
{
	struct atag *params = (struct atag *) atag_addr;

	for (;;) {
		switch (params->hdr.tag) {

			case ATAG_CORE:
				printf(CON_DEBUG,"ATAG_CORE\n");
				printf(CON_DEBUG,"    flags=0x%x\n", params->u.core.flags);
				printf(CON_DEBUG,"    pagesize=0x%x\n", params->u.core.pagesize);
				printf(CON_DEBUG,"    rootdev=0x%x\n", params->u.core.rootdev);
				break;

			case ATAG_MEM:
				printf(CON_DEBUG,"ATAG_MEM\n");
				printf(CON_DEBUG,"    size=0x%x\n", params->u.mem.size);
				printf(CON_DEBUG,"    start=0x%x\n", params->u.mem.start);
				armv7_atag_mem(params->u.mem.start, params->u.mem.size);
				break;

			case ATAG_VIDEOTEXT:
				printf(CON_DEBUG,"ATAG_VIDEOTEXT\n");
				printf(CON_DEBUG,"    x=%x\n", (int)params->u.videotext.x);
				printf(CON_DEBUG,"    y=%x\n", (int)params->u.videotext.y);
				printf(CON_DEBUG,"    video_page=0x%x\n", (int)params->u.videotext.video_page);
				printf(CON_DEBUG,"    video_mode=0x%x\n", (int)params->u.videotext.video_mode);
				printf(CON_DEBUG,"    video_cols=0x%x\n", (int)params->u.videotext.video_cols);
				printf(CON_DEBUG,"    video_ega_bx=0x%x\n", (int)params->u.videotext.video_ega_bx);
				printf(CON_DEBUG,"    video_lines=0x%x\n", (int)params->u.videotext.video_lines);
				printf(CON_DEBUG,"    video_isvga=0x%x\n", (int)params->u.videotext.video_isvga);
				printf(CON_DEBUG,"    video_points=0x%x\n", (int)params->u.videotext.video_points);
				break;

			case ATAG_RAMDISK:
				printf(CON_DEBUG,"ATAG_RAMDISK\n");
				printf(CON_DEBUG,"    flags=0x%x\n", params->u.ramdisk.flags);
				printf(CON_DEBUG,"    size=0x%x\n", params->u.ramdisk.size);
				printf(CON_DEBUG,"    start=0x%x\n", params->u.ramdisk.start);
				break;

			case ATAG_INITRD2:
				printf(CON_DEBUG,"ATAG_INITRD2\n");
				printf(CON_DEBUG,"    start=0x%x\n", params->u.initrd2.start);
				printf(CON_DEBUG,"    size=0x%x\n", params->u.initrd2.size);
				armv7_atag_initrd_pa = params->u.initrd2.start;
				armv7_atag_initrd_sz = params->u.initrd2.size;
				break;

			case ATAG_SERIAL:
				printf(CON_DEBUG,"ATAG_SERIAL\n");
				printf(CON_DEBUG,"    low=0x%x\n", params->u.serialnr.low);
				printf(CON_DEBUG,"    high=0x%x\n", params->u.serialnr.high);
				break;

			case ATAG_REVISION:
				printf(CON_DEBUG,"ATAG_REVISION\n");
				printf(CON_DEBUG,"    rev=0x%x\n", params->u.revision.rev);
				break;

			case ATAG_VIDEOLFB:
				printf(CON_DEBUG,"ATAG_VIDEOLFB\n");
				printf(CON_DEBUG,"    lfb_width=0x%x\n", (int) params->u.videolfb.lfb_width);
				printf(CON_DEBUG,"    lfb_height=0x%x\n", (int) params->u.videolfb.lfb_height);
				printf(CON_DEBUG,"    lfb_depth=0x%x\n", (int) params->u.videolfb.lfb_depth);
				printf(CON_DEBUG,"    lfb_linelength=0x%x\n", (int) params->u.videolfb.lfb_linelength);
				printf(CON_DEBUG,"    lfb_base=0x%x\n", params->u.videolfb.lfb_base);
				printf(CON_DEBUG,"    lfb_size=0x%x\n", params->u.videolfb.lfb_size);
				printf(CON_DEBUG,"    red_size=0x%x\n", (int)params->u.videolfb.red_size);
				printf(CON_DEBUG,"    red_pos=0x%x\n", (int)params->u.videolfb.red_pos);
				printf(CON_DEBUG,"    green_size=0x%x\n", (int)params->u.videolfb.green_size);
				printf(CON_DEBUG,"    green_pos=0x%x\n", (int)params->u.videolfb.green_pos);
				printf(CON_DEBUG,"    blue_size=0x%x\n", (int)params->u.videolfb.blue_size);
				printf(CON_DEBUG,"    blue_pos=0x%x\n", (int)params->u.videolfb.blue_pos);
				printf(CON_DEBUG,"    rsvd_size=0x%x\n", (int)params->u.videolfb.rsvd_size);
				printf(CON_DEBUG,"    rsvd_pos=0x%x\n", (int)params->u.videolfb.rsvd_pos);
				break;

			case ATAG_CMDLINE:
				printf(CON_DEBUG,"ATAG_CMDLINE\n");
				printf(CON_DEBUG,"    cmdline=\"%s\"\n", params->u.cmdline.cmdline);
				armv7_atag_cmdline = params->u.cmdline.cmdline;
				break;

			case ATAG_NONE:
				printf(CON_DEBUG,"ATAG_NONE\n");
				return;
		}
		params = tag_next(params);
	}

}

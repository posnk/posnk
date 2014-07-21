/**
 * driver/video/mbfb.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#include <string.h>
#include <stdint.h>
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/physmm.h"
#include "driver/video/mbfb.h"

uint32_t *fb;

int mbfb_detect()
{
	uint32_t *legacy_fb = (uint32_t *) 0xA0000;
	uintptr_t mu,d,x,y;
	physaddr_t real_fb, t;

	fb = heapmm_alloc_alligned(MBFB_FB_SIZE, PHYSMM_PAGE_SIZE);
	if (!fb)
		return 0;

	legacy_fb[0] = MBFB_SEARCH_MAGIC_1;
	legacy_fb[1] = MBFB_SEARCH_MAGIC_2;

	mu = (uintptr_t) fb;
		
	for (d = 0; d < MBFB_FB_SIZE; d += 0x1000) {
		physmm_free_frame(paging_get_physical_address((void *)(mu + d)));
		paging_unmap((void *)(mu + d));
	}

	real_fb = 0;

	for (t = 0xE0000000; t < 0xFF000000; t += 0x1000) {
		paging_map(fb, t, PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW);
		if ((fb[0] == MBFB_SEARCH_MAGIC_1) && (fb[1] == MBFB_SEARCH_MAGIC_2)) {
			debugcon_printf("Found the lfb at %x\n", t);
			real_fb = t;
			break;
		}
		paging_unmap(fb);
	}

	if (!real_fb) {
		for (d = 0; d < MBFB_FB_SIZE; d += 0x1000) {
			t = physmm_alloc_frame();
			if (!t)
				return 0;
			paging_map((void *)(mu + d), t, PAGING_PAGE_FLAG_RW);
		}
		heapmm_free(fb, MBFB_FB_SIZE);
		return 0;
	}
	
	for (d = 0x1000; d < MBFB_FB_SIZE; d += 0x1000) {

		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW);
	}
	return 1;
}

void mbfb_init()
{
	mbfb_detect();
}

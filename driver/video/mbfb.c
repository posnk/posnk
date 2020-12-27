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
#include <sys/errno.h>
#include <stdint.h>
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/physmm.h"
#include "kernel/device.h"
#include "kernel/syscall.h"
#include "driver/video/mbfb.h"
#include "driver/video/fb.h"
#include "arch/i386/vbe.h"

fb_device_info_t mbfb_device_info;
fb_mode_info_t mbfb_mode_info;
physaddr_t real_fb;

extern vbe_mode_info_t	  vbe_mode;

uint32_t *mbfb_maplfb()
{
	uint32_t *fb;
	uintptr_t mu,d;

	fb = heapmm_alloc_alligned(mbfb_device_info.fb_size, PHYSMM_PAGE_SIZE);
	if (!fb)
		return NULL;

	mu = (uintptr_t) fb;

	for (d = 0; d < mbfb_device_info.fb_size; d += 0x1000) {
		physmm_free_frame(paging_get_physical_address((void *)(mu + d)));
		paging_unmap((void *)(mu + d));
	}

	for (d = 0; d < mbfb_device_info.fb_size; d += 0x1000) {

		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW);
	}
	return fb;
}

uint32_t *mbfb_detect()
{
	uint32_t *fb;
	uint32_t *legacy_fb = (uint32_t *) 0xA0000;
	uintptr_t mu,d;
	physaddr_t t;

	fb = heapmm_alloc_alligned(mbfb_device_info.fb_size, PHYSMM_PAGE_SIZE);
	if (!fb)
		return NULL;

	legacy_fb[0] = MBFB_SEARCH_MAGIC_1;
	legacy_fb[1] = MBFB_SEARCH_MAGIC_2;

	mu = (uintptr_t) fb;

	for (d = 0; d < mbfb_device_info.fb_size; d += 0x1000) {
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
		for (d = 0; d < mbfb_device_info.fb_size; d += 0x1000) {
			t = physmm_alloc_frame();
			if (!t)
				return 0;
			paging_map((void *)(mu + d), t, PAGING_PAGE_FLAG_RW);
		}
		heapmm_free(fb, mbfb_device_info.fb_size);
		return NULL;
	}

	for (d = 0x1000; d < mbfb_device_info.fb_size; d += 0x1000) {

		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW);
	}
	return fb;
}

int mbfb_open(  __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options)
{
	return 0;
}

int mbfb_close( __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd)
{
	return 0;
}

int mbfb_write(	__attribute__((__unused__)) dev_t device,
		__attribute__((__unused__)) void *buf,
		__attribute__((__unused__)) aoff_t count,
		__attribute__((__unused__)) aoff_t *write_size,
		__attribute__((__unused__)) int non_block)
{
	return ENXIO;
}

int mbfb_read(	__attribute__((__unused__)) dev_t device,
		__attribute__((__unused__)) void *buf,
		__attribute__((__unused__)) aoff_t count,
		__attribute__((__unused__)) aoff_t *read_size,
		__attribute__((__unused__)) int non_block)
{
	return ENXIO;
}

int mbfb_ioctl(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, int func, __attribute__((__unused__)) int arg)
{
	switch(func) {

		case IOCTL_FBGDINFO:
			if (!copy_kern_to_user(&mbfb_device_info, (void *) arg, sizeof(fb_device_info_t))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;

		case IOCTL_FBGMINFO:
			if (!copy_kern_to_user(&mbfb_mode_info, (void *) arg, sizeof(fb_mode_info_t))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;

		case IOCTL_FBSMINFO:
			return EINVAL;

		default:
			return 0;

	}
}

int mbfb_mmap(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int flags, void *addr, aoff_t offset, aoff_t size)
{
	uintptr_t d, mu;
	offset &= ~PHYSMM_PAGE_ADDRESS_MASK;
	if (offset >= MBFB_FB_SIZE)
		return EINVAL;
	if ((offset + size) > MBFB_FB_SIZE)
		size = MBFB_FB_SIZE - offset;
	mu = (uintptr_t) addr;
	for (d = 0; d < size; d += 0x1000) {
		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d + offset,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
	}
	return 0;
}

int mbfb_unmmap(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int flags, void *addr, __attribute__((__unused__)) aoff_t offset, aoff_t size)
{
	uintptr_t d, mu;
	mu = (uintptr_t) addr;
	for (d = 0; d < size; d += 0x1000) {
		paging_unmap((void *)(mu + d));
	}
	return 0;
}

char_ops_t mbfb_ops = {
	.open = mbfb_open,
	.close = mbfb_close,
	.write = mbfb_write,
	.read = mbfb_read,
	.ioctl = mbfb_ioctl,
	.mmap = mbfb_mmap,
	.unmmap = mbfb_unmmap
};

char_dev_t mbfb_desc = {
	"multiboot framebuffer",
	23,
	&mbfb_ops
};

void mbfb_init()
{
	if (!vbe_mode.Xres) {
		mbfb_mode_info.fb_width = 1024;
		mbfb_mode_info.fb_height = 768;
		mbfb_mode_info.fb_stride = 1024 * 4;
		mbfb_mode_info.fb_bpp = 24;
		mbfb_device_info.fb_size = MBFB_FB_SIZE;
		fb_primary_lfb = mbfb_detect();
	} else {
		mbfb_mode_info.fb_width = vbe_mode.Xres;
		mbfb_mode_info.fb_height = vbe_mode.Yres;
		mbfb_mode_info.fb_stride = vbe_mode.pitch;
		mbfb_mode_info.fb_bpp = vbe_mode.bpp;
		mbfb_device_info.fb_size = mbfb_mode_info.fb_stride * vbe_mode.Yres;
		real_fb = vbe_mode.physbase;
		fb_primary_lfb = mbfb_maplfb();
	}
	fb_primary_device_info = &mbfb_device_info;
	fb_primary_mode_info = &mbfb_mode_info;
	device_char_register(&mbfb_desc);
}

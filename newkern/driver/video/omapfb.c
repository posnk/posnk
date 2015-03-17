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
#include "driver/video/omapfb.h"
#include "driver/video/fb.h"

fb_device_info_t	omapfb_device_info;
fb_mode_info_t		omapfb_mode_info;
physaddr_t		real_fb;

extern vbe_mode_info_t	vbe_mode;

uint32_t *omapfb_maplfb()
{
	uint32_t *fb;
	uintptr_t mu,d;

	fb = heapmm_alloc_alligned(omapfb_device_info.fb_size, PHYSMM_PAGE_SIZE);

	if (!fb)
		return NULL;

	mu = (uintptr_t) fb;
		
	for (d = 0; d < omapfb_device_info.fb_size; d += 0x1000) {
		physmm_free_frame(paging_get_physical_address((void *)(mu + d)));
		paging_unmap((void *)(mu + d));
	}
	
	for (d = 0; d < omapfb_device_info.fb_size; d += 0x1000) {

		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW);
	}
	return fb;
}

int omapfb_open(  __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options)
{
	return 0;	
}

int omapfb_close( __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd)
{
	return 0;
}

int omapfb_write(	__attribute__((__unused__)) dev_t device, 
			__attribute__((__unused__)) void *buf, 
			__attribute__((__unused__)) aoff_t count,
			__attribute__((__unused__)) aoff_t *write_size,
			__attribute__((__unused__)) int non_block)
{
	return ENXIO;
}

int omapfb_read(	__attribute__((__unused__)) dev_t device, 
			__attribute__((__unused__)) void *buf, 
			__attribute__((__unused__)) aoff_t count,
			__attribute__((__unused__)) aoff_t *read_size,
			__attribute__((__unused__)) int non_block)
{
	return ENXIO;
}

int omapfb_ioctl(	__attribute__((__unused__)) dev_t device, 
			__attribute__((__unused__)) int fd, 
			int func, 
			__attribute__((__unused__)) int arg)
{
	switch(func) {	

		case IOCTL_FBGDINFO:
			if (!copy_kern_to_user(&omapfb_device_info, 
					(void *) arg, 
					sizeof(fb_device_info_t))){
				syscall_errno = EFAULT;
				return -1;
			}		
			return 0;

		case IOCTL_FBGMINFO:
			if (!copy_kern_to_user(&omapfb_mode_info, 
					(void *) arg, 
					sizeof(fb_mode_info_t))){
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

int omapfb_mmap(	__attribute__((__unused__)) dev_t device, 
			__attribute__((__unused__)) int fd, 
			__attribute__((__unused__)) int flags, 
			void *addr, 
			aoff_t offset, 
			aoff_t size)
{	
	uintptr_t d, mu;
	offset &= ~PHYSMM_PAGE_ADDRESS_MASK;
	if (offset >= OMAPFB_FB_SIZE)
		return EINVAL;
	if ((offset + size) > OMAPFB_FB_SIZE)
		size = OMAPFB_FB_SIZE - offset;
	mu = (uintptr_t) addr;
	for (d = 0; d < size; d += 0x1000) {
		paging_map(	 (void *)(mu + d),
			  	 real_fb + (physaddr_t) d + offset,
				 PAGING_PAGE_FLAG_NOCACHE | PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
	}
	return 0;
}

int omapfb_unmmap(
			__attribute__((__unused__)) dev_t device, 
			__attribute__((__unused__)) int fd, 
			__attribute__((__unused__)) int flags, 
			void *addr, 
			__attribute__((__unused__)) aoff_t offset, 
			aoff_t size)
{
	uintptr_t d, mu;
	mu = (uintptr_t) addr;
	for (d = 0; d < size; d += 0x1000) {
		paging_unmap((void *)(mu + d));
	}
	return 0;
}

tty_ops_t omapfb_ops = {
	&omapfb_open,
	&omapfb_close,
	&omapfb_write,
	&omapfb_read,
	&omapfb_ioctl,
	&omapfb_mmap,
	&omapfb_unmmap
};

char_dev_t omapfb_desc = {
	"omap framebuffer",
	23,
	&omapfb_ops
};

void omapfb_init()
{	
	omapfb_mode_info.fb_width  = 800;
	omapfb_mode_info.fb_height = 480;
	omapfb_mode_info.fb_stride = 480 * 2;
	omapfb_mode_info.fb_bpp    = 16;
	omapfb_device_info.fb_size = 0xbb800;
	real_fb 		   = 0x8f9c0000;
	fb_primary_lfb = omapfb_maplfb();
	fb_primary_device_info = &omapfb_device_info;
	fb_primary_mode_info = &omapfb_mode_info;
	device_char_register(&omapfb_desc);
}

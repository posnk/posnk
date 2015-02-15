#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>	
#include "kernel/device.h"


FILE    *bdev;
aoff_t   bsz;

void device_block_stop(){
	fclose(bdev);
}

void device_block_init(char *image)
{
	bdev = fopen(image, "r+");
	if (!bdev) {
		fprintf(stderr, "Could not open block device image %s : %s\n", image, strerror(errno));	
		abort();
	}
	if (fseek(bdev, 0L, SEEK_END)) {
		fprintf(stderr, "Could not seek block device image %s : %s\n", image, strerror(errno));	
		abort();
	}
	bsz = (aoff_t) ftell(bdev);
	rewind(bdev);
}

int device_block_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size)
{
	if ((file_offset > bsz) || ((file_offset + count) > bsz)) {
		fprintf(stderr, "error: tried to write past end of image\n");	
		abort();
	}
	if (fseek(bdev, (long) file_offset, SEEK_SET)) {
		fprintf(stderr, "Could not seek block device image: %s\n", strerror(errno));	
		abort();
	}
	if (fwrite(buffer, count, 1, bdev) != 1) {
		fprintf(stderr, "Could not write to block device image: %s\n", strerror(errno));	
		abort();
	}
	*write_size = count;
	return 0;
	
}

int device_block_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size)
{
	if ((file_offset > bsz) || ((file_offset + count) > bsz)) {
		fprintf(stderr, "error: tried to read past end of image\n");	
		abort();
	}
	if (fseek(bdev, (long) file_offset, SEEK_SET)) {
		fprintf(stderr, "Could not seek block device image: %s\n", strerror(errno));	
		abort();
	}
	if (fread(buffer, count, 1, bdev) != 1) {
		fprintf(stderr, "Could not read from block device image: %s\n", strerror(errno));	
		abort();
	}
	*read_size = count;
	return 0;
}

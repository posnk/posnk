#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
uint32_t *fb;

int main( int argc, const char* argv[] ) {
	int x, y;
	int fd = open("/dev/fb0", O_RDWR | O_NOCTTY);
	if (fd == -1) {
		printf("Couldnt open framebuffer!\n");
		return 255;
	}
	fb = mmap(MAP_PRIVATE, 0x300000, 0, PROT_READ | PROT_WRITE, fd, 0);
	if (!fb) {
		printf("Couldn't map framebuffer!\n");	
		return 255;
	}
	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++) {
			fb[x + y * 1024] = (x << 16) | y;
		}
	return 0;
}

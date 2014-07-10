#include <stdio.h>
#include <stdint.h>

#define SYS_MOUNT	59

uint32_t nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4]);

int mount(char *dev, char *mp, char *fs, uint32_t flags)
{
   int a[] = {(uint32_t) dev, (uint32_t) mp, (uint32_t) fs, flags};
   int b[] = {(uint32_t) 1+strlen(dev), 1+strlen(mp), 1+strlen(fs),0};
   return (int) nk_do_syscall(SYS_MOUNT, a, b); 
}

int main( int argc, const char* argv[] ) {
	if (argc != 4)
		return 0;
	return mount(argv[1], argv[2], argv[3], 0);
}

#include <stdlib.h>

void *heapmm_alloc(size_t size)
{
	return malloc(size);
}

void  heapmm_free(void *addr, size_t size)
{
	free(addr);
}

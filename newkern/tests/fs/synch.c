#include <stdlib.h>
#include <stdio.h>
#include "kernel/synch.h"

semaphore_t *semaphore_alloc()
{
	semaphore_t *sem = malloc(sizeof(semaphore_t));
	*sem = 0;
	return sem;
}

void  semaphore_up(semaphore_t *sem)
{
	(*sem)++;
}

int   semaphore_down(semaphore_t *sem)
{
	if (0 == *sem) {
		fprintf(stderr, "error: driver would block\n");
		abort();	
	}
	(*sem)--;
	return 0;
}

#include <stdio.h>
#include <assert.h>
#define ESUCCESS 0
#include "crt/sys/error.h"

SFUNC(char *, testerror, int arg1, int arg2)
{
	if (arg1)
		THROW(123, NULL);
	if (arg2)
		RETURN("A");
	else
		RETURN("B");
}

SFUNC(char *, testcret, int arg3, int arg4)
{
	CHAINRET(testcret, arg3, arg4);
}

void main()
{
	char *res;
	assert ( testerror(1, 1, &res) == 123 );
	assert ( testerror(0, 1, &res) == ESUCCESS );
	assert ( res[0] == 'A');
	assert ( testerror(0, 0, &res) == ESUCCESS );
	assert ( res[0] == 'B');
}

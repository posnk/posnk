#include "config.h"
#include <sys/utsname.h>
#include "kernel/syscall.h"
#include "kernel/scheduler.h"
#include <string.h>

const char *posnk_version = __DATE__ " " __TIME__ " " BUILD_MACHINE;
const char *posnk_release = "git master";
const char *posnk_machine = ARCH_NAME;
const char *posnk_sysname = "posnk";
const char *posnk_nodename = "";

uint32_t sys_uname(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	struct utsname *out;

	out = ( struct utsname * ) a;

	if (!procvmm_check( out, sizeof( struct utsname ) )) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}

	strcpy( out->version, posnk_version );
	strcpy( out->release, posnk_release );
	strcpy( out->machine, posnk_machine );
	strcpy( out->sysname, posnk_sysname );
	strcpy( out->nodename, posnk_nodename );

	return 0;
}

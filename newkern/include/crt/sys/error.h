/**
 * @file crt/sys/error.h
 *
 * Implements the error passing mechanism
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 16-02-2015 - Created
 */

#ifndef __SYS_ERROR_H__
#define __SYS_ERROR_H__

#define RETURN_VALUE(TyPe)	TyPe* _retval
#define RETURN(VaLuE)		do { *_retval = (VaLuE); return ESUCCESS; } while (0);
#define THROW(StAtUs, VaLuE)	do { *_retval = (VaLuE); return <StAtUs>; } while (0);
#define CHAINRET(FuNc, ...)	do { return FuNc(__VA_ARGS__, _retval); } while (0);

#endif

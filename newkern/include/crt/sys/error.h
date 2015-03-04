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

typedef int errno_t;

#define RETURN_VALUE(TyPe)	TyPe* _retval
#define RETURN(VaLuE)		do { *_retval = (VaLuE); return ESUCCESS; } while (0)
#define RETURNV			do { return ESUCCESS; } while (0)
#define THROW(StAtUs, VaLuE)	do { *_retval = (VaLuE); return StAtUs; } while (0)
#define THROWV(StAtUs)		do { return StAtUs; } while (0)
#define CHAINRET(FuNc, ...)	do { return FuNc(__VA_ARGS__, _retval); } while (0)
#define CHAINRETV(FuNc, ...)	do { return FuNc(__VA_ARGS__); } while (0)
#define SFUNC(TyPe, NaMe, ...)  errno_t NaMe(__VA_ARGS__, RETURN_VALUE(TyPe))
#define SVFUNC(NaMe, ...)  	errno_t NaMe(__VA_ARGS__)
#define SFUNCPTR(TyPe, NaMe, ...)  errno_t (*NaMe)(__VA_ARGS__, RETURN_VALUE(TyPe))
#define SVFUNCPTR(NaMe, ...)  	errno_t (*NaMe)(__VA_ARGS__)

#endif

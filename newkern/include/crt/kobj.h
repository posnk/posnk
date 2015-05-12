/**
 * @file crt/kobj.h
 *
 * Implements pseudo-oop programming
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-05-2015 - Created
 */

#ifndef __KOBJ_H__
#define __KOBJ_H__

#include <sys/error.h>

errno_t kobj_handle_pure_call(	const char *	file,
				int		line,
				const char *	obj_expr,
				const char *	mthd_name );

#define SMCALL(ObJ, RetVal, FuNc, ...)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ, __VA_ARGS__, RetVal)) );
	
#define SVMCALL(ObJ, FuNc, ...)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ, __VA_ARGS__)) );
	
#define CHAINRETM(ObJ, FuNc, ...)	do { \
	return SNCALL(ObJ, _retval, FuNc, __VA_ARGS__); \
	} while (0)

#define CHAINRETMV(ObJ, FuNc, ...)	do { \
	return SVMCALL(ObJ, FuNc, __VA_ARGS__); \
	} while (0)

#define SMIMPL(TyPe, ClAsS, NaMe, ...) 	errno_t NaMe(	ClAsS *_this, \
							__VA_ARGS__, \
							RETURN_VALUE(TyPe))

#define SVMIMPL(ClAsS, NaMe, ...)  	errno_t NaMe(	ClAsS *_this, __VA_ARGS__ )

#define SMDECL(TyPe, ClAsS, NaMe, ...)  errno_t (*NaMe)( ClAsS *, __VA_ARGS__, RETURN_VALUE(TyPe))

#define SVMDECL(ClAsS, NaMe, ...)  	errno_t (*NaMe)(ClAsS *, __VA_ARGS__)

#endif

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

#include <sys/errno.h>

#ifdef PRODUCE_SOOC

errno_t kobj_handle_pure_call(	const char *	file,
				int		line,
				const char *	obj_expr,
				const char *	mthd_name );

#define class_decl(ClAsS)	typedef struct ClAsS ## _tag ClAsS

#define class_defn(ClAsS)	typedef struct ClAsS ## _tag ClAsS;\
				typedef struct ClAsS ## _vtab 

#define method_end(ClAsS)	} ClAsS ## _vtab_t; \
				struct ClAsS ## _tag { \
					ClAsS ## _vtab_t *_vtab

#define method_end_o(ClAsS, OveR)	} ClAsS ## _vtab_t; \
				struct ClAsS ## _tag { \
					OveR	_override; \
					ClAsS ## _vtab_t *_vtab

#define class_impl(ClAsS, NaMe) ClAsS ## _vtab_t NaMe ## _vtab = 
#define method_impl(MeThD, ImPl)	.MeThD = ImPl

#define class_init(ObJ, ImPl)	do { \
					(ObJ)->_vtab = &ImPl ## _vtab; \
				} while (0);

#define SMCALL(ObJ, RetVal, FuNc, ...)  ObJ:>FuNc(__VA_ARGS__) @> RetVal

#define SNMCALL(ObJ, RetVal, FuNc)  ObJ:>FuNc() @> RetVal;
	
#define SVMCALL(ObJ, FuNc, ...)  ObJ:>FuNc(__VA_ARGS__);
	
#define SOMCALL(ObJ, FuNc)  ObJ:>FuNc();
	
#define CHAINRETM(ObJ, FuNc, ...)	do { \
	return SNCALL(ObJ, _retval, FuNc, __VA_ARGS__); \
	} while (0)

#define CHAINRETMV(ObJ, FuNc, ...)	do { \
	return SVMCALL(ObJ, FuNc, __VA_ARGS__); \
	} while (0)

#define CHAINRETMO(ObJ, FuNc)	do { \
	return SOMCALL(ObJ, FuNc); \
	} while (0)

#define CHAINRETMN(ObJ, FuNc)	do { \
	return SNMCALL(ObJ, _retval, FuNc); \
	} while (0)

#define SMIMPL(TyPe, ClAsS, NaMe, ...) 	errno_t NaMe(	ClAsS *_this, \
							__VA_ARGS__, \
							RETURN_VALUE(TyPe))

#define SNMIMPL(TyPe, ClAsS, NaMe) 		errno_t NaMe(	ClAsS *_this, \
							RETURN_VALUE(TyPe))

#define SVMIMPL(ClAsS, NaMe, ...)  		errno_t NaMe(	ClAsS *_this, __VA_ARGS__ )

#define SOMIMPL(ClAsS, NaMe)  			errno_t NaMe(	ClAsS *_this )

#define SMDECL(TyPe, ClAsS, NaMe, ...)  errno_t (*NaMe)( ClAsS *, __VA_ARGS__,\
															RETURN_VALUE(TyPe))

#define SNMDECL(TyPe, ClAsS, NaMe)  	errno_t (*NaMe)( ClAsS *, \
															RETURN_VALUE(TyPe))

#define SVMDECL(ClAsS, NaMe, ...)  		errno_t (*NaMe)(ClAsS *, __VA_ARGS__)

#define SOMDECL(ClAsS, NaMe)  			errno_t (*NaMe)(ClAsS *)

#else

errno_t kobj_handle_pure_call(	const char *	file,
				int		line,
				const char *	obj_expr,
				const char *	mthd_name );

#define class_decl(ClAsS)	typedef struct ClAsS ## _tag ClAsS

#define class_defn(ClAsS)	typedef struct ClAsS ## _tag ClAsS;\
				typedef struct ClAsS ## _vtab 

#define method_end(ClAsS)	} ClAsS ## _vtab_t; \
				struct ClAsS ## _tag { \
					ClAsS ## _vtab_t *_vtab

#define method_end_o(ClAsS, OveR)	} ClAsS ## _vtab_t; \
				struct ClAsS ## _tag { \
					OveR	_override; \
					ClAsS ## _vtab_t *_vtab

#define class_impl(ClAsS, NaMe) ClAsS ## _vtab_t NaMe ## _vtab = 
#define method_impl(MeThD, ImPl)	.MeThD = ImPl

#define class_init(ObJ, ImPl)	do { \
					(ObJ)->_vtab = &ImPl ## _vtab; \
				} while (0);

#define SMCALL(ObJ, RetVal, FuNc, ...)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ, __VA_ARGS__, RetVal)) )

#define SNMCALL(ObJ, RetVal, FuNc)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ, RetVal)) )
	
#define SVMCALL(ObJ, FuNc, ...)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ, __VA_ARGS__)) )
	
#define SOMCALL(ObJ, FuNc)  ( ((ObJ)->_vtab->FuNc == 0) ? \
	(kobj_handle_pure_call(__FILE__, __LINE__, #ObJ, #FuNc)) : \
	((ObJ)->_vtab->FuNc(ObJ)) )
	
#define CHAINRETM(ObJ, FuNc, ...)	do { \
	return SNCALL(ObJ, _retval, FuNc, __VA_ARGS__); \
	} while (0)

#define CHAINRETMV(ObJ, FuNc, ...)	do { \
	return SVMCALL(ObJ, FuNc, __VA_ARGS__); \
	} while (0)

#define CHAINRETMO(ObJ, FuNc)	do { \
	return SOMCALL(ObJ, FuNc); \
	} while (0)

#define CHAINRETMN(ObJ, FuNc)	do { \
	return SNMCALL(ObJ, _retval, FuNc); \
	} while (0)

#define SMIMPL(TyPe, ClAsS, NaMe, ...) 	errno_t NaMe(	ClAsS *_this, \
							__VA_ARGS__, \
							RETURN_VALUE(TyPe))

#define SNMIMPL(TyPe, ClAsS, NaMe) 		errno_t NaMe(	ClAsS *_this, \
							RETURN_VALUE(TyPe))

#define SVMIMPL(ClAsS, NaMe, ...)  		errno_t NaMe(	ClAsS *_this, __VA_ARGS__ )

#define SOMIMPL(ClAsS, NaMe)  			errno_t NaMe(	ClAsS *_this )

#define SMDECL(TyPe, ClAsS, NaMe, ...)  errno_t (*NaMe)( ClAsS *, __VA_ARGS__,\
															RETURN_VALUE(TyPe))

#define SNMDECL(TyPe, ClAsS, NaMe)  	errno_t (*NaMe)( ClAsS *, \
															RETURN_VALUE(TyPe))

#define SVMDECL(ClAsS, NaMe, ...)  		errno_t (*NaMe)(ClAsS *, __VA_ARGS__)

#define SOMDECL(ClAsS, NaMe)  			errno_t (*NaMe)(ClAsS *)

#endif

#endif

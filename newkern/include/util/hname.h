/**
 * @file util/hname.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * @brief Implements a safe hash-accellerated name type
 * 
 * Changelog:
 * 27-07-2015 - Created
 */
 
#ifndef	__util_hname_h__
#define __util_hname_h__

#include <stdint.h>

typedef	struct {
	uint32_t	 hash;
	size_t		 length;
	char		*text;
} hname_t;
 
uint32_t	 hname_hash_string( const char *string );
hname_t		*hname_create ( const char *string );
void		 hname_createtmp ( hname_t	*name, const char *string );
int			 hname_compare ( hname_t *a, hname_t *b );

#endif

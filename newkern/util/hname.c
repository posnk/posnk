/**
 * @file util/hname.c
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
 
 #include "util/hname.h"
 #include "kernel/heapmm.h"
 #include <assert.h>
 #include <string.h>
 #include <stdint.h>
 
 /**
  * Generates a hash for a string
  * @param	string	The string to hash
  * @return			The hash of string
  */
  
uint32_t	hname_hash_string( const char *string )
{
	uint32_t	work;
	 
	for ( work = 0; *string != 0; string++ )
		work = work * 37 + string;
	
	return work;
}

 /**
  * Creates a hashname for a string
  * @param	string	The string to hash
  * @return			The hashname
  */

hname_t		*hname_create ( const char *string )
{
	//TODO: Optimize this stuff
	size_t	length;	
	hname_t	*name;
	
	assert ( string != NULL );
	
	length	= strlen( string );
	name	= heapmm_alloc ( sizeof ( hname_t ) + 1 + length );
	
	if ( name == NULL )
		return NULL;
	
	name->text		= &name[1];
	
	strcpy( name->text, string );
	
	name->length	= length;
	name->hash		= hname_hash_string( string );
	
	return name;
	
}

 /**
  * Creates a temporary hashname
  * @param	name	The hashname target
  * @param	string	The string to hash
  */

void		 hname_createtmp ( hname_t	*name, const char *string )
{
	//TODO: Optimize this stuff
	
	assert ( name != NULL );	
	assert ( string != NULL );
	
	name->text		= string;
	name->length	= strlen( string );
	name->hash		= hname_hash_string( string );
}

 /**
  * Destroys a hashname
  * @param	name	The name to destroy
  */

void		 hname_destroy ( hname_t *name )
{
	
	assert ( name != NULL );
	
	heapmm_free ( name, sizeof( hname_t ) + 1 + name->length );
	
}

/**
 * Compares two hashnames
 * @return	If the hashnames matched
 */
int			 hname_compare ( hname_t *a, hname_t *b )
{
	assert ( a != NULL );
	assert ( b != NULL );
	
	if ( a == b )
		return 1;
	
	if ( a->hash != b->hash )
		return 0;
		
	if ( a->length != b->length )
		return 0;
		
	return strcmp( a->text, b->text ) == 0;
}


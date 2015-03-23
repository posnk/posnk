#include <string.h>
#include <stdint.h>
#include "config.h"

/* POSIX PASS */
char * strrchr( const char * str , int c )
{
	size_t pos = strlen( str );
	if ( c == 0 )
		return (char *) &(str[ pos ]);
	while ( pos != 0 )
		if ( str[ --pos ] == ((char) c) )
			return (char *) &(str[ pos ]);
	return NULL;
}

/* POSIX PASS */
char * strchrnul( const char * str , int c )
{
	size_t pos = 0;
	while ( str[ pos ] )
		if ( str[ pos++ ] == ((char) c) )
			return (char *) &(str[ pos - 1 ]);
	return (char *) &(str[ pos ]);
}
 
#ifndef CONFIG_FASTCRT

/* POSIX PASS */
char * strchr( const char * str , int c )
{
	size_t pos = 0;
	while ( str[ pos ] )
		if ( str[ pos++ ] == ((char) c) )
			return (char *) &(str[ pos - 1 ]);
	if ( c == 0 )
		return (char *) &(str[ pos ]);
	else
		return NULL;
}

size_t strlen ( const char* str )
{
	size_t	len=0;
	while ( str[len++] ); /* careful! MSVC++ optimization might embed call to strlen()*/
	return len - 1;
}

void * memset ( void * ptr, int value, size_t num )
{
	uint32_t  expval,nint, *pint;
	char *p = (char *) ptr;
	unsigned char val = (unsigned char) value;
	size_t i;
	expval = ((value & 0xFF) << 24) | 
		 ((value & 0xFF) << 16) | 
		 ((value & 0xFF) << 8)  | 
	           (value &0xFF);
	nint = num >> 2;
	num = num - (nint << 2);
	pint = (uint32_t *) ptr;
	for (i = 0;i < nint; i++)
		*pint++ = expval;
	p = (char *) pint;
	for (i = 0;i < num;i++)
		*p++ = val;
	return ptr;
}

void * memcpy ( void * destination, const void * source, size_t num )
{
	char *p = (char *) destination;
	char *s = (char *) source;
	size_t i;
	uint32_t nint, *dint, *sint;
	nint = num >> 2;
	num = num - (nint << 2);
	dint = (uint32_t *) destination;
	sint = (uint32_t *) source;
	for (i = 0;i < nint; i++)
		*dint++ = *sint++;
	p = (char *) dint;
	s = (char *) sint;
	for (i = 0;i < num;i++)
		*p++ = *s++;
	return destination;
}
#endif
char * strcpy ( char * destination, const char * source )
{
	return (char *) memcpy(destination, source, strlen(source) + 1);
}

char * strncpy(char *dest, const char *src, size_t n)
{
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];
	for ( ; i < n; i++)
		dest[i] = '\0';

	return dest;
}


void *memcpy_o(void *dst_ptr, const void *src_ptr, size_t count,void * buf)
{
	memcpy(buf,src_ptr,count);
	return memcpy(dst_ptr,buf,count);
}

int strcmp ( const char * str1, const char * str2 )
{
	size_t pos = 0;
	size_t len = strlen(str1);
	if (len != strlen(str2))
		return 1;
	for (pos = 0;pos < len;pos++)
		if (str1[pos] != str2[pos]){
			return 1;
		}
	return 0;
}

int strncmp ( const char * str1, const char * str2, size_t num)
{
	size_t pos = 0;
	if (strlen(str1) < num)
		return 1;
	if (strlen(str2) < num)
		return 1;
	for (pos = 0;pos < num;pos++)
		if (str1[pos] != str2[pos])
			return 1;
	return 0;
}

int startswith ( const char * str1, const char * str2){
	return strncmp(str1,str2,strlen(str2)-1);
}

char *strcat(char *s, const char *append)
{
	char *save = s;

	for (; *s; ++s);
	while ((*s++ = *append++) != 0);
	return(save);
}


char * strtok(char *s, const char *delim)
{
	char *spanp;
	int c, sc;
	char *tok;
	static char *last;


	if (s == 0 && (s = last) == 0)
		return (0);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = 0;
		return (0);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = 0;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

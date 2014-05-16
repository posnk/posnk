#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

/* POSIX PASS */
size_t strlen ( const char* str );

/* POSIX PASS */
void * memset ( void * ptr, int value, size_t num );

/* POSIX PASS */
void * memcpy ( void * destination, const void * source, size_t num );

/* POSIX PASS */
char * strcpy ( char * destination, const char * source );

/* POSIX PASS */
char * strncpy(char *dest, const char *src, size_t n);

/* NONPOSIX */
void * memcpy_o(void *dst_ptr, const void *src_ptr, size_t count,void * buf);

/* NONPOSIX */
int startswith ( const char * str1, const char * str2);

/* POSIX PASS */
int strcmp ( const char * str1, const char * str2);

/* POSIX PASS */
int strncmp ( const char * str1, const char * str2, size_t num);

/* POSIX PASS */
char * strtok(char *s, const char *delim);

/* POSIX PASS */
char * strcat(char *s, const char *append);

/* POSIX PASS */
char * strchr( const char * str , int c );

/* POSIX PASS */
char * strrchr( const char * str , int c );

/* POSIX PASS */
char * strchrnul( const char * str , int c );
#endif

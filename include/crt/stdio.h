/******************************************************************************\
Copyright (C) 2015 Peter Bosch
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file include/crt/stdio.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */
#ifndef __stdio__

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(	char *str, size_t size, const char *format, va_list list );

int vsprintf(	char *str, const char *format, va_list list );

int sprintf(	char *str, const char *format, ... );

int snprintf(	char *str, size_t size, const char *format, ... );

#endif

/**
 * @file driver/platform/platform.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch
 * 
 * Changelog:
 * \li 25-03-2015 - Created
 */

#ifndef __driver_platform_platform_h__
#define __driver_platform_platform_h__

void	platform_initialize( void );

int	platform_get_interrupt_id ( int int_channel );

void	platform_end_of_interrupt ( int int_channel, int int_id );

#endif

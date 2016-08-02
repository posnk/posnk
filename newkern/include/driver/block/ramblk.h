/**
 * @file driver/block/ramblk.h
 *
 * Implements a RAM backed block device
 *
 * Part of P-OS driver library.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 18-07-2016 - Created
 */
#ifndef __ramblk_h__
#define __ramblk_h__

#include "config.h"
#include <sys/errno.h>

#include "kernel/earlycon.h"
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"

void ramblk_register( int num, aoff_t size, void * data );

#endif

/******************************************************************************\
Copyright (C) 2017 Peter Bosch

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
 * @file kernel/net/pbuf.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef NET_PBUF_H
#define NET_PBUF_H

#include <stdint.h>
#include <sys/types.h>

/**
 * Packet buffer is physically contiguous
 */
#define PBUF_CONTIG		(1<<0)
/**
 * Packet buffer is physically contiguous
 */
#define PBUF_FREE		(1<<0)

/**
 * Packet buffer type definition
 * @see struct pbuf
 */
typedef struct pbuf pbuf_t;

/**
 * Packet buffer cluster type definition
 * @see struct pcluster
 */
typedef struct pcluster pcluster_t;

/**
 * Packet buffer cluster
 */
struct pcluster {

	/**
	 * The next cluster in the cluster list
	 */
	struct pcluster *next;
	
	/**
	 * The number of buffers in the cluster 
	 */
	int              nbufs;
	
	/**
	 * The number of free buffers in the cluster
	 */
	int              nfree;
	
	/**
	 * The packet buffer headers
	 */
	struct pbuf     *bufs;
	
};

/**
 * Packet buffer structure
 */
struct pbuf {
	
	llist_t       node;

	/**
	 * Packet flags
	 */
	int           flags;
	
	/**
	 * Next packet in the queue
	 */
	struct pbuf  *qnext;

	/**
	 * First fragment of this packet
	 */
	struct pbuf	 *ffrag;
	
	/**
	 * Next fragment of this packet
	 */
	struct pbuf  *nfrag;

	/**
	 * The size of the buffer payload
	 */
	size_t        datasz;

    /**
     * The payload pointer
     */
    uint8_t      *data;

	/**
	 * The raw size of the buffer
	 */
    size_t        rawsz;
    
    /**
     * The buffer pointer
     */
	uint8_t      *buf;
	
	/**
	 * The containing cluster ( may be NULL )
	 */
	pcluster_t   *cluster;
};

/**
 * Allocates a packet buffer
 */
void pbuf_alloc( size_t size );

/**
 * Frees a packet buffer
 */
void pbuf_free( pbuf_t *pbuf );

/**
 * Prepends data to the packet
 * @return Pointer to the prepended data
 */
uint8_t *pbuf_prepend( pbuf_t *pbuf, size_t size );

/**
 * Moves the payload pointer
 * @param start The amount of bytes to skip from the start.
 * @param end	The amount of bytes to skip before the end.
 */
void pbuf_trim( pbuf_t *pbuf, size_t start, size_t end );

/**
 * Combines split packet buffers into one containing only the payload.
 * @note The resulting packet buffer is not required to be physically
 *		 contiguous
 */
pbuf_t *pbuf_flatten( pbuf_t *in );

#endif

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
 * @file kernel/net/pbuf.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */
 
#include "kernel/net/pbuf.h"
#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#include <assert.h>
#include "config.h"

#if CONFIG_PBUF_CONTIGSZ > PHYSMM_PAGE_SIZE
#error Cannot assume possibility to create contiguous bufs larger than a page
#endif

pcluster_t *pcluster_free_list;
pcluster_t *pcluster_part_list;

/**
 * Initializes a packet buffer
 */
void pbuf_init( pbuf_t pbuf, uint8_t *buf, size_t bufsz )
{

	size_t     hdrsz,ftrsz;
	
	hdrsz = CONFIG_PBUF_HEADER_SIZE;
	ftrsz = CONFIG_PBUF_FOOTER_SIZE;

	assert( ( hdrsz + ftrsz ) > bufsz );
	
	memset( buf, 0, bufsz );

	pbuf->buf   = buf;
	pbuf->rawsz = bufsz;
	
	pbuf->data = buf + hdrsz;
	pbuf->datasz = bufsz - hdrsz - ftrsz;

}

/**
 * Allocate a packet cluster
 */
pcluster_t *pcluster_alloc( void )
{
	pcluster_t *cluster;
	uint8_t    *buf;
	int         cnt, i;
	size_t      allocsz;

	cnt = PHYSMM_PAGE_SIZE / CONFIG_PBUF_CONTIGSZ;

	buf = heapmm_alloc_page( );

	if ( !buf )
		goto errcleanup_0;

    allocsz = sizeof( pcluster_t ) + cnt * sizeof( pbuf_t );

	cluster = heapmm_alloc( allocsz );
	
	if ( !cluster )
		goto errcleanup_1;

	memset( cluster, 0, allocsz );

	cluster->nbufs = cnt;
	cluster->nfree = cnt;
	cluster->bufs = ( pbuf_t * ) &cluster[1];
	
	for ( i = 0; i < cnt; i++ ) {
	
		pbuf_init(	cluster->bufs + i, 
					buf + ( i * CONFIG_PBUF_CONTIGSZ ),
					CONFIG_PBUF_CONTIGSZ  );
					
		cluster->bufs[ i ].flags |= PBUF_CONTIG;
		cluster->bufs[ i ].cluster = cluster;
		
	}
	
	return cluster;

errcleanup_1:
	
	heapmm_free( buf, PHYSMM_PAGE_SIZE );
	
errcleanup_0:

	return NULL;
	
}

/**
 * Free a packet cluster
 */
void pcluster_free( pcluster_t *cluster )
{
	pcluster_t *cluster;
	uint8_t    *buf;
	int         cnt, i;
	size_t      allocsz, bufsz;
	
	cnt = cluster->nbufs;

	assert ( cluster->nfree == cnt );
	
	buf = cluster->bufs[ 0 ]->buf;
	bufsz = cnt * CONFIG_PBUF_CONTIGSZ;

	allocsz = sizeof( pcluster_t ) + cnt * sizeof( pbuf_t );
	
	heapmm_free( buf, bufsz );
	
	heapmm_free( cluster, allocsz );
	
}

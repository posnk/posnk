/**
 * fs/ext2/fsapi.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-07-2014 - Created
 */

#ifndef __FS_EXT2_FSAPI_H__
#define __FS_EXT2_FSAPI_H__

#include "kernel/vfs.h"
#include <sys/types.h>
#include "fs/ext2/fsdata.h"
#include "kernel/streams.h"

#define EXT2_BITMAP_GET(V,B)		((V) &  (1 << (B)))
#define EXT2_BITMAP_SET(V,B)		((V) |= (1 << (B)))
#define EXT2_BITMAP_CLR(V,B)		((V) &= ~(1 << (B)))

//We currently only support filetype in dirent
#define EXT2_SUPPORTED_REQ_FEATURES	(2)

#define EXT2_SUPPORTED_ROF_FEATURES	(0)

typedef struct ext2_vinode				ext2_vinode_t;
typedef struct ext2_device				ext2_device_t;

struct ext2_vinode {
	inode_t	     vfs_ino;
	ext2_inode_t inode;
};

struct ext2_device {
	fs_device_t		device;
	ext2_superblock_t	superblock;
	dev_t			dev_id;
	uint32_t		bgdt_block;
	aoff_t			inode_load_size;
};

void ext2_handle_error(ext2_device_t *device);
uint32_t ext2_divup(uint32_t a, uint32_t b);
uint32_t ext2_roundup(uint32_t a, uint32_t b);
SFUNC(aoff_t, ext2_read_block, 
					ext2_device_t *dev, 
					uint32_t block_ptr, 
					uint32_t in_block, 
					void *buffer, 
					aoff_t count);
SFUNC(aoff_t, ext2_write_block, 
					ext2_device_t *dev, 
					uint32_t block_ptr, 
					uint32_t in_block, 
					void *buffer, 
					aoff_t count);
SFUNC(ext2_block_group_desc_t *, ext2_load_bgd, 
									ext2_device_t *device, 
									uint32_t bg_id);
SVFUNC(ext2_store_bgd, 
									ext2_device_t *device, 
									uint32_t bg_id, 
									ext2_block_group_desc_t *bgd);

void ext2_free_bgd(	__attribute__((__unused__)) ext2_device_t *device, 
					ext2_block_group_desc_t *bgd);			
SVFUNC(ext2_free_block, ext2_device_t *device, uint32_t block_id);
SFUNC(uint32_t, ext2_alloc_block, ext2_device_t *device, uint32_t start);
SFUNC(uint32_t, ext2_alloc_inode, ext2_device_t *device);
SVFUNC( ext2_free_inode, ext2_device_t *device, uint32_t inode_id);
SFUNC(uint32_t, ext2_allocate_indirect_block, 
					ext2_device_t *device, 
					ext2_inode_t *inode);
SFUNC(uint32_t, ext2_decode_block_id, 
					ext2_device_t *device, 
					ext2_inode_t *inode, 
					uint32_t block_id);
SVFUNC(ext2_set_block_id, ext2_device_t *device, 
							ext2_inode_t *inode, 
							uint32_t block_id, 
							uint32_t block_v);
void ext2_e2tovfs_inode(ext2_device_t *device, ext2_vinode_t *_ino, ino_t ino_id);
void ext2_vfstoe2_inode(ext2_vinode_t *_ino, ino_t ino_id);
SFUNC(inode_t *, ext2_load_inode, fs_device_t *device, ino_t id);
SVFUNC(ext2_store_inode,inode_t *_inode);
SVFUNC(ext2_shrink_inode, ext2_device_t *device, ext2_inode_t *inode, aoff_t old_size, aoff_t new_size);
SVFUNC(ext2_trunc_inode, inode_t *_inode, aoff_t size);
SFUNC(aoff_t, ext2_write_inode, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length);
SFUNC(aoff_t, ext2_read_inode, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length);
SVFUNC(ext2_load_e2inode, ext2_device_t *device, ext2_inode_t *ino, uint32_t ino_id);
SVFUNC(ext2_store_e2inode, ext2_device_t *device, ext2_inode_t *ino, uint32_t ino_id);
SVFUNC(ext2_mknod, inode_t *_inode) ;
SFUNC(dirent_t *, ext2_finddir, inode_t *_inode, char * name);
SVFUNC(ext2_mkdir, inode_t *_inode);
SVFUNC( ext2_unlink, inode_t *_inode, char *name );
SVFUNC( ext2_link, inode_t *_inode, char *name, ino_t ino_id);
SFUNC(aoff_t, ext2_readdir, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length);
SFUNC( aoff_t, ext2_dir_readwrite_stub, 	
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) void *buffer,
					__attribute__((__unused__)) aoff_t length );
SVFUNC( ext2_dir_close, stream_info_t *stream );
SFUNC( aoff_t, ext2_dir_readdir, stream_info_t *stream, sys_dirent_t *buffer, aoff_t buflen);
#endif

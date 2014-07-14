/**
 * fs/ext2.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-07-2014 - Created
 */

#ifndef __FS_EXT2_H__
#define __FS_EXT2_H__

#include <stdint.h>
#include <sys/types.h>
#include "kernel/vfs.h"

#define EXT2_FS_STATE_CLEAN		(1)
#define EXT2_FS_STATE_ERRORS		(2)

#define EXT2_ERROR_ACTION_IGNORE	(1)
#define EXT2_ERROR_ACTION_REMOUNT_RO	(2)
#define EXT2_ERROR_ACTION_CRASH		(3)

#define EXT2_OS_ID_LINUX		(0)
#define EXT2_OS_ID_HURD			(1)
#define EXT2_OS_ID_MASIX		(2)
#define EXT2_OS_ID_FREEBSD		(3)
#define EXT2_OS_ID_BSDLITE		(4)

#define EXT2_VERSION_MAJOR_NOEXT	(0)
#define EXT2_VERSION_MAJOR_DYNAMIC	(1)

#define EXT2_NO_EXT_FIRST_INO		(11)
#define EXT2_NO_EXT_INODE_SIZE		(128)

#define EXT2_BAD_INODE			(1)
#define EXT2_ROOT_INODE			(2)
#define EXT2_ACL_IDX_INODE		(3)

#define EXT2_MOUNT_FLAG_RO		(1<<0)

#define EXT2_MODE_FMT(MoDe)		(MoDe & 0xF000)
#define EXT2_IFSOCK			(0xC000)
#define EXT2_IFLNK			(0xA000)
#define EXT2_IFREG			(0x8000)
#define EXT2_IFBLK			(0x6000)
#define EXT2_IFDIR			(0x4000)
#define EXT2_IFCHR			(0x2000)
#define EXT2_IFIFO			(0x1000)

#define EXT2_DEV_DECODE(dIn)		MAKEDEV( ((dIn >> 8) & 0xFF), (dIn & 0xFF) ) 

//We currently only support filetype in dirent
#define EXT2_SUPPORTED_REQ_FEATURES	(2)

#define EXT2_SUPPORTED_ROF_FEATURES	(0)

typedef struct ext2_superblock		ext2_superblock_t;
typedef struct ext2_block_group_desc	ext2_block_group_desc_t;
typedef struct ext2_inode		ext2_inode_t;
typedef struct ext2_dirent		ext2_dirent_t;
typedef struct ext2_vinode		ext2_vinode_t;
typedef struct ext2_device		ext2_device_t;

struct ext2_superblock {
	uint32_t inode_count;
	uint32_t block_count;
	uint32_t su_block_count;
	uint32_t free_block_count;
	uint32_t free_inode_count;
	uint32_t superblock_block_no;
	uint32_t block_size_enc;//1024 << block_size_enc = block_size
	uint32_t frag_size_enc;//1024 << block_size_enc = frag_size
	uint32_t blocks_per_group;
	uint32_t frags_per_group;
	uint32_t inodes_per_group;
	uint32_t mount_time;
	uint32_t write_time;//48
	uint16_t mounts_since_check;//52
	uint16_t max_mounts_before_check;
	uint16_t signature;//56
	uint16_t fs_state;
	uint16_t error_action;//60
	uint16_t version_minor;//62
	uint32_t check_time;//64
	uint32_t check_interval;//68
	uint32_t os_id;//72
	uint32_t version_major;//76
	uint16_t su_uid;//80
	uint16_t su_gid;//82
/* Extended Superblock */

	uint32_t first_inode;//Non ext: 11
	uint16_t inode_size;//Inode size in bytes, 128 in non ext2
	uint16_t block_group_no;//If backup
	uint32_t optional_features;
	uint32_t required_features;
	uint32_t ro_force_features;
	uint8_t  fs_id[16];
	uint8_t	 volume_name[16];
	uint8_t  last_mountpoint[64];
	uint32_t compression_type;

	uint8_t	 file_prealloc_block;
	uint8_t  dir_prealloc_block;
	uint16_t spare;

	uint8_t	 journal_id[16];
	uint32_t journal_inode;
	uint32_t journal_device;
	uint32_t orphan_inode_list_head;

	uint32_t hash_seed[4];
	uint8_t  def_hash_version;
	uint8_t  padding[3];

	uint32_t default_mount_options;
	uint32_t first_meta_bg;

	uint8_t  spare_2[788];
}  __attribute__((packed));

struct ext2_block_group_desc {
	uint32_t block_bitmap;
	uint32_t inode_bitmap;
	uint32_t inode_table;
	uint16_t free_block_count;
	uint16_t free_inode_count;
	uint16_t used_dir_count;
	uint16_t padding;
	uint8_t	 reserved[12];
}  __attribute__((packed));


struct ext2_inode {
	uint16_t mode;
	uint16_t uid;
	uint32_t size;
	uint32_t atime;
	uint32_t ctime;
	uint32_t mtime;
	uint32_t dtime;
	uint16_t gid;
	uint16_t link_count;
	uint32_t blocks;
	uint32_t flags;
	uint32_t osd1;
	uint32_t block[15];
	uint32_t file_acl;
	uint32_t dir_acl;
	uint32_t faddr;
	uint8_t	 osd2[12];
}  __attribute__((packed));

struct ext2_dirent {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  file_type;
}  __attribute__((packed)); //8 long

struct ext2_vinode {
	inode_t	     vfs_ino;
	ext2_inode_t inode;
};

struct ext2_device {
	fs_device_t		device;
	ext2_superblock_t	superblock;
	dev_t			dev_id;
	uint32_t		bgdt_block;
};

fs_device_t *ext2_mount(dev_t device, uint32_t flags);

#endif

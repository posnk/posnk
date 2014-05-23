/*
 * kernel/elfloader.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-04-2014 - Created
 */

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "kernel/earlycon.h"
#include "kernel/vfs.h"
#include "kernel/heapmm.h"
#include "kernel/tar.h"

int tar_read_record(inode_t *tar_file, off_t *pos);
int tar_read_record_mem(uintptr_t tar_data, off_t *pos);

int tar_extract(char * path)
{	
	off_t pos = 0;
	inode_t *inode = vfs_find_inode(path);

	if (!inode) {
		return ENOENT;
	}
	if (!S_ISREG(inode->mode)) {
		return EACCES;
	} 
	while (!tar_read_record(inode, &pos));
	return 0;	
}

int tar_extract_mem(void *data)
{	
	off_t pos = 0;
	while (!tar_read_record_mem((uintptr_t)data, &pos));
	return 0;	
}

uint32_t tar_num_dec(char *in, int l)
{
 
    uint32_t size = 0;
    uint32_t j;
    uint32_t count = 1;
 
    for (j = l-1; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
 
}

mode_t tar_get_mode(tar_header_t *header)
{
	mode_t mode = (mode_t) tar_num_dec(header->mode,8);
	switch(header->typeflag){
		default:
		case AREGTYPE:
		case REGTYPE:
		case CONTTYPE:
			mode |= S_IFREG;
			break;
		case SYMTYPE://TODO: Implement proper symlinks
		case LNKTYPE:
			break;
		case CHRTYPE:
			mode |= S_IFCHR;
			break;
		case BLKTYPE:
			mode |= S_IFBLK;
			break;
		case FIFOTYPE:
			mode |= S_IFIFO;
			break;
		case DIRTYPE:
			break;
	}
	return mode;
}

dev_t tar_get_dev(tar_header_t *header)
{
	dev_t major = (dev_t) tar_num_dec(header->devmajor,8);
	dev_t minor = (dev_t) tar_num_dec(header->devminor,8);
	return MAKEDEV(major,minor);
}

int tar_round_up(int num, int factor)
{
    return num + factor - 1 - (num - 1) % factor;
}



int tar_read_record_mem(uintptr_t tar_data, off_t *pos)
{
	inode_t *file;
	size_t wd_count;
	int status;	
	tar_header_t *header = (tar_header_t *) (tar_data + (uintptr_t)*pos);
	(*pos) += sizeof(tar_header_t);
	if (strlen(header->name) == 0){
		return 1;
	}
	if (strcmp(header->magic, TMAGIC) != 0) {
		earlycon_printf("WARNING: invalid tar record for file %s magic:\"%s\" read:%i pos:%i\n",header->name,header->magic,*pos);
		if (strlen(header->name) <= 1) {
			return EIO;
		}
			
	} 
	//earlycon_printf("untar %s \n",header->name);
	switch(header->typeflag){
		default:
		case AREGTYPE:
		case REGTYPE:
		case CONTTYPE:
			status = vfs_mknod(header->name, tar_get_mode(header), 0);
			if (status) {
				earlycon_printf("WARNING: error creating file while extracting %s, errno: %i\n",header->name, status);
				return status;
			}
			file = vfs_find_inode(header->name);
			if (!file) {
				earlycon_printf("WARNING: error opening file while extracting %s, errno: %i\n",header->name, status);
				return ENOSPC;
			}
			file->size = (off_t) tar_num_dec(header->size,12);
			if (file->size != 0) {
				status = vfs_write(file, 0, (void*)(tar_data + (uintptr_t)*pos), file->size, &wd_count, 0);
				(*pos) += tar_round_up(file->size,512);
				if (status) {
					earlycon_printf("WARNING: write error while extracting %s\n",header->name);
					return status;
				}
				if (wd_count !=  (size_t) file->size) {
					earlycon_printf("WARNING: out of space while extracting %s\n",header->name);
					return ENOSPC;
				}
			}
			break;
		case SYMTYPE://TODO: Implement proper symlinks
		case LNKTYPE:
			status = vfs_link(header->linkname, header->name);
			if (status) {
				return status;
			}
			break;
		case CHRTYPE:
		case BLKTYPE:
		case FIFOTYPE:
			status = vfs_mknod(header->name, tar_get_mode(header), tar_get_dev(header));
			if (status) {
				return status;
			}
			break;
		case DIRTYPE:
			status = vfs_mkdir(header->name, tar_get_mode(header));
			if (status) {
				earlycon_printf("WARNING: error creating directory while extracting %s, errno: %i\n",header->name, status);
				return status;
			}
			break;
		
	}
	if (header->typeflag != LNKTYPE) {
		file = vfs_find_inode(header->name);
		if (!file) {
			earlycon_printf("WARNING: error opening node while extracting %s, errno: %i\n",header->name, status);
			return ENOSPC;
		}
		file->uid = (uid_t) tar_num_dec(header->uid,8);
		file->gid = (gid_t) tar_num_dec(header->gid,8);
	}
	return 0;	
}

int tar_read_record(inode_t *tar_file, off_t *pos)
{
	void *buf;
	inode_t *file;
	size_t rd_count;
	size_t wd_count;
	off_t wr_count = 0;
	off_t wr_size  = 0;
	off_t block_size = 512*2*512;
	off_t e_block_size = block_size;
	int status;	
	tar_header_t *header = heapmm_alloc(sizeof(tar_header_t)); 
	if (!header) {
		return ENOMEM;
	}
	status = vfs_read(tar_file, *pos, header, sizeof(tar_header_t), &rd_count, 0);	
	if (status) {
		heapmm_free(header, sizeof(tar_header_t));
		earlycon_printf("WARNING: error read file while extracting %s, errno: %i\n",tar_file->name,status);
		return status;
	}
	(*pos) += rd_count;
	if (rd_count != sizeof(tar_header_t)) {
		heapmm_free(header, sizeof(tar_header_t));
		earlycon_printf("WARNING: size mismatch read file while extracting %s read:%i pos:%i, errno: %i\n",tar_file->name,status,rd_count,*pos);
		return EIO;
	}
	if (strlen(header->name) == 0){
		heapmm_free(header, sizeof(tar_header_t));
		return 1;
	}
	if (strcmp(header->magic, TMAGIC) != 0) {
		earlycon_printf("WARNING: invalid tar record for file %s magic:\"%s\" read:%i pos:%i\n",header->name,header->magic,rd_count,*pos);
		if (strlen(header->name) <= 1) {
			heapmm_free(header, sizeof(tar_header_t));
			return EIO;
		}
			
	} 
	switch(header->typeflag){
		default:
		case AREGTYPE:
		case REGTYPE:
		case CONTTYPE:
			status = vfs_mknod(header->name, tar_get_mode(header), 0);
			if (status) {
				earlycon_printf("WARNING: error creating file while extracting %s, errno: %i\n",header->name, status);
				heapmm_free(header, sizeof(tar_header_t));
				return status;
			}
			file = vfs_find_inode(header->name);
			if (!file) {
				earlycon_printf("WARNING: error opening file while extracting %s, errno: %i\n",header->name, status);
				heapmm_free(header, sizeof(tar_header_t));
				return ENOSPC;
			}
			file->size = (off_t) tar_num_dec(header->size,12);
			if (file->size != 0) {
				buf = heapmm_alloc(block_size); 
				if (!buf) {
					heapmm_free(header, sizeof(tar_header_t));
					return ENOSPC;
				}
				for (wr_count = 0; wr_count < file->size; wr_count += e_block_size) {
					if ((file->size - wr_count) < block_size)
						e_block_size = 512;
					status = vfs_read(tar_file, *pos, buf, e_block_size, &rd_count, 0);	
					if (status) {
						heapmm_free(header, sizeof(tar_header_t));
						heapmm_free(buf, block_size);
						earlycon_printf("WARNING: read error while extracting %s\n",header->name);
						return status;
					}
					(*pos) += rd_count;
					if (rd_count != (size_t) e_block_size) {
						earlycon_printf("WARNING: hit EOF while extracting %s\n",header->name);
						heapmm_free(header, sizeof(tar_header_t));
						heapmm_free(buf, block_size);
						return EIO;
					}
					wr_size = file->size - wr_count;
					if (wr_size > e_block_size)
						wr_size = e_block_size;
					status = vfs_write(file, wr_count, buf, wr_size, &wd_count, 0);
					if (status) {
						heapmm_free(header, sizeof(tar_header_t));
						heapmm_free(buf, block_size);
						earlycon_printf("WARNING: write error while extracting %s\n",header->name);
						return status;
					}
					if (wd_count !=  (size_t) wr_size) {
						earlycon_printf("WARNING: out of space while extracting %s\n",header->name);
						heapmm_free(header, sizeof(tar_header_t));
						heapmm_free(buf, block_size);
						return ENOSPC;
					}	
				}
				heapmm_free(buf, block_size);
			}
			break;
		case SYMTYPE://TODO: Implement proper symlinks
		case LNKTYPE:
			status = vfs_link(header->linkname, header->name);
			if (status) {
				heapmm_free(header, sizeof(tar_header_t));
				return status;
			}
			break;
		case CHRTYPE:
		case BLKTYPE:
		case FIFOTYPE:
			status = vfs_mknod(header->name, tar_get_mode(header), tar_get_dev(header));
			if (status) {
				heapmm_free(header, sizeof(tar_header_t));
				return status;
			}
			break;
		case DIRTYPE:
			status = vfs_mkdir(header->name, tar_get_mode(header));
			if (status) {
				earlycon_printf("WARNING: error creating directory while extracting %s, errno: %i\n",header->name, status);
				heapmm_free(header, sizeof(tar_header_t));
				return status;
			}
			break;
		
	}
	if (header->typeflag != LNKTYPE) {
		file = vfs_find_inode(header->name);
		if (!file) {
			earlycon_printf("WARNING: error opening node while extracting %s, errno: %i\n",header->name, status);
			heapmm_free(header, sizeof(tar_header_t));
			return ENOSPC;
		}
		file->uid = (uid_t) tar_num_dec(header->uid,8);
		file->gid = (gid_t) tar_num_dec(header->gid,8);
	}
	heapmm_free(header, sizeof(tar_header_t));
	return 0;	
}

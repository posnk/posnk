/**
 * kernel/vfs.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-04-2014 - Created
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/scheduler.h"
#include "kernel/permissions.h"
#include "kernel/synch.h"
#include "kernel/device.h"
#include "kernel/pipe.h"

#include "config.h"
//TODO: Make this an MRU cache
llist_t *inode_cache;

typedef struct vfs_cache_params {
	uint32_t device_id;
	ino_t inode_id;
} vfs_cache_params_t;

perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode)
{
	if (req_mode & ((inode->mode) & 7))
		return PERM_CLASS_OTHER;
	if (req_mode & ((inode->mode >> 3) & 7))
		return PERM_CLASS_GROUP;
	if (req_mode & ((inode->mode >> 6) & 7))
		return PERM_CLASS_OWNER;
	return PERM_CLASS_NONE;
}

int vfs_have_permissions(inode_t *inode, mode_t req_mode) {
	return get_perm_class(inode->uid, inode->gid) <= vfs_get_min_permissions(inode, req_mode);
}

/** 
 * Iterator function that looks up the requested inode
 */
int vfs_cache_find_iterator (llist_t *node, void *param)
{
	inode_t *inode = (inode_t *) node;
	vfs_cache_params_t *p = (vfs_cache_params_t *) param;
	return (inode->id == p->inode_id) && (inode->device_id == p->device_id);		
}

inode_t *vfs_get_inode(fs_device_t *device, ino_t inode_id)
{
	inode_t *result;
	vfs_cache_params_t search_params;
	/* Search inode cache */
	search_params.device_id = device->id;
	search_params.inode_id = inode_id;
	result = (inode_t *) llist_iterate_select(inode_cache, &vfs_cache_find_iterator, (void *) &search_params);
	if (result) {
		/* Cache hit, return inode */
		return result;
	}
	/* Cache miss, fetch it from disk */
	//NOTE : Schedule may happen below!
	result = device->ops->load_inode(device, inode_id);
	//NOTE : Schedule may have happened
	if (result) {
		/* File exists, add it to cache */
		llist_add_end(inode_cache, (llist_t *) result);
	}
	return result;
}

inode_t *vfs_effective_inode(inode_t * inode)
{
	if (inode->mount)
		return inode->mount;
	if (inode->link_path[0])
		return vfs_find_inode(inode->link_path);
	return inode;
}

dirent_t *vfs_find_dirent(inode_t * inode, char * name)
{
	return inode->device->ops->find_dirent(inode, name);
}

int vfs_int_mknod(inode_t * inode)
{
	if (!inode->device->ops->mknod)
		return ENOTSUP;
	return inode->device->ops->mknod(inode);
}

int vfs_int_rmnod(inode_t * inode)
{
	if (!inode->device->ops->rmnod)
		return ENOTSUP;
	return inode->device->ops->rmnod(inode);
}

int vfs_int_mkdir(inode_t * inode)
{
	if (!inode->device->ops->mkdir)
		return ENOTSUP;
	return inode->device->ops->mkdir(inode);
}

int vfs_int_link(inode_t * inode , char * name , ino_t nod_id)
{
	if (!inode->device->ops->link)
		return ENOTSUP;
	return inode->device->ops->link(inode, name, nod_id);
}

int vfs_int_unlink(inode_t * inode , char * name)
{	
	if (!inode->device->ops->unlink)
		return ENOTSUP;
	return inode->device->ops->unlink(inode, name);
}

int vfs_int_read_dir(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *read_size)
{
	return inode->device->ops->read_dir(inode, buffer, file_offset, count, read_size);
}

int vfs_int_read(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *read_size)
{
	return inode->device->ops->read_inode(inode, buffer, file_offset, count, read_size);
}

int vfs_int_write(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *write_size)
{
	if (!inode->device->ops->write_inode)
		return ENOTSUP;//TODO: Maybe return EROFS here?
	return inode->device->ops->write_inode(inode, buffer, file_offset, count, write_size);
}

int vfs_int_truncate(inode_t * inode, aoff_t size)
{
	if (!inode->device->ops->trunc_inode)
		return ENOTSUP;
	return inode->device->ops->trunc_inode(inode, size);
}

int vfs_write(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block)
{
	int status;
	aoff_t pad_size = 0;
	void *zbuffer;
	if (!inode)
		return EINVAL;
	inode = vfs_effective_inode(inode);
	semaphore_down(inode->lock);
	if (!vfs_have_permissions(inode, MODE_WRITE)) {
		semaphore_up(inode->lock);
		return EBADF;
	}
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:
			if ((file_offset + count) > inode->size) {
				/* Writing past EOF */
				if (file_offset > inode->size) {
					/* Writing starts past EOF */
					zbuffer = heapmm_alloc(file_offset - inode->size);

					memset(zbuffer, 0, file_offset - inode->size);

					status = vfs_int_write(inode, zbuffer, inode->size, file_offset - inode->size, &pad_size);

					heapmm_free(zbuffer,file_offset - inode->size);		

					inode->size += pad_size;
					if (pad_size)
						inode->mtime = system_time;

					if (status) {
						*read_size = 0;
						return status;	
					}
				}
			}		
		
			status = vfs_int_write(inode, buffer, file_offset, count, read_size);

			if (*read_size)
				inode->mtime = system_time;

			if ((file_offset + *read_size) > inode->size)
				inode->size = file_offset + *read_size;

			semaphore_up(inode->lock);

			return status;
		case S_IFDIR:
			semaphore_up(inode->lock);			
			return EISDIR;
		case S_IFIFO:
			semaphore_up(inode->lock);
			return pipe_write(inode->fifo, buffer, count, read_size, non_block);		
		case S_IFCHR:				
			semaphore_up(inode->lock);		
			status = device_char_write(inode->if_dev, file_offset, buffer, count, read_size, non_block);	
			return status;	
		case S_IFBLK:	
			status = device_block_write(inode->if_dev, file_offset, buffer, count, read_size);			
			semaphore_up(inode->lock);			
			return status;	
		default:			
			semaphore_up(inode->lock);	
			return EISDIR;		

	}
	
}

int vfs_truncate(inode_t * inode, aoff_t length)
{
	int status;
	if (!inode)
		return EINVAL;
	inode = vfs_effective_inode(inode);

	semaphore_down(inode->lock);

	if (!vfs_have_permissions(inode, MODE_WRITE)) {
		semaphore_up(inode->lock);
		return EBADF;
	}
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:			
			status = vfs_int_truncate(inode, length);

			if (!status) {
				inode->mtime = system_time;
				inode->size = length;
			}

			semaphore_up(inode->lock);

			return status;
		case S_IFDIR:
			semaphore_up(inode->lock);			
			return EISDIR;
		case S_IFBLK:	
		case S_IFCHR:	
		case S_IFIFO:
		default:		
			semaphore_up(inode->lock);
			return EINVAL;		

	}
	
}

int vfs_read(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block)
{
	int status;
	if (!inode)
		return EINVAL;
	inode = vfs_effective_inode(inode);
	semaphore_down(inode->lock);
	if (!vfs_have_permissions(inode, MODE_READ)) {
		semaphore_up(inode->lock);
		return EBADF;
	}
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:
			if ((file_offset + count) > inode->size) {
				/* Reading past EOF */
				if (file_offset >= inode->size) {
					/* Read starts past EOF */
					semaphore_up(inode->lock);
					(*read_size) = 0;
					return 0;					
				}
				count = inode->size - file_offset;	
			}				
			status = vfs_int_read(inode, buffer, file_offset, count, read_size);

			if (*read_size)
				inode->atime = system_time;

			semaphore_up(inode->lock);

			return status;
		case S_IFDIR:
			semaphore_up(inode->lock);			
			return EISDIR;
		case S_IFIFO:
			semaphore_up(inode->lock);
			return pipe_write(inode->fifo, buffer, count, read_size, non_block);	
		case S_IFCHR:	
			status = device_char_read(inode->if_dev, file_offset, buffer, count, read_size, non_block);			
			semaphore_up(inode->lock);			
			return status;	
		case S_IFBLK:	
			status = device_block_read(inode->if_dev, file_offset, buffer, count, read_size);			
			semaphore_up(inode->lock);			
			return status;	
		default:			
			semaphore_up(inode->lock);	
			return EISDIR;		

	}
	
}

int vfs_getdents(inode_t * inode , aoff_t file_offset, dirent_t * buffer, aoff_t count, aoff_t *read_size)
{
	int status;

	if (!inode)
		return EINVAL;

	inode = vfs_effective_inode(inode);

	semaphore_down(inode->lock);

	if (!vfs_have_permissions(inode, MODE_READ)) {
		semaphore_up(inode->lock);
		return EBADF;
	}

	switch ((inode->mode) & S_IFMT) {
		case S_IFDIR:
			if ((file_offset + count) > inode->size) {
				/* Reading past EOF */
				if (file_offset >= inode->size) {
					/* Read starts past EOF */
					semaphore_up(inode->lock);
					(*read_size) = 0;
					return 0;					
				}
				count = inode->size - file_offset;	
			}	
			
			status = vfs_int_read_dir(inode, buffer, file_offset, count, read_size);

			if (*read_size)
				inode->atime = system_time;

			semaphore_up(inode->lock);

			return status;
		default:			
			semaphore_up(inode->lock);	
			return ENOTDIR;		

	}
	
}

int vfs_rmdir(char *path)
{
	inode_t *inode = vfs_find_inode(path);
	if (!inode)
		return ENOENT;
	if (inode->size)
		return EEXIST;
	if (inode->mount || (scheduler_current_task->root_directory->inode == inode))
		return EBUSY;
	return vfs_unlink(path);
}

int vfs_mkdir(char *path, mode_t mode)
{
	inode_t *dir;
	inode_t *parent;

	int err = vfs_mknod(path, S_IFDIR | (mode & 0777), 0);

	if (err)
		return err;

	parent = vfs_find_parent(path);
	dir = vfs_find_inode(path);
	if ((!parent) || !dir)
		return EFAULT;//TODO: Find more appropriate ERRNO

	semaphore_down(dir->lock);

	err = vfs_int_mkdir(dir);

	if (err) {
		semaphore_up(dir->lock);
		vfs_unlink(path);
		return err;
	}

	parent->mtime = system_time;
	semaphore_up(dir->lock);
	return 0;
}

int vfs_mknod(char *path, mode_t mode, dev_t dev)
{
	int status;
	char * separator;
	char * name;
	inode_t *parent = vfs_find_parent(path);
	if (!parent) {
		return ENOENT;
	}
	inode_t *inode = heapmm_alloc(parent->device->inode_size);
	if (!inode)
		return ENOMEM;
	if (vfs_find_inode(path)) {
		heapmm_free(inode, parent->device->inode_size);
		return EEXIST;
	}
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		heapmm_free(inode, parent->device->inode_size);
		return EPERM;
	}
	memset(inode, 0, parent->device->inode_size);
	inode->device_id = parent->device_id;
	inode->device = parent->device;
	/* INODE ID is filled by fs driver */
	separator = strrchr(path, '/');
	if (separator == (path + strlen(path) - 1)){
		path[strlen(path) - 1] = '\0';
		separator = strrchr(path, '/');
	}
	if (separator)
		name = separator + 1;
	else
		name = path;
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		heapmm_free(inode, parent->device->inode_size);		
		return ENAMETOOLONG;
	}
	strcpy(inode->name, name);
	inode->mode = mode & ~(scheduler_current_task->umask);
	inode->if_dev = dev;
	inode->uid = scheduler_current_task->uid;
	inode->gid = scheduler_current_task->gid;
	inode->lock = semaphore_alloc();
	inode->atime = inode->mtime = inode->ctime = system_time;
	if (S_ISFIFO(inode->mode)) {
		inode->fifo = pipe_create();
	}
	status = vfs_int_mknod(inode);//Push inode to storage
	if (status) { 
		heapmm_free(inode, parent->device->inode_size);
		return status;
	}	
	llist_add_end(inode_cache, (llist_t *) inode);
	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = vfs_int_link(parent, name, inode->id);
	if (status) {
		llist_unlink((llist_t *) inode);
		vfs_int_rmnod(inode);//TODO: Check result?
		heapmm_free(inode, parent->device->inode_size);
		return status;
	}
	inode->hard_link_count++;
	semaphore_up(inode->lock);
	return 0;	
}
fs_device_t *ext2_mount(dev_t device, uint32_t flags);

int vfs_mount(char *device, char *mountpoint, char *fstype, uint32_t flags)
{
	fs_device_t *fsdevice;
	inode_t	    *mp_inode;
	inode_t	    *dev_inode;
	mp_inode = vfs_find_inode(mountpoint);
	if (!mp_inode)
		return ENOENT;
	if (!S_ISDIR(mp_inode->mode))
		return ENOTDIR;

	dev_inode = vfs_find_inode(device);
	if (!dev_inode)
		return ENOENT;
	if (!S_ISBLK(dev_inode->mode))
		return ENOTBLK;
		
	fsdevice = ext2_mount(dev_inode->if_dev, flags);
	
	if (!fsdevice)
		return EINVAL;
		
	mp_inode->mount = fsdevice->ops->load_inode(fsdevice, fsdevice->root_inode_id);	
	
	if (!mp_inode->mount)
		return EINVAL;

	return 0;
}

int vfs_symlink(char *oldpath, char *path)
{
	int status;
	char * separator;
	char * name;
	inode_t *parent = vfs_find_parent(path);
	if (!parent) {
		return ENOENT;
	}
	inode_t *inode = heapmm_alloc(parent->device->inode_size);
	if (!inode)
		return ENOMEM;
	if (vfs_find_inode(path)) {
		heapmm_free(inode, parent->device->inode_size);
		return EEXIST;
	}
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		heapmm_free(inode, parent->device->inode_size);
		return EPERM;
	}
	memset(inode, 0, parent->device->inode_size);
	inode->device_id = parent->device_id;
	inode->device = parent->device;
	/* INODE ID is filled by fs driver */
	separator = strrchr(path, '/');
	if (separator == (path + strlen(path) - 1)){
		path[strlen(path) - 1] = '\0';
		separator = strrchr(path, '/');
	}
	if (separator)
		name = separator + 1;
	else
		name = path;
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		heapmm_free(inode, parent->device->inode_size);		
		return ENAMETOOLONG;
	}
	strcpy(inode->name, name);
	inode->mode = 0;
	inode->if_dev = 0;
	inode->uid = scheduler_current_task->uid; 
	inode->gid = scheduler_current_task->gid; 
	inode->atime = inode->mtime = inode->ctime = system_time;
	inode->lock = semaphore_alloc();
	strcpy(inode->link_path, oldpath);

	status = vfs_int_mknod(inode);

	if (status) { //Push inode to storage
		heapmm_free(inode, parent->device->inode_size);
		return status;
	}	
	llist_add_end(inode_cache, (llist_t *) inode);
	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = vfs_int_link(parent, name, inode->id);

	if (status) {
		llist_unlink((llist_t *) inode);
		vfs_int_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);
		return status;
	}
	inode->hard_link_count++;
	semaphore_up(inode->lock);
	return 0;	
}

int vfs_unlink(char *path)
{
	int status;
	inode_t *inode = vfs_find_symlink(path);
	inode_t *parent = vfs_find_parent(path);
	char * separator;
	char * name;
	if (!inode)
		return ENOENT;
	if (!parent) 
		return ENOENT;
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		return EACCES;
	}
	semaphore_down(inode->lock);
	if (inode->usage_count != 0) {
		semaphore_up(inode->lock);
		return EBUSY;
	}
	separator = strrchr(path, '/');
	if (separator == (path + strlen(path) - 1)){
		path[strlen(path) - 1] = '\0';
		separator = strrchr(path, '/');
	}
	if (separator)
		name = separator + 1;
	else
		name = path;
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		heapmm_free(inode, parent->device->inode_size);		
		return ENAMETOOLONG;
	}
	semaphore_down(parent->lock);

	status = vfs_int_unlink(parent, name);

	if (status) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);
		return status;
	}
	semaphore_up(parent->lock);
	inode->hard_link_count--;
	inode->ctime = system_time;
	if (inode->hard_link_count == 0) {
		/* File is orphaned */
		vfs_int_rmnod(inode); //This might error but there is little we can do about that anyway
		llist_unlink((llist_t *) inode);
		heapmm_free(inode, inode->device->inode_size);		
	} else {
		semaphore_up(inode->lock);
	}
	return 0;
}

int vfs_initialize(fs_device_t * root_device)
{
	//TODO: Track mounted devices
	inode_cache = heapmm_alloc(sizeof(llist_t));
	inode_t *root_inode = root_device->ops->load_inode(root_device, root_device->root_inode_id);
	if (!root_inode)
		return 0;
	llist_create(inode_cache);
	llist_add_end(inode_cache, (llist_t *) root_inode);
	scheduler_current_task->root_directory = vfs_dir_cache_mkroot(root_inode);
	scheduler_current_task->current_directory = scheduler_current_task->root_directory;
	return 1;
}

int vfs_link(char *oldpath, char *newpath)
{
	int status;
	char * separator;
	char * name;
	inode_t *parent = vfs_find_parent(newpath);
	inode_t *inode = vfs_find_inode(oldpath);
	if (!inode) {
		return ENOENT;
	}
	semaphore_down(inode->lock);
	if (!parent) {
		semaphore_up(inode->lock);
		return ENOENT;
	}
	semaphore_down(parent->lock);
	if (vfs_find_inode(newpath)) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);
		return EEXIST;
	}
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);
		return EACCES;
	}
	if (parent->device_id != inode->device_id) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);		
		return EXDEV;
	}
	separator = strrchr(newpath, '/');
	if (separator == (newpath + strlen(newpath) - 1)){
		newpath[strlen(newpath) - 1] = '\0';
		separator = strrchr(newpath, '/');
	}
	if (separator)
		name = separator + 1;
	else
		name = newpath;
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);	
		return ENAMETOOLONG;
	}
	status = vfs_int_link(parent, name, inode->id);
	
	if (status) {
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);	
		return status;
	}
	parent->mtime = system_time;
	semaphore_up(parent->lock);
	inode->hard_link_count++;
	inode->ctime = system_time;
	semaphore_up(inode->lock);
	return 0;	
}

dir_cache_t *vfs_dir_cache_mkroot(inode_t *root_inode)
{
	dir_cache_t *dirc = heapmm_alloc(sizeof(dir_cache_t));
	dirc->parent = dirc;
	dirc->inode = root_inode;
	dirc->usage_count = 1;
	dirc->inode->usage_count++;
	return dirc;	
}

void vfs_dir_cache_release(dir_cache_t *dirc)
{
	dirc->usage_count--;
	if (dirc->usage_count)
		return;
	if (dirc->parent != dirc)
		vfs_dir_cache_release(dirc->parent);
	dirc->inode->usage_count--;
	heapmm_free(dirc, sizeof(dir_cache_t));	
}

dir_cache_t *vfs_find_dirc_parent(char * path)
{
	dir_cache_t *dirc = scheduler_current_task->current_directory;
	dir_cache_t *newc;
	inode_t * parent = dirc->inode;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	
	if ((path == NULL) || ((*path) == '\0'))
		return NULL;
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);
	end_of_path = strchr(remaining_path, 0);
	for (;;) {
		separator = strchrnul(remaining_path, (int) '/');
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
		if ((element_count == 0) && (element_size == 0)){ // First character is / 
			dirc = scheduler_current_task->root_directory;
			parent = dirc->inode;
		} else if (element_size == 0) {
			if ((separator + 2) >= end_of_path) {
				dirc->usage_count++;
				dirc->inode->usage_count++;
				return dirc;
			}
		} else if ((element_size == 2) && !strncmp(remaining_path, "..", 2)) {
			newc = dirc->parent;
			if (newc != dirc) {		
				if ((!dirc->usage_count))	
					heapmm_free(dirc, sizeof(dir_cache_t));
				parent = newc->inode;
				dirc = newc;
			}						
		} else if ((element_size == 1) && !strncmp(remaining_path, ".", 1)) {				
		} else {
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';
			dirent = vfs_find_dirent(parent, path_element);
			if (dirent == NULL) {
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				if ((separator + 1) >= end_of_path) {
					dirc->usage_count++;
					dirc->inode->usage_count++;
					return dirc;
				} else if (dirc && dirc->usage_count == 0){
					vfs_dir_cache_release(dirc->parent);
					heapmm_free(dirc, sizeof(dir_cache_t));
				}
				return NULL;
			}
			dirc->inode->usage_count++;
			dirc->usage_count++;
			newc = heapmm_alloc(sizeof(dir_cache_t));
			newc->parent = dirc;
			newc->inode = vfs_effective_inode(vfs_get_inode(parent->device, dirent->inode_id));
			newc->usage_count = 0;
			dirc = newc;		
			parent = dirc->inode;
		}
		remaining_path = separator + 1;
		if (remaining_path >= end_of_path) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			dirc->parent->usage_count++;
			dirc->parent->inode->usage_count++;
			vfs_dir_cache_release(dirc);
			return dirc->parent;
		}
		if (!S_ISDIR(parent->mode)) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			if (dirc && dirc->usage_count == 0){
				vfs_dir_cache_release(dirc->parent);
				heapmm_free(dirc, sizeof(dir_cache_t));
			}
			return NULL;
		}
	}
}

dir_cache_t *vfs_find_dirc(char * path)
{ //TODO: Check for search permission
	dir_cache_t *dirc = scheduler_current_task->current_directory;
	dir_cache_t *newc;
	inode_t * parent = dirc->inode;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	
	if ((path == NULL) || ((*path) == '\0'))
		return NULL;
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);
	end_of_path = strchr(remaining_path, 0);
	for (;;) {
		separator = strchrnul(remaining_path, (int) '/');
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
		if ((element_count == 0) && (element_size == 0)){ // First character is / 
			dirc = scheduler_current_task->root_directory;
			parent = dirc->inode;
		} else if (element_size == 0) {
			if ((separator + 2) >= end_of_path) {
				dirc->usage_count++;
				dirc->inode->usage_count++;
				return dirc;
			}
		} else if ((element_size == 2) && !strncmp(remaining_path, "..", 2)) {
			newc = dirc->parent;
			if (newc != dirc) {		
				if ((!dirc->usage_count))	
					heapmm_free(dirc, sizeof(dir_cache_t));
				parent = newc->inode;
				dirc = newc;
			}						
		} else if ((element_size == 1) && !strncmp(remaining_path, ".", 1)) {				
		} else {
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';
			dirent = vfs_find_dirent(parent, path_element);
			if (dirent == NULL) {
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				if (dirc && dirc->usage_count == 0){
					vfs_dir_cache_release(dirc->parent);
					heapmm_free(dirc, sizeof(dir_cache_t));
				}
				return NULL;
			}
			dirc->inode->usage_count++;
			dirc->usage_count++;
			newc = heapmm_alloc(sizeof(dir_cache_t));
			newc->parent = dirc;
			newc->inode = vfs_effective_inode(vfs_get_inode(parent->device, dirent->inode_id));
			newc->usage_count = 0;
			dirc = newc;		
			parent = dirc->inode;
		}
		remaining_path = separator + 1;
		if (remaining_path >= end_of_path) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			dirc->usage_count++;
			dirc->inode->usage_count++;
			return dirc;
		}
		if (!S_ISDIR(parent->mode)) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			if (dirc && dirc->usage_count == 0){
				vfs_dir_cache_release(dirc->parent);
				heapmm_free(dirc, sizeof(dir_cache_t));
			}
			return NULL;
		}
	}
} 

inode_t *vfs_find_parent(char * path)
{ //TODO: Check for search permission
	inode_t *ino;
	dir_cache_t *dirc = vfs_find_dirc_parent(path);
	if (!dirc)
		return NULL;
	ino = dirc->inode;
	vfs_dir_cache_release(dirc);
	return ino;
} 

inode_t *vfs_find_inode(char * path)
{ //TODO: Check for search permission
/*
	inode_t * parent = scheduler_current_task->current_directory;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	
	if ((path == NULL) || ((*path) == '\0'))
		return NULL;
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);
	end_of_path = strchr(remaining_path, 0);
	for (;;) {
		separator = strchrnul(remaining_path, (int) '/');
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
		if ((element_count == 0) && (element_size == 0)) // First character is /
			parent = vfs_effective_inode(scheduler_current_task->root_directory);
		else if (element_size == 0) {
			if ((separator + 2) >= end_of_path) {
				return parent;
			}
		} else {
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';
			dirent = vfs_find_dirent(parent, path_element);
			if (dirent == NULL) {
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				return NULL;
			}
			parent = vfs_effective_inode(vfs_get_inode(parent->device, dirent->inode_id));			
		}
		remaining_path = separator + 1;
		if (remaining_path >= end_of_path) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			return parent;
		}
		if (!S_ISDIR(parent->mode)) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			return NULL;
		}
	}*/
	inode_t *ino;
	dir_cache_t *dirc = vfs_find_dirc(path);
	if (!dirc)
		return NULL;
	ino = dirc->inode;
	vfs_dir_cache_release(dirc);
	return ino;
} 

inode_t *vfs_find_symlink(char * path)
{ //TODO: Check for search permission
	/*inode_t * parent = scheduler_current_task->current_directory;
	* /
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	
	if ((path == NULL) || ((*path) == '\0'))
		return NULL;
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);
	end_of_path = strchr(remaining_path, 0);
	for (;;) {
		separator = strchrnul(remaining_path, (int) '/');
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
		if ((element_count == 0) && (element_size == 0)) // First character is /
			parent = vfs_effective_inode(scheduler_current_task->root_directory);
		else if (element_size == 0) {
			if ((separator + 2) >= end_of_path) {
				if (dirent)
					return vfs_get_inode(parent->device, dirent->inode_id);
				return parent;
			}
		} else {
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';
			dirent = vfs_find_dirent(parent, path_element);
			if (dirent == NULL) {
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				return NULL;
			}
			parent = vfs_effective_inode(vfs_get_inode(parent->device, dirent->inode_id));			
		}
		remaining_path = separator + 1;
		if (remaining_path >= end_of_path) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			if (dirent)
				return vfs_get_inode(parent->device, dirent->inode_id);
			return parent;
		}
		if (!S_ISDIR(parent->mode)) {
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			return NULL;
		}
	}*/
	dirent_t *dirent;
	char * separator;
	char * name;
	inode_t *ino;
	dir_cache_t *dirc = vfs_find_dirc(path);
	if (!dirc)
		return NULL;
	if ((dirc == scheduler_current_task->root_directory) || (dirc == scheduler_current_task->current_directory)) {
		ino = dirc->inode;
		vfs_dir_cache_release(dirc);
		return ino;		
	}
	separator = strrchr(path, '/');
	if (separator == (path + strlen(path) - 1)){
		path[strlen(path) - 1] = '\0';
		separator = strrchr(path, '/');
	}
	if (separator)
		name = separator + 1;
	else
		name = path;
	dirent = vfs_find_dirent(dirc->parent->inode, name);
	if (dirent == NULL) {
		vfs_dir_cache_release(dirc);
		return NULL;
	}
	ino = vfs_get_inode(dirc->parent->inode->device, dirent->inode_id);
	vfs_dir_cache_release(dirc);
	return ino;
} 



/*
 * kernel/sc_vfs.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-04-2014 - Created
 */

#include <stdint.h>
#include "kernel/syscall.h"
#include "kernel/vfs.h"
#include "kernel/heapmm.h"
#include <sys/errno.h>
#include <sys/stat.h>
#include <string.h>

//int vfs_link(char *oldpath, char *newpath);
uint32_t sys_link(uint32_t param[4], uint32_t param_size[4])
{
	char *oldpath;
	char *newpath;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH) || (param_size[1] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], oldpath, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(param_size[1]);
	if (!copy_user_to_kern((void *)param[1], newpath, param_size[1])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		heapmm_free(newpath, param_size[1]);
		return (uint32_t) -1;
	}	
	status = vfs_link(oldpath, newpath);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, param_size[0]);
	heapmm_free(newpath, param_size[1]);
	return (uint32_t) status;
}

uint32_t sys_mount(uint32_t param[4], uint32_t param_size[4])
{
	char *oldpath;
	char *newpath;
	char *fs;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH) || (param_size[1] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], oldpath, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(param_size[1]);
	if (!copy_user_to_kern((void *)param[1], newpath, param_size[1])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		heapmm_free(newpath, param_size[1]);
		return (uint32_t) -1;
	}
	fs = heapmm_alloc(param_size[2]);
	if (!copy_user_to_kern((void *)param[2], fs, param_size[2])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		heapmm_free(newpath, param_size[1]);
		heapmm_free(fs, param_size[2]);
		return (uint32_t) -1;
	}	
	status = vfs_mount(oldpath, newpath, fs, param[3]);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, param_size[0]);
	heapmm_free(newpath, param_size[1]);
	heapmm_free(fs, param_size[2]);
	return (uint32_t) status;
}

//int vfs_symlink(char *oldpath, char *newpath);
uint32_t sys_symlink(uint32_t param[4], uint32_t param_size[4])
{
	char *oldpath;
	char *newpath;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH) || (param_size[1] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], oldpath, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(param_size[1]);
	if (!copy_user_to_kern((void *)param[1], newpath, param_size[1])) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, param_size[0]);
		heapmm_free(newpath, param_size[1]);
		return (uint32_t) -1;
	}	
	status = vfs_symlink(oldpath, newpath);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, param_size[0]);
	heapmm_free(newpath, param_size[1]);
	return (uint32_t) status;
}

//int vfs_unlink(char *path);
uint32_t sys_unlink(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = vfs_unlink(path);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

int _sys_chdir(char *path)
{
	int status;
	dir_cache_t *dirc;

	status = vfs_find_dirc(path, &dirc);
	if (status) {
		syscall_errno = status;
		return -1;
	}

	status = vfs_chdir(dirc);

	if (status) {
		syscall_errno = status;
		return -1;
	}
	vfs_dir_cache_release(dirc);

	return 0;
}

int _sys_chroot(char *path)
{
	int status;
	dir_cache_t *dirc;

	status = vfs_find_dirc(path, &dirc);
	if (status) {
		syscall_errno = status;
		return -1;
	}

	status = vfs_chroot(dirc);

	if (status) {
		syscall_errno = status;
		return -1;
	}
	vfs_dir_cache_release(dirc);

	return 0;
}

//int chroot(char *path);
uint32_t sys_chroot(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_chroot(path);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

//int chdir(char *path);
uint32_t sys_chdir(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_chdir(path);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

int _sys_chmod(char *path, mode_t mode)
{
	inode_t *inode;
	int status;

	status = vfs_find_inode(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}

	if (get_perm_class(inode->uid, inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}
	if (get_effective_uid() != 0)
		mode &= 0777;
	inode->mode &= ~07777;
	inode->mode |= mode & 07777;
	inode->ctime = system_time;
	vfs_inode_release(inode);
	return 0;
}

//int chmod(char *path, mode_t mode);
uint32_t sys_chmod(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_chmod(path, (mode_t) param[1]);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}


int _sys_chown(char *path, uid_t owner, gid_t group)
{
	inode_t *inode;
	int status;

	status = vfs_find_inode(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}
	if (get_perm_class(inode->uid, inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}
	if (get_perm_class(-1, group) > PERM_CLASS_GROUP)
		group = 65535;
	if (group != 65535)
		inode->gid = group;
	if (owner != 65535)
		inode->uid = owner;
	inode->ctime = system_time;
	vfs_inode_release(inode);
	return 0;
}

//int chown(char *path, uid_t owner, gid_t group);
uint32_t sys_chown(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_chown(path, (uid_t) param[1], (uid_t) param[2]);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}


int _sys_truncate(char *path, off_t length)
{
	inode_t *inode;
	int status;

	status = vfs_find_inode(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}
	//TODO: Add permission check
	inode->ctime = system_time;
	vfs_truncate(inode, length);
	vfs_inode_release(inode);
	return 0;
}

//int truncate(char *path, off_t length);
uint32_t sys_truncate(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_truncate(path, (off_t) param[1]);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

uint32_t sys_umask(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	mode_t old = scheduler_current_task->umask;
	scheduler_current_task->umask = param[0] & 0777;
	return (uint32_t) old;
}

//int vfs_mknod(char *path, mode_t mode, dev_t dev)
uint32_t sys_mknod(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = vfs_mknod(path, (mode_t) param[1], (dev_t) param[2]);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

//int vfs_mkdir(char *path, mode_t mode)
uint32_t sys_mkdir(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = vfs_mkdir(path, (mode_t) param[1]);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

int _sys_stat(char *path, struct stat* buf)
{
	inode_t *inode;
	int status;

	status = vfs_find_inode(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}
	buf->st_dev  = (dev_t) inode->device_id;//TODO: FIX
	buf->st_ino  = inode->id;
	buf->st_rdev = inode->if_dev;
	buf->st_size = inode->size;
	buf->st_mode = inode->mode;
	//buf->st_blocks = buf->st_size * 512;//TODO: Implement sparse files
	//buf->st_blksize = 512;//TODO: Ask FS about block size
	buf->st_nlink = inode->hard_link_count;
	buf->st_uid = inode->uid;
	buf->st_gid = inode->gid;
	buf->st_atime = (time_t) inode->atime; 
	buf->st_mtime = (time_t) inode->mtime;
	buf->st_ctime = (time_t) inode->ctime;
	vfs_inode_release(inode);
	return 0;
}

//int stat(char *path, struct stat* buf);
uint32_t sys_stat(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	struct stat* buf;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_stat(path, buf);
	if (!copy_kern_to_user(buf, (void *)param[1], sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(path, param_size[0]);
	heapmm_free(buf, sizeof(struct stat));
	return (uint32_t) status;
}

int _sys_readlink(char *path, char *buf, size_t bufsiz)
{
	size_t read_size = 0;
	inode_t *inode;
	int status;

	status = vfs_find_symlink(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}

	syscall_errno = vfs_readlink(inode, buf, bufsiz, &read_size);

	vfs_inode_release(inode);

	if (syscall_errno != 0)
		return -1;

	return read_size;
}

//int stat(char *path, struct stat* buf);
uint32_t sys_readlink(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	char* buf;
	size_t bufsiz;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	bufsiz = param[2];
	if (bufsiz > CONFIG_FILE_MAX_NAME_LENGTH)
		bufsiz = CONFIG_FILE_MAX_NAME_LENGTH;
	buf = heapmm_alloc(bufsiz);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_readlink(path, buf, bufsiz);
	if (!copy_kern_to_user(buf, (void *)param[1], bufsiz)) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		heapmm_free(buf, bufsiz);
		return (uint32_t) -1;
	}
	heapmm_free(path, param_size[0]);
	heapmm_free(buf, bufsiz);
	return (uint32_t) status;
}

int _sys_lstat(char *path, struct stat* buf)
{
	inode_t *inode;
	int status;

	status = vfs_find_symlink(path, &inode);
	if (status) {
		syscall_errno = status;
		return -1;
	}
	buf->st_dev  = (dev_t) inode->device_id;//TODO: FIX
	buf->st_ino  = inode->id;
	buf->st_rdev = inode->if_dev;
	buf->st_size = inode->size;
	buf->st_mode = inode->mode;
	//buf->st_blocks = buf->st_size * 512;//TODO: Implement sparse files
	//buf->st_blksize = 512;//TODO: Ask FS about block size
	buf->st_nlink = inode->hard_link_count;
	buf->st_uid = inode->uid;
	buf->st_gid = inode->gid;
	buf->st_atime = (time_t) inode->atime; 
	buf->st_mtime = (time_t) inode->mtime;
	buf->st_ctime = (time_t) inode->ctime;
	vfs_inode_release(inode);
	return 0;
}

//int lstat(char *path, struct stat* buf);
uint32_t sys_lstat(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	struct stat* buf;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_lstat(path, buf);
	if (!copy_kern_to_user(buf, (void *)param[1], sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(path, param_size[0]);
	heapmm_free(buf, sizeof(struct stat));
	return (uint32_t) status;
}

uint32_t sys_rmdir(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = vfs_rmdir(path);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

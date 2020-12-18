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
#include "kernel/device.h"
#include <sys/errno.h>
#include <sys/stat.h>
#include <string.h>

//int vfs_link(char *oldpath, char *newpath);
SYSCALL_DEF2(link)
{
	char *oldpath;
	char *newpath;
	int status;
	int sza, szb;
	sza = procvmm_check_string( (char *)a, CONFIG_FILE_MAX_NAME_LENGTH );
	szb = procvmm_check_string( (char *)b, CONFIG_FILE_MAX_NAME_LENGTH );
	if (( sza < 0 ) || ( szb < 0 )) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, oldpath, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(szb);
	if (!copy_user_to_kern((void *)b, newpath, szb)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		heapmm_free(newpath, szb);
		return (uint32_t) -1;
	}
	status = vfs_link(oldpath, newpath);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, sza);
	heapmm_free(newpath, szb);
	return (uint32_t) status;
}

SYSCALL_DEF4(mount)
{
	char *oldpath;
	char *newpath;
	char *fs;
	int status;
	int sza,szb,szc;
	sza = procvmm_check_string( (char *)a, CONFIG_FILE_MAX_NAME_LENGTH );
	szb = procvmm_check_string( (char *)b, CONFIG_FILE_MAX_NAME_LENGTH );
	szc = procvmm_check_string( (char *)c, 8 );
	if (( sza < 0 ) || ( szb < 0 ) || (szc < 0) ) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, oldpath, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(szb);
	if (!copy_user_to_kern((void *)b, newpath, szb)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		heapmm_free(newpath, szb);
		return (uint32_t) -1;
	}
	fs = heapmm_alloc(szc);
	if (!copy_user_to_kern((void *)c, fs, szc)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		heapmm_free(newpath, szb);
		heapmm_free(fs, szc);
		return (uint32_t) -1;
	}
	status = vfs_mount(oldpath, newpath, fs, d);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, sza);
	heapmm_free(newpath, szb);
	heapmm_free(fs, szc);
	return (uint32_t) status;
}

//int vfs_symlink(char *oldpath, char *newpath);
SYSCALL_DEF2(symlink)
{
	char *oldpath;
	char *newpath;
	int status;
	int sza,szb;
	sza = procvmm_check_string( (char *)a, CONFIG_FILE_MAX_NAME_LENGTH );
	szb = procvmm_check_string( (char *)b, CONFIG_FILE_MAX_NAME_LENGTH );
	if (( sza < 0 ) || ( szb < 0 )) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	oldpath = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, oldpath, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		return (uint32_t) -1;
	}
	newpath = heapmm_alloc(szb);
	if (!copy_user_to_kern((void *)b, newpath, szb)) {
		syscall_errno = EFAULT;
		heapmm_free(oldpath, sza);
		heapmm_free(newpath, szb);
		return (uint32_t) -1;
	}
	status = vfs_symlink(oldpath, newpath);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(oldpath, sza);
	heapmm_free(newpath, szb);
	return (uint32_t) status;
}

SYSCALL_DEF0(sync)
{
	vfs_cache_flush();
	vfs_sync_filesystems();
	return (uint32_t) device_block_flush_global();
}

//int vfs_unlink(char *path);
SYSCALL_DEF1(unlink)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = vfs_unlink(path);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, sza);
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
SYSCALL_DEF1(chroot)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = _sys_chroot(path);
	heapmm_free(path, sza);
	return (uint32_t) status;
}

//int chdir(char *path);
SYSCALL_DEF1(chdir)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = _sys_chdir(path);
	heapmm_free(path, sza);
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
SYSCALL_DEF2(chmod)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = _sys_chmod(path, (mode_t) b);
	heapmm_free(path, sza);
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
SYSCALL_DEF3(chown)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = _sys_chown(path, (uid_t) b, (uid_t) c);
	heapmm_free(path, sza);
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
SYSCALL_DEF2(truncate)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = _sys_truncate(path, (off_t) b);
	heapmm_free(path, sza);
	return (uint32_t) status;
}

SYSCALL_DEF1(umask)
{
	mode_t old = current_process->umask;
	current_process->umask = a & 0777;
	return (uint32_t) old;
}

//int vfs_mknod(char *path, mode_t mode, dev_t dev)
SYSCALL_DEF3(mknod)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = vfs_mknod(path, (mode_t) b, (dev_t) c);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, sza);
	return (uint32_t) status;
}

//int vfs_mkdir(char *path, mode_t mode)
SYSCALL_DEF2(mkdir)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = vfs_mkdir(path, (mode_t) b);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, sza);
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
SYSCALL_DEF2(stat)
{
	char *path;
	struct stat* buf;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_stat(path, buf);
	if (!copy_kern_to_user(buf, (void *)b, sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(path, sza);
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
SYSCALL_DEF3(readlink)
{
	char *path;
	char* buf;
	size_t bufsiz;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	bufsiz = c;
	if (bufsiz > CONFIG_FILE_MAX_NAME_LENGTH)
		bufsiz = CONFIG_FILE_MAX_NAME_LENGTH;
	buf = heapmm_alloc(bufsiz);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_readlink(path, buf, bufsiz);
	if (!copy_kern_to_user(buf, (void *)b, bufsiz)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		heapmm_free(buf, bufsiz);
		return (uint32_t) -1;
	}
	heapmm_free(path, sza);
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
SYSCALL_DEF2(lstat)
{
	char *path;
	struct stat* buf;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_lstat(path, buf);
	if (!copy_kern_to_user(buf, (void *)b, sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(path, sza);
	heapmm_free(buf, sizeof(struct stat));
	return (uint32_t) status;
}

SYSCALL_DEF1(rmdir)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = vfs_rmdir(path);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, sza);
	return (uint32_t) status;
}

SYSCALL_DEF1(access)
{
	char *path;
	int status;
	int sza;
	sza = procvmm_check_string((char*)a,CONFIG_FILE_MAX_NAME_LENGTH);
	if ((sza < 0)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sza);
	if (!copy_user_to_kern((void *)a, path, sza)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sza);
		return (uint32_t) -1;
	}
	status = vfs_access(path, (int) b);
	if (status != 0) {
		syscall_errno = status;
		status = -1;
	}
	heapmm_free(path, sza);
	return (uint32_t) status;
}

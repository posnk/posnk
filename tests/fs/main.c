#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "kernel/vfs.h"

void vfs_test_init();

void register_fs_drivers();

void device_block_init(char *image);

fs_device_t *vfs_test_mount(char *fstype, dev_t device, uint32_t flags);

inode_t *vfs_test_get_root(fs_device_t *fsdevice);
void device_block_stop();
void usage(char *reason)
{
	fprintf(stderr, "Syntax error: %s\nUsage: testfs <fs name> <image>\n", reason);
	exit(EXIT_FAILURE);
}
//65804
void test_fs(char *fs, uint32_t flags, dev_t dev)
{
	fs_device_t *instance;
	inode_t *root, *testino;	
	dirent_t *de;
	aoff_t write_size = 0;
	int status;
	uint8_t rbuffer[9];

	instance = vfs_test_mount(fs, dev, flags);
	
	assert (instance != NULL);
	
	root = vfs_test_get_root(instance);

	assert (root != NULL);	

	status = vfs_test_mknod(root, "testfile", S_IFREG | 0777, 0);

	if (status) {

		fprintf(stderr, "error: test_mknod(root, \"testfile\", S_IFREG | 0777, 0) failed with status %i (%s)\n", status, strerror(status));
		exit(EXIT_FAILURE);
	}

	status = root->device->ops->store_inode(root);	

	if (status) {

		fprintf(stderr, "error: store_inode(root) failed with status %i (%s)\n", status, strerror(status));
		exit(EXIT_FAILURE);
	}

	/*de = vfs_find_dirent(root, "testfile");

	if (!de) {

		fprintf(stderr, "error: could not find dirent \"testfile\"\n");
		exit(EXIT_FAILURE);
	}*/

	testino = instance->ops->load_inode(instance, 12);
	
	if (!testino) {

		fprintf(stderr, "error: could not load inode %i (\"testfile\")\n", (int) 12);
		exit(EXIT_FAILURE);
	}

	status = vfs_int_write(testino, "testtext", 1024*65806, 8, &write_size);

	if (status) {

		fprintf(stderr, "error: write(testino, \"testtext\", T_INDIR_START) failed with status %i (%s)\n", status, strerror(status));
		exit(EXIT_FAILURE);
	}

	if (write_size != 8) {

		fprintf(stderr, "error: write(testino, \"testtext\", T_INDIR_START) only wrote %i bytes\n",(int) write_size);
		exit(EXIT_FAILURE);
	}

	testino->size = 1024*65806 + 8;

	status = root->device->ops->store_inode(testino);	

	if (status) {

		fprintf(stderr, "error: store_inode(testino) failed with status %i (%s)\n", status, strerror(status));
		exit(EXIT_FAILURE);
	}

	status = vfs_int_read(testino, rbuffer, 1024*65806, 8, &write_size);

	if (status) {

		fprintf(stderr, "error: read(testino, \"testtext\", T_INDIR_START) failed with status %i (%s)\n", status, strerror(status));
		exit(EXIT_FAILURE);
	}

	if (write_size != 8) {

		fprintf(stderr, "error: read(testino, \"testtext\", T_INDIR_START) only read %i bytes\n",(int) write_size);
		exit(EXIT_FAILURE);
	}

	if (memcmp(rbuffer,"testtext",8)) {

		fprintf(stderr, "error: read(testino, \"testtext\", T_INDIR_START) yielded corrupt data\n");
		exit(EXIT_FAILURE);
	}

} 

int main(int argc, char **argv)
{
	char *fs_name, *image;
	if (argc != 3)
		usage("argument count");
	fs_name = argv[1];
	image = argv[2];
	device_block_init(image);
	vfs_test_init();
	register_fs_drivers();
	test_fs(fs_name, 0, 121);
	device_block_stop();
	return 0;
}

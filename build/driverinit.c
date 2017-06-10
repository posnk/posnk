#define FS_DRIVER(NaMe, PaTh) int NaMe ## _register();
#define PNP_DRIVER(NaMe, PaTh) int NaMe ## _register(); 
#define DEV_DRIVER(NaMe, PaTh) int NaMe ## _init();

#include "fs/fs.list"
#include "driver/dev.list"

#undef FS_DRIVER
#undef PNP_DRIVER
#undef DEV_DRIVER

void register_fs_drivers() {

#define FS_DRIVER(NaMe, PaTh) NaMe ## _register();

#include "fs/fs.list"

}
#ifndef HOSTED_TEST
void register_dev_drivers() {

#define DEV_DRIVER(NaMe, PaTh) NaMe ## _init();
#define PNP_DRIVER(NaMe, PaTh) NaMe ## _register();
#include "driver/dev.list"

}
#endif

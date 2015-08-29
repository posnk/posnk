define(`FS_DRIVER', `include(fs/$2/$1.list)dnl')dnl
define(`DRIVER_SRC', ``	driver/$1.c \'')dnl
define(`FS_SRC', ``	fs/$1.c \'')dnl
define(`DEV_DRIVER', `include(driver/$2/$1.list)dnl')dnl
define(`PNP_DRIVER', `include(driver/$2/$1.list)dnl')dnl
SRCS_DRIVER = _dinit.c \
include(`fs/fs.list')dnl
include(`driver/dev.list')dnl

SRCS_HOST_FS = _dinit.c \
include(`fs/fs.list')dnl

clean_driver:
define(`FS_DRIVER', `include(fs/$2/$1.list)dnl')dnl
define(`DRIVER_SRC', ``	$(RM) driver/$1.o'')dnl
define(`FS_SRC', ``	$(RM) fs/$1.o'')dnl
define(`DEV_DRIVER', `include(driver/$2/$1.list)dnl')dnl
define(`PNP_DRIVER', `include(driver/$2/$1.list)dnl')dnl
	$(RM) _dinit.o
include(`fs/fs.list')dnl
include(`driver/dev.list')dnl

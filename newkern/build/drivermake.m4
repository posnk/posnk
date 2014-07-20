define(`FS_DRIVER', ``	fs/$1.c \'')dnl
define(`DRIVER_SRC', ``	driver/$1 \'')dnl
define(`DEV_DRIVER', `include(driver/$2/$1.list)dnl')dnl
define(`PNP_DRIVER', `include(driver/$2/$1.list)dnl')dnl
SRCS_DRIVER = _dinit.c \
include(`fs/fs.list')dnl
include(`driver/dev.list')dnl

cmd_libbb/makedev.o := i386-pc-posnk-gcc -Wp,-MD,libbb/.makedev.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.22.1)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -g -O0  -march=i386 -mpreferred-stack-boundary=2    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(makedev)"  -D"KBUILD_MODNAME=KBUILD_STR(makedev)" -c -o libbb/makedev.o libbb/makedev.c

deps_libbb/makedev.o := \
  libbb/makedev.c \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config/big/endian.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/nommu.h) \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/include-fixed/limits.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/stdint.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/_default_types.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/features.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/include/stdbool.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/unistd.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/unistd.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/_ansi.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/newlib.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/config.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/ieeefp.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/types.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/_types.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/_types.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/lock.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/include/stddef.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/types.h \

libbb/makedev.o: $(deps_libbb/makedev.o)

$(deps_libbb/makedev.o):

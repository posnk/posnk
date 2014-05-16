cmd_libbb/remove_file.o := i386-pc-posnk-gcc -Wp,-MD,libbb/.remove_file.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.22.1)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -Os  -march=i386 -mpreferred-stack-boundary=2    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(remove_file)"  -D"KBUILD_MODNAME=KBUILD_STR(remove_file)" -c -o libbb/remove_file.o libbb/remove_file.c

deps_libbb/remove_file.o := \
  libbb/remove_file.c \
  include/libbb.h \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/use/bb/shadow.h) \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/feature/utmp.h) \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/use/bb/pwd/grp.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/feature/ipv6.h) \
    $(wildcard include/config/feature/seamless/xz.h) \
    $(wildcard include/config/feature/seamless/lzma.h) \
    $(wildcard include/config/feature/seamless/bz2.h) \
    $(wildcard include/config/feature/seamless/gz.h) \
    $(wildcard include/config/feature/seamless/z.h) \
    $(wildcard include/config/feature/check/names.h) \
    $(wildcard include/config/feature/prefer/applets.h) \
    $(wildcard include/config/long/opts.h) \
    $(wildcard include/config/feature/getopt/long.h) \
    $(wildcard include/config/feature/pidfile.h) \
    $(wildcard include/config/feature/syslog.h) \
    $(wildcard include/config/feature/individual.h) \
    $(wildcard include/config/echo.h) \
    $(wildcard include/config/printf.h) \
    $(wildcard include/config/test.h) \
    $(wildcard include/config/kill.h) \
    $(wildcard include/config/chown.h) \
    $(wildcard include/config/ls.h) \
    $(wildcard include/config/xxx.h) \
    $(wildcard include/config/route.h) \
    $(wildcard include/config/feature/hwib.h) \
    $(wildcard include/config/desktop.h) \
    $(wildcard include/config/feature/crond/d.h) \
    $(wildcard include/config/use/bb/crypt.h) \
    $(wildcard include/config/feature/adduser/to/group.h) \
    $(wildcard include/config/feature/del/user/from/group.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
    $(wildcard include/config/feature/editing.h) \
    $(wildcard include/config/feature/editing/history.h) \
    $(wildcard include/config/feature/editing/savehistory.h) \
    $(wildcard include/config/feature/tab/completion.h) \
    $(wildcard include/config/feature/username/completion.h) \
    $(wildcard include/config/feature/editing/vi.h) \
    $(wildcard include/config/feature/editing/save/on/exit.h) \
    $(wildcard include/config/pmap.h) \
    $(wildcard include/config/feature/show/threads.h) \
    $(wildcard include/config/feature/ps/additional/columns.h) \
    $(wildcard include/config/feature/topmem.h) \
    $(wildcard include/config/feature/top/smp/process.h) \
    $(wildcard include/config/killall.h) \
    $(wildcard include/config/pgrep.h) \
    $(wildcard include/config/pkill.h) \
    $(wildcard include/config/pidof.h) \
    $(wildcard include/config/sestatus.h) \
    $(wildcard include/config/unicode/support.h) \
    $(wildcard include/config/feature/mtab/support.h) \
    $(wildcard include/config/feature/clean/up.h) \
    $(wildcard include/config/feature/devfs.h) \
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
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/ctype.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/_ansi.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/dirent.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/dirent.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/errno.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/errno.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/reent.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/fcntl.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/fcntl.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/_default_fcntl.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/stat.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/time.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/time.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/inttypes.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/setjmp.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/setjmp.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/signal.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/signal.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/stdio.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/include/stdarg.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/stdio.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/stdlib.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/stdlib.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/alloca.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/string.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/cdefs.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/string.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/libgen.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/ioctl.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/time.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/wait.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/termios.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/termios.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/param.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/sys/syslimits.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/endian.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/machine/param.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/pwd.h \
  /home/peterbjornx/nk/gcc_posnk/lib/gcc/i386-pc-posnk/4.7.2/../../../../i386-pc-posnk/include/grp.h \
  include/xatonum.h \

libbb/remove_file.o: $(deps_libbb/remove_file.o)

$(deps_libbb/remove_file.o):

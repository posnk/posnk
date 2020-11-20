#
# 'make depend' uses makedepend to automatically generate dependencies
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

export

CROSS_COMPILE = $(TARGET)-
ARCH = $(shell scripts/getarch $(TARGET))
BUILDDIR = ./
ARCHCAP = `echo $(ARCH) | tr a-z A-Z`
ARCHDEF = -DARCH_$(ARCHCAP) -DARCH_NAME=\"$(ARCH)\"
VERDEF = -DBUILD_MACHINE=\"`whoami`@`uname -n`\"
DEFS = $(ARCHDEF) $(VERDEF)
ULDEFS = $(VERDEF) $(ARCHDEF)

# define the C compiler to use
CC   = @echo " [   CC    ]	" $< ; $(CROSS_COMPILE)gcc
LD   = @echo " [   LD    ]	" $@ ; $(CROSS_COMPILE)gcc
RLD  = @echo " [   LD    ]	" $@ ; $(CROSS_COMPILE)ld
CPP  = @echo " [   CPP   ]	" $@ ; $(CROSS_COMPILE)cpp
OCP  = @echo " [ OBJCPY  ]	" $@ ; $(CROSS_COMPILE)objcopy
AR   = @echo " [   AR    ]	" $@ ; $(CROSS_COMPILE)ar
RL   = @echo " [ RANLIB  ]	" $@ ; $(CROSS_COMPILE)ranlib
M4   = @echo " [   M4    ]	" $@ ; m4
GAS  = @echo " [   AS    ]	" $< ; $(CROSS_COMPILE)gcc
NASM = @echo " [  NASM   ]	" $< ; nasm
HCC  = @echo " [ HOSTCC  ]	" $< ; gcc
HLD  = @echo " [ HOSTLD  ]	" $@ ; gcc
HCPP = @echo " [ HOSTCPP ]	" $@ ; cpp
HGAS = @echo " [ HOSTAS  ]	" $@ ; as

# Set the compiler and assembler flags
CFLAGS = $(DEFS) -Wall -g -Wextra -fno-exceptions -ffreestanding \
	-fno-omit-frame-pointer -finline-functions -finline-functions-called-once \
	-fauto-inc-dec

ULCFLAGS = $(ULDEFS) -Wall -g -Wextra -fno-exceptions -ffreestanding \
	-finline-functions -finline-functions-called-once

ULARFLAGS = rcs

HCFLAGS = -Wall -g -Wextra -DHOSTED_TEST -include hostedtest.h \
	-mno-tbm -D_FILE_OFFSET_BITS=64

NASMFLAGS = -w+orphan-labels -felf -g

ULNASMFLAGS = -w+orphan-labels -felf -g

GASFLAGS = -g

# Set the correct include paths
INCLUDES = -I./include -I./include/crt

HINCLUDES = -I./include -I./include/hcrt

ULINCLUDES = -I./include/crt

# Set the required linker flags
LFLAGS = -g -ffreestanding -O2 -nostdlib -static-libgcc

HLFLAGS = -g -lfuse

# define any libraries to link into executable
LIBS = -lgcc

# define the source files for using Peter's kernel heap manager
SRCS_PHEAPMM = kernel/heapmm.c

# define the source files for using Doug Lea's malloc as kernel heap manager
SRCS_DLHEAPMM = kernel/dlmalloc.c \
		kernel/dlheapmm.c

# define the platform-independent source files
SRCS = kernel/physmm.c \
kernel/paging.c \
kernel/blkcache.c \
kernel/earlycon.c \
kernel/exception.c \
kernel/interrupt.c \
kernel/scheduler.c \
kernel/synch.c \
kernel/process.c \
kernel/signal.c \
kernel/procvmm.c \
kernel/permissions.c \
kernel/blkdev.c \
kernel/chardev.c \
kernel/tty.c \
kernel/pipe.c \
kernel/vfs/icache.c \
kernel/vfs/dcache.c \
kernel/vfs/ifswrap.c \
kernel/vfs/pathres.c \
kernel/vfs/perm.c \
kernel/vfs/ifsmgr.c \
kernel/vfs/mount.c \
kernel/vfs.c \
kernel/elfloader.c \
kernel/tar.c \
kernel/version.c \
kernel/syscall.c \
kernel/streams.c \
kernel/svipc/ipc.c \
kernel/svipc/shm.c \
kernel/svipc/sem.c \
kernel/svipc/msg.c \
kernel/time.c \
kernel/exec.c \
kernel/drivermgr.c \
kernel/sc_process.c \
kernel/sc_perm.c \
kernel/sc_vfs.c \
kernel/sc_ipc.c \
kernel/sc_streams.c \
kernel/sc_time.c \
kernel/sc_signals.c \
kernel/shutdown.c \
kernel/init/cmdline.c \
kernel/init/kinit.c \
kernel/init/uinit.c \
kernel/init/idle.c \
kernel/console/srcmap.c \
kernel/console/conmgr.c \
kernel/console/conput.c \
kernel/console/consink.c \
kdbg/heapdbg.c \
kdbg/taskdbg.c \
kdbg/kdbg.c \
kdbg/kdbgio.c \
kdbg/kdbgmm.c \
kdbg/stacktrc.c \
fs/mbr.c \
util/llist.c \
util/mruc.c \
crt/string.c \
crt/stdlib.c \
crt/numfmt.c \
crt/printf.c

# define the source files for the hosted filesystem test
TEST_FS_SRCS = tests/fs/mm.c \
tests/fs/synch.c \
tests/fs/sched.c \
tests/fs/img.c \
tests/fs/fuse.c \
kernel/vfs/icache.c \
kernel/vfs/dcache.c \
kernel/vfs/ifswrap.c \
kernel/vfs/pathres.c \
kernel/vfs/perm.c \
kernel/vfs/ifsmgr.c \
kernel/vfs/mount.c \
kernel/blkcache.c \
kernel/blkdev.c \
kernel/chardev.c \
kernel/permissions.c \
kernel/time.c \
kernel/vfs.c \
kernel/pipe.c \
kernel/streams.c \
kernel/sc_vfs.c \
tests/fs/main.c \
kernel/drivermgr.c \
util/mruc.c \
util/llist.c

# define the sources for the hosted MRU cache tests
TEST_MRUC_SRCS = tests/test_mruc.c \
util/llist.c \
util/mruc.c

# define the sources for the hosted error passing test
SRCS_TESTERROR = tests/test_error.c

# define the standalone test sources
TESTSRCS = tests/test_heapmm.c tests/test_physmm.c

# define the default targets

.PHONY: depend clean

default: default_$(ARCH)

all:	default

# include the userlib makefile

include userlib/Makefile

# include the architecture makefile

include arch/$(ARCH)/Makefile

# generate object file lists for arch-independent parts of the kernel

OBJS          = $(addprefix $(BUILDDIR),$(SRCS:.c=.o))
OBJS_DRIVER   = $(addprefix $(BUILDDIR),$(SRCS_DRIVER:.c=.o))
OBJS_DLHEAPMM = $(addprefix $(BUILDDIR),$(SRCS_DLHEAPMM:.c=.o))
OBJS_USERLIB  = $(addprefix $(BUILDDIR),$(SRCS_USERLIB:.c=.o))

# rules to compile those objects

$(OBJS): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $ -c $< -o $@

$(OBJS_USERLIB): $(BUILDDIR)%.o: %.c
	$(CC) $(ULCFLAGS) $(ULINCLUDES) $ -c $< -o $@

$(OBJS_DLHEAPMM): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_PHEAPMM): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# build tree creation

# dynamic makefile and source generation

$(BUILDDIR)_dmake: build/driverinit.c build/drivermake.m4 fs/fs.list
	find -type d -links 2 -exec mkdir -p "$(BUILDDIR){}" \; 2> /dev/null
	$(M4) -I . build/drivermake.m4 > $(BUILDDIR)_dmake
	$(CPP) -I . -I ./include build/driverinit.c > _dinit.c

# include dynamic makefile

include $(BUILDDIR)_dmake

# driver compilation rule

$(OBJS_DRIVER): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# generate full kernel object list
OBJS_KERN = $(BUILDDIR)arch/$(ARCH).o $(OBJS) $(OBJS_DLHEAPMM) $(OBJS_DRIVER)

# kernel linking rule

$(BUILDDIR)vmpos: $(BUILDDIR)_dmake $(OBJS_KERN)
	$(LD) $(LFLAGS) -T arch/$(ARCH)/kern.ld -o $@ $(OBJS_KERN) $(LIBS)
	@rm $(BUILDDIR)_dmake $(BUILDDIR)kernel/version.o

# userlib archiving rule
$(BUILDDIR)libposnk.a: $(BUILDDIR)_dmake $(OBJS_USERLIB) $(OBJS_ULARCH)
	$(AR) $(ULARFLAGS) $(BUILDDIR)libposnk.a $(OBJS_USERLIB) $(OBJS_ULARCH)
	$(RL) $(BUILDDIR)libposnk.a

# kernel install rule

install: $(BUILDDIR)vmpos
	install $(BUILDDIR)vmpos $(DESTDIR)/boot/vmpos
	install $(BUILDDIR)vmpos $(DESTDIR)/boot/vmpos_unstripped
	strip $(DESTDIR)/boot/vmpos

install_h:
	@cp include/crt/sys/ioctl.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/ioctl.h
	@cp include/crt/sys/termios.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/termios.h
	@cp include/crt/sys/mman.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/mman.h
	@cp include/crt/sys/ipc.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/ipc.h
	@cp include/crt/sys/shm.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/shm.h
	@cp include/crt/sys/sem.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/sem.h
	@cp include/crt/sys/msg.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/msg.h
	@cp include/crt/linux/input.h ../gcc_posnk/i386-pc-posnk/include/linux/


#test_heapmm: $(OBJS) tests/test_heapmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_heapmm $(OBJS) tests/test_heapmm.o $(LFLAGS) $(LIBS)

#test_physmm: $(OBJS) tests/test_physmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_physmm $(OBJS) tests/test_physmm.o $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# (see the gnu make manual section about automatic variables)

clean: clean_driver
	@$(RM) *.o *~ $(MAIN)
	@$(RM) fs/*.o
	@$(RM) kernel/vfs/*.o
	@$(RM) kdbg/*.o
	@$(RM) kernel/*.o
	@$(RM) driver/console/*.o
	@$(RM) driver/bus/*.o
	@$(RM) arch/i386/*.o
	@$(RM) arch/armv7/*.o
	@$(RM) arch/armv7/*.so
	@$(RM) arch/armv7/*.ao
	@$(RM) arch/armv7/*.bo
	@$(RM) arch/armv7/loader/*.o
	@$(RM) arch/armv7/loader/*.so
	@$(RM) arch/armv7/loader/*.ao
	@$(RM) *.bo
	@$(RM) util/*.o
	@$(RM) crt/*.o
	@rm _dmake
	@rm loader_armv7*

clean_test:
	$(RM) test/fs/*.ho
	$(RM) fs/*.ho
	$(RM) kernel/*.ho
	$(RM) kernel/vfs/*.ho
	$(RM) driver/console/*.ho
	$(RM) driver/bus/*.ho
	$(RM) arch/i386/*.ho

depend: $(SRCS) $(TESTSRCS) $(SRCS_I386)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


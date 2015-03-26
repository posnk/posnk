#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CROSS_COMPILE = /opt/standalone/bin/i586-elf-
ARCH = i386

# define the C compiler to use
CC   = @echo " [  CC  ]	" $< ; $(CROSS_COMPILE)gcc
LD   = @echo " [  LD  ]	" $@ ; $(CROSS_COMPILE)gcc
CPP  = @echo " [  CPP ]	" $@ ; $(CROSS_COMPILE)cpp
OCP  = @echo " [OBJCPY]	" $@ ; $(CROSS_COMPILE)objcopy
M4   = @echo " [  M4  ]	" $@ ; m4
GAS  = @echo " [  AS  ]	" $@ ; $(CROSS_COMPILE)gcc
NASM = @echo " [ NASM ]	" $< ; nasm
HCC  = @echo " [ HCC  ]	" $< ; gcc
HLD  = @echo " [ HLD  ]	" $@ ; gcc
HCPP = @echo " [ HCPP ]	" $@ ; cpp
HGAS = @echo " [  HAS ]	" $@ ; as

# define any compile-time flags
CFLAGS = -Wall -g -Wextra -fno-exceptions -ffreestanding -fno-omit-frame-pointer -finline-functions -finline-functions-called-once -fauto-inc-dec

HCFLAGS = -Wall -g -Wextra -DHOSTED_TEST -include hostedtest.h

NASMFLAGS = -w+orphan-labels -felf -g

GASFLAGS = -g

MKIFLAGS = -A arm -O linux -C none 

ARMV7_LOAD_ADDR = 80008000
ARMV7_ENTRY_POINT = 80008000

# define any directories containing header files other than /usr/include
#
INCLUDES = -I./include -I./include/crt

HINCLUDES = -I./include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -g -T linker.ld -ffreestanding -O2 -nostdlib -static-libgcc
A7LFLAGS = -g -ffreestanding -O2 -nostdlib -static-libgcc

HLFLAGS = -g

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = 

SRCS_PHEAPMM = kernel/heapmm.c

SRCS_DLHEAPMM = kernel/dlmalloc.c \
		kernel/dlheapmm.c

# define the C source files
SRCS = kernel/physmm.c \
kernel/paging.c \
kernel/blkcache.c \
kernel/earlycon.c \
kernel/exception.c \
kernel/interrupt.c \
kernel/scheduler.c \
kernel/synch.c \
kernel/process.c \
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
kernel/vfs.c \
kernel/elfloader.c \
kernel/tar.c \
kernel/syscall.c \
kernel/streams.c \
kernel/ipc.c \
kernel/shm.c \
kernel/sem.c \
kernel/msg.c \
kernel/time.c \
kernel/drivermgr.c \
kernel/sc_process.c \
kernel/sc_perm.c \
kernel/sc_vfs.c \
kernel/sc_ipc.c \
kernel/sc_streams.c \
kernel/sc_time.c \
kernel/sc_signals.c \
kernel/shutdown.c \
kdbg/heapdbg.c \
kdbg/taskdbg.c \
kdbg/kdbg.c \
kdbg/kdbgio.c \
kdbg/kdbgmm.c \
kdbg/stacktrc.c \
fs/mbr.c \
util/llist.c \
crt/string.c \
crt/stdlib.c 

SRCS_I386 = arch/i386/x86.c \
arch/i386/paging.c \
arch/i386/init.c \
arch/i386/isr.c \
arch/i386/idt.c \
arch/i386/pic.c \
arch/i386/pit.c \
arch/i386/fpu.c \
arch/i386/task_switch.c \
arch/i386/protection.c \
driver/console/vgacon.c \
driver/console/sercon.c \
driver/console/vgacon_early.c \
driver/console/sercon_debug.c \
driver/bus/pci.c \
driver/bus/pci_intel_host.c \
arch/i386/fastcrt/memcpy.S \
arch/i386/fastcrt/memchr.S \
arch/i386/fastcrt/memmove.S \
arch/i386/fastcrt/memset.S \
arch/i386/fastcrt/strchr.S \
arch/i386/fastcrt/strlen.S 

ASMS_I386 = arch/i386/start.s \
arch/i386/isr_entry.s \
arch/i386/phys_copy.s \
arch/i386/contextswitch.s \
arch/i386/prot_asm.s

SRCS_ARMV7KERNEL = arch/armv7/init.c \
arch/armv7/loader/sercon.c \
arch/armv7/isr.c \
arch/armv7/paging.c \
arch/armv7/task_switch.c \
arch/armv7/debug.c \
driver/platform/omap3430/mpu_intc.c \
driver/platform/omap3430/omap3430.c \
driver/platform/omap3430/gptimer.c \
$(SRCS) $(SRCS_DLHEAPMM)

ASMS_ARMV7KERNEL = arch/armv7/mode.S \
arch/armv7/exception.S \
arch/armv7/mmu.S \
arch/armv7/cswitch.S

SRCS_ARMV7LOADER = arch/armv7/loader/init.c \
arch/armv7/loader/sercon.c \
arch/armv7/loader/mmu.c \
arch/armv7/loader/earlycon.c \
arch/armv7/loader/elf.c \
arch/armv7/atags.c \
kernel/physmm.c \
util/llist.c \
crt/string.c \
crt/stdlib.c 

ASMS_ARMV7LOADER = arch/armv7/loader/start.S \
arch/armv7/mode.S \
arch/armv7/exception.S \
arch/armv7/mmu.S \
arch/armv7/loader/chain.S

TEST_FS_SRCS = tests/fs/mm.c \
tests/fs/dev.c \
tests/fs/synch.c \
tests/fs/vfs.c \
tests/fs/main.c \
kernel/drivermgr.c \
util/llist.c

SRCS_TESTERROR = tests/test_error.c

TESTSRCS = tests/test_heapmm.c tests/test_physmm.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o) $(SRCS_DLHEAPMM:.c=.o)
OBJS_I386 = $(SRCS_I386:.c=.o) $(ASMS_I386:.s=.o)
OBJS_ARMV7LOADER = $(SRCS_ARMV7LOADER:.c=.o) $(ASMS_ARMV7LOADER:.S=.ao) payload_armv7.bo
OBJS_ARMV7KERNEL = $(SRCS_ARMV7KERNEL:.c=.o) $(ASMS_ARMV7KERNEL:.S=.ao)

OBJS_TESTFS = $(TEST_FS_SRCS:.c=.ho) 

.PHONY: depend clean

default: default_$(ARCH)

default_i386: cdrom_i386 install_h

default_armv7: mmc_armv7

all:	default 

_dmake: build/driverinit.c build/drivermake.m4 fs/fs.list
	$(M4) -I . build/drivermake.m4 > _dmake
	$(CPP) -I . build/driverinit.c > _dinit.c

test_dmake:	
	$(HCPP) -DHOSTED_TEST -I . build/driverinit.c > _dinit.c
	$(M4) -I . build/drivermake.m4 > _dmake

include _dmake

OBJS_DRIVER = $(SRCS_DRIVER:.c=.o)

OBJS_HOST_FS = $(SRCS_HOST_FS:.c=.ho)

OBJS_TESTERROR = $(SRCS_TESTERROR:.c=.ho)

testfs: test_dmake $(OBJS_TESTFS) $(OBJS_HOST_FS)
	$(HLD) $(HLFLAGS) -o testfs $(OBJS_TESTFS) $(OBJS_HOST_FS)

testerror: $(OBJS_TESTERROR)
	$(HLD) $(HLFLAGS) -o testerror $(OBJS_TESTERROR)

test: testfs testerror
	@echo " [ TEST ] error.h"; ./testerror
	@touch empty
	@dd if=/dev/zero of=ext2test.ext2 bs=1024 count=100k
	@echo " [MKE2FS] " ext2test.ext2 ; yes | /sbin/mke2fs ./ext2test.ext2 -L posnk_e2_test -t ext2 -O -filetype,-sparse_super,-ext_attr,-resize_inode -b 1024 > /dev/null
	@echo " [TESTFS] " ext2 ext2test.ext2 ; ./testfs ext2 ext2test.ext2
	@echo " [E2FSCK] " ext2test.ext2 ; /sbin/e2fsck -f ./ext2test.ext2
	@rm empty

mmc_armv7: _dmake loader_armv7_uboot initrd_armv7_uboot
	@dd if=/dev/zero of=posnk.sd bs=1024 count=100k 2> /dev/null
	@echo " [MKE2FS] " posnk.sd ; yes | /sbin/mke2fs ./posnk.sd -L posnk_sd -t ext2 -O -filetype,-sparse_super,-ext_attr,-resize_inode -b 1024 > /dev/null
	@echo " [MOUNT ] " posnk.sd ../hdd ; sudo mount -o loop -t ext2 posnk.sd ../hdd
	@echo " [  CP  ] " loader_armv7_uboot ; cp loader_armv7_uboot ../hdd/posldr
	@echo " [  CP  ] " initrd_armv7_uboot ; cp initrd_armv7_uboot ../hdd/posrd
	@echo " [UMOUNT] " ../hdd ; sleep 0.2; sudo umount ../hdd
 	
payload_armv7: _dmake $(OBJS_ARMV7KERNEL) $(OBJS_DRIVER) arch/armv7/kern.ld
	$(LD) -T arch/armv7/kern.ld $(A7LFLAGS) $(LIBS) -o payload_armv7 $(OBJS_ARMV7KERNEL) $(OBJS_DRIVER) -lgcc
	@rm _dmake

payload_armv7.bo: payload_armv7
	$(OCP) -I binary -O elf32-littlearm -B armv4t payload_armv7 payload_armv7.bo
	
initrd_armv7_uboot: initrd_armv7.tar
	@echo " [MKIMG ] " initrd_armv7_uboot ; mkimage $(MKIFLAGS) -T ramdisk -n posrd -d initrd_armv7.tar initrd_armv7_uboot

loader_armv7_uboot: loader_armv7.bin
	@echo " [MKIMG ] " loader_armv7_uboot ; mkimage $(MKIFLAGS) -T kernel -a $(ARMV7_LOAD_ADDR) -e $(ARMV7_ENTRY_POINT) -n posnk_loader -d loader_armv7.bin loader_armv7_uboot

loader_armv7.bin: loader_armv7	
	$(OCP) -I elf32-littlearm -O binary --gap-fill 0xFF --set-start 0x$(ARMV7_LOAD_ADDR) -j .setup loader_armv7 loader_armv7.bin

loader_armv7: $(OBJS_ARMV7LOADER) arch/armv7/loader/ldr.ld
	$(LD) -T arch/armv7/loader/ldr.ld $(A7LFLAGS) $(LIBS) -o loader_armv7 $(OBJS_ARMV7LOADER) -lgcc
	

kernel_i386: _dmake $(OBJS) $(OBJS_I386) $(OBJS_DRIVER)
	$(LD) $(LFLAGS) $(LIBS) -o vmpos $(OBJS) $(OBJS_I386) $(OBJS_DRIVER)
	@rm _dmake

install_h:
	@cp include/crt/sys/ioctl.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/ioctl.h
	@cp include/crt/sys/termios.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/termios.h
	@cp include/crt/sys/mman.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/mman.h
	@cp include/crt/sys/ipc.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/ipc.h
	@cp include/crt/sys/shm.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/shm.h
	@cp include/crt/sys/sem.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/sem.h
	@cp include/crt/sys/msg.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/msg.h
	@cp include/crt/linux/input.h ../gcc_posnk/i386-pc-posnk/include/linux/

cdrom_i386: kernel_i386 initrd
	@echo " [  TAR ]	initrd.tar" ; ./mkinitrd_pos
	@cp vmpos cdrom_files/vmpos
	@echo " [ MKCD ]	pos.iso" ; grub-mkrescue -o pos.iso cdrom_files


#test_heapmm: $(OBJS) tests/test_heapmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_heapmm $(OBJS) tests/test_heapmm.o $(LFLAGS) $(LIBS)

#test_physmm: $(OBJS) tests/test_physmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_physmm $(OBJS) tests/test_physmm.o $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -marm -c $<  -o $@

%.ao: %.S
	$(GAS) $(GASFLAGS) $(INCLUDES) -marm -c $<  -o $@

%.ho: %.c
	$(HCC) $(HCFLAGS) $(HINCLUDES) -c $<  -o $@

.s.o:
	$(NASM) $(NASMFLAGS) $< -o $@ 

clean: clean_driver
	@$(RM) *.o *~ $(MAIN)
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
	$(RM) driver/console/*.ho
	$(RM) driver/bus/*.ho
	$(RM) arch/i386/*.ho

emulate: 
# cdrom_i386
	clear;qemu -m 1024 -cdrom pos.iso -hda hdd.img -s -boot order=dca -vga std $(EMUFLAGS)


depend: $(SRCS) $(TESTSRCS) $(SRCS_I386)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


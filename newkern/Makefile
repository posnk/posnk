#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = @echo " [  CC  ]	" $< ; /opt/standalone/bin/i586-elf-gcc
LD = @echo " [  LD  ]	" $@ ; /opt/standalone/bin/i586-elf-gcc

NASM = @echo " [ NASM ]	" $< ; nasm

# define any compile-time flags
CFLAGS = -Wall -g -Wextra -fno-exceptions -ffreestanding -fno-omit-frame-pointer

NASMFLAGS = -w+orphan-labels -felf -g

# define any directories containing header files other than /usr/include
#
INCLUDES = -I./include -I./include/crt

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -g -T linker.ld -ffreestanding -O2 -nostdlib -static-libgcc

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = 

# define the C source files
SRCS = kernel/heapmm.c \
kernel/physmm.c \
kernel/paging.c \
kernel/blkcache.c \
kernel/earlycon.c \
kernel/exception.c \
kernel/scheduler.c \
kernel/synch.c \
kernel/process.c \
kernel/procvmm.c \
kernel/permissions.c \
kernel/blkdev.c \
kernel/chardev.c \
kernel/tty.c \
kernel/pipe.c \
kernel/vfs.c \
kernel/elfloader.c \
kernel/tar.c \
kernel/syscall.c \
kernel/streams.c \
kernel/time.c \
kernel/sc_process.c \
kernel/sc_perm.c \
kernel/sc_vfs.c \
kernel/sc_streams.c \
kernel/sc_time.c \
kernel/sc_signals.c \
kdbg/heapdbg.c \
kdbg/kdbg.c \
kdbg/kdbgio.c \
kdbg/kdbgmm.c \
kdbg/stacktrc.c \
fs/ramfs.c \
util/llist.c \
crt/string.c \
crt/stdlib.c 

SRCS_DRV_VTCON = driver/console/vterm/vterm.c \
driver/console/vterm/vterm_colors.c \
driver/console/vterm/vterm_csi.c \
driver/console/vterm/vterm_csi_CUP.c \
driver/console/vterm/vterm_csi_CUx.c \
driver/console/vterm/vterm_csi_DCH.c \
driver/console/vterm/vterm_csi_DECSTBM.c \
driver/console/vterm/vterm_csi_DL.c \
driver/console/vterm/vterm_csi_ECH.c \
driver/console/vterm/vterm_csi_ED.c \
driver/console/vterm/vterm_csi_EL.c \
driver/console/vterm/vterm_csi_ICH.c \
driver/console/vterm/vterm_csi_IL.c \
driver/console/vterm/vterm_csi_RESTORECUR.c \
driver/console/vterm/vterm_csi_SAVECUR.c \
driver/console/vterm/vterm_csi_SGR.c \
driver/console/vterm/vterm_dec_RM.c \
driver/console/vterm/vterm_dec_SM.c \
driver/console/vterm/vterm_erase.c \
driver/console/vterm/vterm_escape.c \
driver/console/vterm/vterm_misc.c \
driver/console/vterm/vterm_render.c \
driver/console/vterm/vterm_resize.c \
driver/console/vterm/vterm_scroll.c \
driver/console/vterm/vterm_tty.c \
driver/console/vterm/vterm_write.c \
driver/console/vga_vterm.c

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
driver/oldkb/dec.c \
driver/oldkb/i8042.c \
driver/oldkb/kbd.c 

ASMS_I386 = arch/i386/start.s \
arch/i386/isr_entry.s \
arch/i386/phys_copy.s \
arch/i386/contextswitch.s \
arch/i386/prot_asm.s

TESTSRCS = tests/test_heapmm.c tests/test_physmm.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o) $(SRCS_DRV_VTCON:.c=.o)
OBJS_I386 = $(SRCS_I386:.c=.o) $(ASMS_I386:.s=.o)  
all:	cdrom_i386 install_h

kernel_i386: $(OBJS) $(OBJS_I386)
	$(LD) $(LFLAGS) $(LIBS) -o vmpos $(OBJS) $(OBJS_I386)

install_h:
	@cp include/crt/sys/ioctl.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/ioctl.h
	@cp include/crt/sys/termios.h ../nkgcc/newlib-2.1.0/newlib/libc/sys/posnk/sys/termios.h

cdrom_i386: kernel_i386 initrd
	@echo " [  TAR ]	initrd.tar" ; ./mkinitrd_pos
	@cp vmpos cdrom_files/vmpos
	@echo " [ MKCD ]	pos.iso" ; grub-mkrescue -o pos.iso cdrom_files


#test_heapmm: $(OBJS) tests/test_heapmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_heapmm $(OBJS) tests/test_heapmm.o $(LFLAGS) $(LIBS)

#test_physmm: $(OBJS) tests/test_physmm.o
#	$(CC) $(CFLAGS) $(INCLUDES) -o test_physmm $(OBJS) tests/test_physmm.o $(LFLAGS) $(LIBS)

.PHONY: depend clean

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

.s.o:
	$(NASM) $(NASMFLAGS) $< -o $@ 

clean:
	$(RM) *.o *~ $(MAIN)
	$(RM) kernel/*.o
	$(RM) fs/*.o
	$(RM) arch/i386/*.o
	$(RM) driver/console/*.o

depend: $(SRCS) $(TESTSRCS) $(SRCS_I386)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


SRCS_C_I386 = arch/i386/x86.c \
arch/i386/paging.c \
arch/i386/init.c \
arch/i386/isr.c \
arch/i386/idt.c \
arch/i386/signal.c \
arch/i386/fpu.c \
arch/i386/task_switch.c \
arch/i386/protection.c \
arch/i386/gdbstub.c \
driver/console/sercon_debug.c \
driver/bus/pci.c \
driver/bus/pci_intel_host.c

SRCS_A_I386 = arch/i386/fastcrt/memcpy.S \
arch/i386/fastcrt/memchr.S \
arch/i386/fastcrt/memmove.S \
arch/i386/fastcrt/memset.S \
arch/i386/fastcrt/strchr.S \
arch/i386/fastcrt/strlen.S

SRCS_N_I386 = arch/i386/start.s \
arch/i386/isr_entry.s \
arch/i386/phys_copy.s \
arch/i386/contextswitch.s \
arch/i386/prot_asm.s \
arch/i386/synch.s

SRCS_ULN_I386 = userlib/i386/syscall.s

OBJS_C_I386 = $(addprefix $(BUILDDIR),$(SRCS_C_I386:.c=.o))
OBJS_N_I386 = $(addprefix $(BUILDDIR),$(SRCS_N_I386:.s=.o))
OBJS_A_I386 = $(addprefix $(BUILDDIR),$(SRCS_A_I386:.S=.o))

OBJS_ULN_I386 = $(addprefix $(BUILDDIR),$(SRCS_ULN_I386:.s=.o))

OBJS_I386 = $(OBJS_C_I386) $(OBJS_N_I386) $(OBJS_A_I386)

$(OBJS_C_I386): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_N_I386): $(BUILDDIR)%.o: %.s
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJS_A_I386): $(BUILDDIR)%.o: %.S
	$(GAS) $(GASFLAGS) -c $< -o $@

$(OBJS_ULN_I386): $(BUILDDIR)%.o: %.s
	$(NASM) $(ULNASMFLAGS) $< -o $@

OBJS_ULARCH = $(OBJS_ULN_I386)

ARCHOBJ=$(BUILDDIR)arch/i386.o

$(ARCHOBJ): $(OBJS_I386)
	$(RLD) $(RLDFLAGS) -r $(OBJS_I386) -o $@

default_i386: $(BUILDDIR)vmpos $(BUILDDIR)libposnk.a

cdrom_i386: vmpos cdrom_files/ramdisk.ext2
#	@echo " [  TAR ]	initrd.tar" ; ./mkinitrd_pos
	@cp vmpos cdrom_files/vmpos
	@echo " [ MKCD ]	pos.iso" ; grub-mkrescue -o pos.iso cdrom_files


# ARMv7 Architecture Obj C source files
SRCS_C_ARMV7 = arch/armv7/init.c \
arch/armv7/isr.c \
arch/armv7/paging.c \
arch/armv7/task_switch.c \
arch/armv7/signal.c \
arch/armv7/gdbstub.c \
arch/armv7/debug.c

# ARMv7 Architecture Obj Asm source files
SRCS_A_ARMV7 = arch/armv7/mode.S \
arch/armv7/exception.S \
arch/armv7/mmu.S \
arch/armv7/cswitch.S

# ARMv7 Bootloader C source files
SRCS_BLC_ARMV7 = arch/armv7/loader/init.c \
arch/armv7/loader/sercon.c \
arch/armv7/loader/mmu.c \
arch/armv7/loader/elf.c \
arch/armv7/atags.c \
kernel/physmm.c \
kernel/console/conmgr.c \
kernel/console/conput.c \
kernel/console/consink.c \
kernel/console/srcmap.c \
kdbg/ubsan.c \
util/llist.c \
crt/string.c \
crt/printf.c \
crt/numfmt.c \
crt/stdlib.c


# ARMv7 Bootloader Asm source files
SRCS_BLA_ARMV7 = arch/armv7/loader/start.S \
arch/armv7/mode.S \
arch/armv7/exception.S \
arch/armv7/mmu.S \
arch/armv7/loader/chain.S


OBJS_C_ARMV7 = $(addprefix $(BUILDDIR),$(SRCS_C_ARMV7:.c=.o))
OBJS_A_ARMV7 = $(addprefix $(BUILDDIR),$(SRCS_A_ARMV7:.S=.o))
OBJS_BLC_ARMV7 = $(addprefix $(BUILDDIR),$(SRCS_BLC_ARMV7:.c=.lo))
OBJS_BLA_ARMV7 = $(addprefix $(BUILDDIR),$(SRCS_BLA_ARMV7:.S=.lo))

$(OBJS_C_ARMV7): $(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_A_ARMV7): $(BUILDDIR)%.o: %.S
	$(GAS) $(GASFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_BLC_ARMV7): $(BUILDDIR)%.lo: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_BLA_ARMV7): $(BUILDDIR)%.lo: %.S
	$(GAS) $(GASFLAGS) $(INCLUDES) -c $< -o $@


OBJ_WK_ARMV7 = $(BUILDDIR)arch/armv7/loader/payload.o

ARMV7_LDR_ELF =  $(BUILDDIR)arch/armv7/loader/vmpos_ldr.elf
ARMV7_LDR_BIN =  $(BUILDDIR)arch/armv7/loader/vmpos_ldr.bin
ARMV7_LDR_IMG =  $(BUILDDIR)arch/armv7/loader/vmpos.img


OBJS_ARMV7    = $(OBJS_C_ARMV7) $(OBJS_A_ARMV7)

OBJS_BL_ARMV7 = $(OBJS_BLC_ARMV7) $(OBJS_BLA_ARMV7) $(OBJ_WK_ARMV7)


ARCHOBJ=$(BUILDDIR)arch/armv7.o

$(ARCHOBJ): $(OBJS_ARMV7)
	$(RLD) $(RLDFLAGS) -r $(OBJS_ARMV7) -o $@

default_armv7: $(ARMV7_LDR_IMG)
#$(BUILDDIR)libposnk.a

MKIFLAGS = -A arm -O linux -C none
MKIMAGE  = @echo " [MKIMG ] " $@ ; mkimage
ARMV7_LOAD_ADDR = 80008000
ARMV7_ENTRY_POINT = 80008000

# Wrap kernel ELF into obj file to link into loader
$(OBJ_WK_ARMV7): $(BUILDDIR)vmpos
	@cd $(BUILDDIR);$(OCP) -I binary -O elf32-littlearm -B armv4t vmpos $(OBJ_WK_ARMV7)

# Link the loader
$(ARMV7_LDR_ELF): $(OBJS_BL_ARMV7) arch/armv7/loader/ldr.ld
	$(LD) -T arch/armv7/loader/ldr.ld $(LFLAGS) $(LIBS) -o $@ $(OBJS_BL_ARMV7) -lgcc

# Create a loadable binary from the loader-kernel ELF
$(ARMV7_LDR_BIN): $(ARMV7_LDR_ELF)
	$(OCP) -I elf32-littlearm -O binary --gap-fill 0xFF --set-start 0x$(ARMV7_LOAD_ADDR) -j .setup $(ARMV7_LDR_ELF) $@

# Create a UBoot image from the loader-kernel BIN
$(ARMV7_LDR_IMG): $(ARMV7_LDR_BIN)
	$(MKIMAGE) $(MKIFLAGS) -T kernel -a $(ARMV7_LOAD_ADDR) -e $(ARMV7_ENTRY_POINT) -n posnk_loader -d $(ARMV7_LDR_BIN) $@


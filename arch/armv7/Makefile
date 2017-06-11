
MKIFLAGS = -A arm -O linux -C none

ARMV7_LOAD_ADDR = 80008000
ARMV7_ENTRY_POINT = 80008000

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

OBJS_ARMV7LOADER = $(SRCS_ARMV7LOADER:.c=.o) $(ASMS_ARMV7LOADER:.S=.ao) payload_armv7.bo
OBJS_ARMV7KERNEL = $(SRCS_ARMV7KERNEL:.c=.o) $(ASMS_ARMV7KERNEL:.S=.ao)

mmc_armv7: _dmake loader_armv7_uboot initrd_armv7_uboot
	@dd if=/dev/zero of=posnk.sd bs=1024 count=100k 2> /dev/null
	@echo " [MKE2FS] " posnk.sd ; yes | /sbin/mke2fs ./posnk.sd -L posnk_sd -t ext2 -O none -b 1024 > /dev/null
	@echo " [MOUNT ] " posnk.sd ../hdd ; sudo mount -o loop -t ext2 posnk.sd ../hdd
	@echo " [  CP  ] " loader_armv7_uboot ; cp loader_armv7_uboot ../hdd/posldr
	@echo " [  CP  ] " initrd_armv7_uboot ; cp initrd_armv7_uboot ../hdd/posrd
	@echo " [UMOUNT] " ../hdd ; sleep 0.2; sudo umount ../hdd
 	
payload_armv7: _dmake $(OBJS_ARMV7KERNEL) $(OBJS_DRIVER) arch/armv7/kern.ld
	$(LD) -T arch/armv7/kern.ld $(A7LFLAGS) $(LIBS) -o payload_armv7 $(OBJS_ARMV7KERNEL) $(OBJS_DRIVER) -lgcc
	@rm _dmake

payload_armv7.bo: payload_armv7
	$(OCP) -I binary -O elf32-littlearm -B armv4t payload_armv7 payload_armv7.bo
	
initrd_armv7_uboot: initrd
	@echo " [  TAR ]	initrd.tar" ; ./scripts/mkinitrd_pos
	@echo " [MKIMG ] " initrd_armv7_uboot ; mkimage $(MKIFLAGS) -T ramdisk -n posrd -d cdrom_files/initrd.tar initrd_armv7_uboot

loader_armv7_uboot: loader_armv7.bin
	@echo " [MKIMG ] " loader_armv7_uboot ; mkimage $(MKIFLAGS) -T kernel -a $(ARMV7_LOAD_ADDR) -e $(ARMV7_ENTRY_POINT) -n posnk_loader -d loader_armv7.bin loader_armv7_uboot

loader_armv7.bin: loader_armv7	
	$(OCP) -I elf32-littlearm -O binary --gap-fill 0xFF --set-start 0x$(ARMV7_LOAD_ADDR) -j .setup loader_armv7 loader_armv7.bin

loader_armv7: $(OBJS_ARMV7LOADER) arch/armv7/loader/ldr.ld
	$(LD) -T arch/armv7/loader/ldr.ld $(A7LFLAGS) $(LIBS) -o loader_armv7 $(OBJS_ARMV7LOADER) -lgcc
	

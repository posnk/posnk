/**
 * arch/i386/paging.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "kernel/earlycon.h"
#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "arch/i386/pic.h"
#include "arch/i386/pit.h"
#include "arch/i386/x86.h"
#include "arch/i386/idt.h"
#include "arch/i386/paging.h"
#include "arch/i386/multiboot.h"
#include "arch/i386/protection.h"
#include "arch/i386/vbe.h"
#include "driver/bus/pci.h"
#include "driver/block/ramblk.h"
#include "kernel/syscall.h"
#include "kernel/streams.h"
#include "kernel/vfs.h"
#include "kernel/tar.h"
#include "kernel/ipc.h"
#include "kernel/device.h"
#include "kernel/tty.h"
#include "kernel/drivermgr.h"
#include "kernel/interrupt.h"
#include "kernel/version.h"
#include "kdbg/dbgapi.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

multiboot_info_t *mb_info;
extern int ata_interrupt_enabled;

//XXX: Dirty Hack
vbe_mode_info_t	  vbe_mode;

extern uint32_t i386_start_kheap;

uint32_t i386_init_multiboot_memprobe(multiboot_info_t* mbt)
{
	uint32_t available = 0;
	multiboot_memory_map_t* mmapr= (multiboot_memory_map_t*) (((uintptr_t)mbt->mmap_addr) + 0xC0000000) ;
	multiboot_memory_map_t* mmap = mmapr;
	physmm_free_range(0x400000, 0x800000);
	while(((uintptr_t)mmap) < (((uintptr_t)mmapr) + mbt->mmap_length)) {
			/*earlycon_printf("MMAP ENTRY baseh=0x%x basel=0x%x lenh=0x%x lenl=0x%x type=0x%x\n",
				 mmap->base_addr_high,
				 mmap->base_addr_low,
				 mmap->length_high,
				 mmap->length_low,
				 mmap->type);*/
			// TODO : Verify page alignment of mmap entries
			if (mmap->base_addr_low >= 0x100000){
			
			if ((mmap->type == 1) && (mmap->base_addr_high == 0)) {
				if (mmap->length_high != 0)
					mmap->length_low = 0xFFFFFFFF - mmap->base_addr_low;
				available += mmap->length_low;
				physmm_free_range(mmap->base_addr_low, mmap->base_addr_low+mmap->length_low);
			}
			
		}
			mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}
	return available;
}

void i386_init_reserve_modules(multiboot_info_t *mbt)
{
	unsigned int i;
	multiboot_module_t *modules = (multiboot_module_t *) (mbt->mods_addr + 0xC0000000);
	if ( !mbt->mods_count )
		return;
	physmm_claim_range((physaddr_t)mbt->mods_addr, ((physaddr_t)mbt->mods_addr) + sizeof(multiboot_module_t)*(mbt->mods_count));
	for (i = 0; i < mbt->mods_count; i++){
		physmm_claim_range((physaddr_t)modules[i].string, (physaddr_t)modules[i].string + 80);
		physmm_claim_range((physaddr_t)modules[i].mod_start, (physaddr_t)modules[i].mod_end);
	}
}

void i386_init_handle_elf(multiboot_info_t *mbt)
{
	unsigned int i;
	earlycon_printf("multiboot: flags=0x%x\n",mbt->flags);
	if ( MULTIBOOT_INFO_ELFSYM & ~mbt->flags )
		return;
	earlycon_printf("multiboot: elf symbol header"
	                "num:%i size:%i addr:%x shndx:%x\n",
	                mbt->u.elf_sec.num,
	                mbt->u.elf_sec.size,
	                mbt->u.elf_sec.addr,
	                mbt->u.elf_sec.shndx);
}
//TODO: Unify Module handling across architectures
void i386_init_load_modules(multiboot_info_t *mbt)
{
	unsigned int i;
	uintptr_t page_ptr = 0x40000000;
	size_t ptr = 0;
	size_t mod_size;
	multiboot_module_t *modules = (multiboot_module_t *) (mbt->mods_addr + 0xC0000000);
	if ( !mbt->mods_count )
		return;
	for (i = 0; i < mbt->mods_count; i++){
		//char *name = (char *)(modules[i].string + 0xC0000000);
		mod_size = modules[i].mod_end - modules[i].mod_start;
		page_ptr = (uintptr_t) heapmm_alloc_alligned( mod_size, 4096 );
		for (ptr = 0; ptr < mod_size; ptr+=PHYSMM_PAGE_SIZE) {
			physmm_free_frame(
				paging_get_physical_address((void *) (page_ptr + ptr)));
			paging_unmap((void *) (page_ptr + ptr));
			paging_map((void *) (page_ptr + ptr), (physaddr_t)(modules[i].mod_start + ptr), I386_PAGE_FLAG_RW | I386_PAGE_FLAG_PRESENT);
		}
		ramblk_register( i, (aoff_t) mod_size, (void*) page_ptr );
		//tar_extract_mem((void *) page_ptr);
		//for (ptr = 0; ptr < mod_size; ptr+=PHYSMM_PAGE_SIZE) {
		//	paging_unmap((void *) (page_ptr + ptr));
		//	physmm_free_frame((physaddr_t)(modules[i].mod_start + ptr));
		//}
			
	}
}

void i386_init_handle_vbe(multiboot_info_t *mbt)
{
	if ((~mbt->flags & MULTIBOOT_INFO_VIDEO_INFO) || (!mbt->vbe_mode_info)) {
		debugcon_puts("BOOTLOADER DID NOT SET UP VIDEO MODE\n");
		vbe_mode.Xres = 0;
		return;
	}
	mbt->vbe_mode_info |= 0xC0000000;
	memcpy(&vbe_mode, (void *) mbt->vbe_mode_info, sizeof(vbe_mode_info_t));
	debugcon_printf("Bootloader VBE info @%x : {mode: %i,  lfb: %x, w:%i, h: %i, bpp: %i}\n", (mbt->vbe_mode_info), (int)mbt->vbe_mode, (int)vbe_mode.physbase, (int)vbe_mode.Xres, (int)vbe_mode.Yres, (int)vbe_mode.bpp);	
}

void i386_init_handle_cmdline(multiboot_info_t *mbt)
{
	/* Check if we had a command line */
	if ( (~mbt->flags & MULTIBOOT_INFO_CMDLINE) || (!mbt->cmdline) )
		return;
	memcpy( kernel_cmdline, (char *)(mbt->cmdline| 0xC0000000), 
			CONFIG_CMDLINE_MAX_LENGTH );
	kernel_cmdline[CONFIG_CMDLINE_MAX_LENGTH-1] = 0;
}
void i386_kmain();
void i386_init_mm(multiboot_info_t* mbd, unsigned int magic)
{
	size_t initial_heap = 4096;
	uint32_t mem_avail = 0;
	void *pdir_ptr;
	
	mbd= (multiboot_info_t*) (((uintptr_t)mbd) + 0xC0000000);
	
	earlycon_init();
	debugcon_init();
	debugcon_puts("Debugger console up on ttyS0\n");

	i386_fpu_initialize();

	earlycon_puts("Initializing physical memory manager...");
	physmm_init();

	earlycon_puts("OK\nRegistering available memory...");


	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		earlycon_puts("WARN: Not loaded by multiboot\n");
		earlycon_puts("Assuming 8MB of RAM and trying again...");
		physmm_free_range(0x100000, 0x800000);
		mem_avail = 0x700000;
	} else {
		mem_avail = i386_init_multiboot_memprobe(mbd);
		i386_init_reserve_modules(mbd);
		i386_init_handle_vbe(mbd);
		i386_init_handle_cmdline(mbd);
		i386_init_handle_elf(mbd);
	}
	physmm_claim_range(0x100000, 0x400000);
	earlycon_puts("OK\n");
	earlycon_printf("Detected %i MB of RAM.\n", (mem_avail/0x100000));
	earlycon_puts("Enabling paging...");
	paging_init();

	earlycon_puts("OK\nInitializing kernel heap manager...");
	kdbg_initialize();

	initial_heap = heapmm_request_core((void *)0xD0000000, initial_heap);

	if (initial_heap == 0) {
		earlycon_puts("FAIL\nCould not allocate first page of heap!\n");
		for(;;); //TODO: PANIC!!!		
	}

	heapmm_init((void *)0xD0000000, initial_heap);

	pdir_ptr = heapmm_alloc_page();
	physmm_free_frame(paging_get_physical_address(pdir_ptr));
	paging_map(pdir_ptr, paging_get_physical_address((void *)0xFFFFF000), PAGING_PAGE_FLAG_RW);
	paging_active_dir->content = pdir_ptr;
	earlycon_puts("OK\nInitializing kernel stack...");
	
	earlycon_puts("OK\nCalling kmain...");
	mb_info = mbd;
	paging_unmap( 0 );
	
	i386_kmain();

	
} 

void vgacon_clear_video();

void vterm_vga_init();

void i386_init_stub( process_info_t *proc )
{

	debugcon_printf("init_stub entry:%x\n", proc);
	
	scheduler_reown_task( scheduler_current_task, proc );
	
	int fd = _sys_open( console_path, O_RDWR, 0 );
	if (fd < 0) {
		earlycon_printf("Error opening tty : %i\n",syscall_errno);
	} else {
		fd = _sys_dup(fd);
		if (fd < 0) {
			earlycon_printf("Error dupping stdin : %i\n",syscall_errno);
		} else {
			fd = _sys_dup(fd);
			if (fd < 0) {
				earlycon_printf("Error dupping stderr : %i\n",syscall_errno);
			}
		}
	}
	char *ptr = NULL;
	void *nullaa = &ptr;
	//vgacon_clear_video();
	int status = process_exec( init_path, nullaa, nullaa );
	if (status) {
		earlycon_printf("Error executing init : %i\n",status);
	}
	for (;;)
		asm ("hlt;");
}

/**
 * This function implements the idle task that runs when there are no other running processes
 */

void i386_idle_task( process_info_t *proc )
{	
	scheduler_set_as_idle();	
	
	scheduler_reown_task( scheduler_current_task, proc );
	
	asm ("sti");
		
	for (;;)
		asm ("hlt;");
}
void sercon_init();
void register_dev_drivers();
/**
 * Once we are here everything is set up properly
 * First 4 MB are both at 0xC000 0000 and 0x0000 0000 to support GDT trick!
 * Task Stack	is at 0xBFFF DFFF downwards
 * ISR Stack	is at 0xBFFF E000
 * --------- KERNEL RAM BOUNDARY ----------
 * Code    	is at 0xC000 0000
 * Heap		is at 0xD000 0000
 * Pagedir	is at 0xFFC0 0000
 */
void i386_kmain()
{
	pid_t pid_init, pid_idle;
	int init_status, rv;

	earlycon_puts("OK\n\n");

	earlycon_printf("P-OS Release %s Version %s. %i MB free\n",
		posnk_release, posnk_version, physmm_count_free()/0x100000);

	earlycon_printf("kernel commandline:\n%s\n",kernel_cmdline);

	cmdline_parse();

	earlycon_puts("Initializing exception handlers...");
	i386_idt_initialize();
	earlycon_puts("OK\n");

	earlycon_puts("Initializing scheduler...");
	scheduler_init();
	process_init();
	earlycon_puts("OK\n");

	earlycon_puts("Initializing interrupt controller...");
	interrupt_init();
	i386_pic_initialize();
	earlycon_puts("OK\n");

	earlycon_puts("Initializing system timer...");
	i386_pit_setup(1000, 0, I386_PIT_OCW_MODE_RATEGEN);
	earlycon_puts("OK\n");

	earlycon_puts("Initializing driver framework...");
	drivermgr_init();
	device_char_init();
	device_block_init();
	tty_init();
	earlycon_puts("OK\n");

	earlycon_puts("Registering built in drivers...");
	sercon_init();
	register_dev_drivers();
	earlycon_puts("OK\n");

#ifndef CONFIG_i386_NO_PCI
	earlycon_puts("Enumerating PCI buses...");
	pci_enumerate_all();
	earlycon_puts("OK\n");
#endif

	earlycon_puts("Loading module files...");
	i386_init_load_modules(mb_info);
	earlycon_puts("OK\n");

	earlycon_puts("Initializing VFS and mounting rootfs...");
	if (vfs_initialize( root_dev, root_fs ))
		earlycon_puts("OK\n");
	else
		earlycon_puts("FAIL\n");
//DEV_DRIVER(fb, video)
//DEV_DRIVER(mbfb, video)
//DEV_DRIVER(fbcon_vterm, console)
	/*earlycon_puts("Extracting initrd..");
	if (!tar_extract("/initrd.tar"))
		earlycon_puts("OK\n");
	else
		earlycon_puts("FAIL\n");*/

	ipc_init();
	
	earlycon_puts("Initializing protection...");
	i386_protection_init();
	earlycon_puts("OK\n");
	
	syscall_init();


	pid_init = scheduler_spawn( i386_init_stub, fork_process(), NULL );
	
	if ( pid_init < 0 )
		earlycon_puts("FAIL\n");

	pid_idle = scheduler_spawn( i386_idle_task, current_process, NULL );
	
	if ( pid_idle < 0 )
		earlycon_puts("FAIL\n");
		
	ata_interrupt_enabled = 1;

	rv = sys_waitpid((uint32_t) 1,(uint32_t) &init_status,0,0,0,0);
	
	earlycon_printf("PANIC! Init exited with status: %i %i\n",init_status,rv);

	earlycon_puts("\n\nkernel main exited... halting!");
	halt();

}

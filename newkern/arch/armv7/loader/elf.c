/**
 * arch/armv7/loader/mmu.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2015 - Created
 */

#include "arch/armv7/mmu.h"
#include "util/llist.h"
#include "kernel/physmm.h"
#include "kernel/elf.h"
#include "arch/armv7/loader.h"
#include "arch/armv7/bootargs.h"
#include "config.h"
#include <string.h>
#include <assert.h>

#define ENOEXEC 2

void elf_section(uint32_t vaddr, uint32_t msize, char * data, uint32_t csize)
{
	physaddr_t frame;
	uint32_t vad;

	

	for (vad = vaddr; vad < (vaddr + msize); vad += 4096) {
		frame = physmm_alloc_frame();
		armv7_add_kmap(vaddr, frame, 4096, 	ARMV7_BA_KMAP_EXEC | 
							ARMV7_BA_KMAP_READ | 
							ARMV7_BA_KMAP_WRITE);
		if (frame == PHYSMM_NO_FRAME)	{
			sercon_printf("NO MEMORY!\n");
			halt();
		}
		armv7_mmu_map((void *)vad, frame);
	}

	armv7_mmu_flush_tlb();

	if (msize > csize)
		memset((void *)(vaddr + csize), 0, msize - csize);

	if (csize != 0)
		memcpy((void *) vaddr, data, csize);

}

int elf_load(char * file)
{		
	int flags;
	int ph_ptr = 0;
	Elf32_Ehdr *elf_header;
	Elf32_Phdr aelf_pheader;
	Elf32_Phdr *elf_pheader = &aelf_pheader;
	uintptr_t image_base = 0xc0000000;
	uintptr_t image_top = 0;
	elf_header = (Elf32_Ehdr *) file;

	if (elf_header->e_type != ET_EXEC) {
		return ENOEXEC;
	}

	if (elf_header->e_machine != EM_ARM) {
		debugcon_printf("ERROR: tried to load non-ARM elf file: %x\n", elf_header->e_machine);
		return ENOEXEC;
	}

	if (elf_header->e_phnum == 0) {
		debugcon_printf("ERROR: tried to load object file\n");
		return ENOEXEC;
	}

	//debugcon_printf("Loading ELF binary, magic: %c%c%c%c, shnum:%i, phnum:%i ...\n", elf_header->e_ident[0], elf_header->e_ident[1], elf_header->e_ident[2], elf_header->e_ident[3], 
	//	elf_header->e_shnum, elf_header->e_phnum);

	for (ph_ptr = 0; ph_ptr < elf_header->e_phnum; ph_ptr++) {
		memcpy(elf_pheader, &file[elf_header->e_phoff+(elf_header->e_phentsize * ph_ptr)], elf_header->e_phentsize);
	//	debugcon_printf("Loading program section off:%x va:%x pa:%x sz:%x fsz:%x...\n", elf_pheader->p_offset, elf_pheader->p_vaddr, elf_pheader->p_paddr, elf_pheader->p_memsz, elf_pheader->p_filesz);

		switch (elf_pheader->p_type) {
			case PT_LOAD:
				if (elf_pheader->p_vaddr < 0xC0000000) {
					debugcon_printf("ERROR: tried to map non-kernel memory: %x\n", elf_pheader->p_vaddr);
					return ENOEXEC;
				}
				flags = 0;
				//if (elf_pheader->p_flags & PF_W)
				//	flags |= PROCESS_MMAP_FLAG_WRITE;	
				//status = procvmm_mmap_file((void *)elf_pheader->p_vaddr, (size_t) elf_pheader->p_memsz,
				//			    inode, (off_t) elf_pheader->p_offset, (off_t) elf_pheader->p_filesz,
				//			    flags, NULL);
				elf_section(elf_pheader->p_vaddr, elf_pheader->p_memsz,&file[elf_pheader->p_offset], elf_pheader->p_filesz);
				if ((elf_pheader->p_vaddr + elf_pheader->p_memsz) > image_top)
					image_top = elf_pheader->p_vaddr + elf_pheader->p_memsz;
				if (elf_pheader->p_vaddr < image_base)
					image_base = elf_pheader->p_vaddr;
				break;
			case PT_TLS:
				break;
			default:
				debugcon_printf("ERROR: unknown program header type: %x\n", elf_pheader->p_type);
				return ENOEXEC;
		}
	}
	image_top += PHYSMM_PAGE_SIZE;
	image_top &= ~PHYSMM_PAGE_ADDRESS_MASK;
	debugcon_printf("elf: loaded ELF image between VA %x and VA %x\n",image_base, image_top);
	//debugcon_printf("Calling elf image entry point\n");
	elf_kmain = (void *)elf_header->e_entry;
	return 0;	
}


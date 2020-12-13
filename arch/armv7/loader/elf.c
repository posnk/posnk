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
#define CON_SRC "loader"
#include "kernel/console.h"
#include "arch/armv7/loader.h"
#include "arch/armv7/bootargs.h"
#include "config.h"
#include <string.h>
#include <assert.h>

#define ENOEXEC 2

uint32_t elf_top;

void (*elf_kmain)(uint32_t);
void elf_section(uint32_t vaddr, uint32_t msize, char * data, uint32_t csize)
{
	physaddr_t frame;
	uint32_t vad;



	for (vad = vaddr; vad < (vaddr + msize); vad += 4096) {
		frame = physmm_alloc_frame();
		armv7_add_kmap(frame, vad, 4096, 	ARMV7_BA_KMAP_EXEC |
							ARMV7_BA_KMAP_READ |
							ARMV7_BA_KMAP_WRITE);
		if (frame == PHYSMM_NO_FRAME)	{
			printf(CON_ERROR, "NO MEMORY!\n");
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
		printf(CON_ERROR, "tried to load non-ARM elf file: %x\n", elf_header->e_machine);
		return ENOEXEC;
	}

	if (elf_header->e_phnum == 0) {
		printf(CON_ERROR, "tried to load object file\n");
		return ENOEXEC;
	}


	for (ph_ptr = 0; ph_ptr < elf_header->e_phnum; ph_ptr++) {
		memcpy(elf_pheader, &file[elf_header->e_phoff+(elf_header->e_phentsize * ph_ptr)], elf_header->e_phentsize);

		switch (elf_pheader->p_type) {
			case PT_LOAD:
				if (elf_pheader->p_vaddr < 0xC0000000) {
					printf(CON_ERROR, "tried to map non-kernel memory: %x\n", elf_pheader->p_vaddr);
					return ENOEXEC;
				}
				flags = 0;
				elf_section(elf_pheader->p_vaddr, elf_pheader->p_memsz,&file[elf_pheader->p_offset], elf_pheader->p_filesz);
				if ((elf_pheader->p_vaddr + elf_pheader->p_memsz) > image_top)
					image_top = elf_pheader->p_vaddr + elf_pheader->p_memsz;
				if (elf_pheader->p_vaddr < image_base)
					image_base = elf_pheader->p_vaddr;
				break;
			case PT_TLS:
				break;
			default:
				printf(CON_ERROR, "unknown program header type: %x\n", elf_pheader->p_type);
				break;//	return ENOEXEC;
		}
	}
	image_top += PHYSMM_PAGE_SIZE;
	image_top &= ~PHYSMM_PAGE_ADDRESS_MASK;
	elf_top = image_top;
	//debugcon_printf("Calling elf image entry point\n");
	elf_kmain = (void *)elf_header->e_entry;
	return 0;
}


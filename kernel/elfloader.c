/*
 * kernel/elfloader.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-04-2014 - Created
 */

#include <sys/errno.h>
#include <sys/stat.h>
#include <string.h>
#define CON_SRC "elfloader"
#include "kernel/console.h"
#include "kernel/vfs.h"
#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#include "kernel/elf.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"

int elf_verify( const char *hdr, size_t size )
{
	const Elf32_Ehdr *elf_header = ( Elf32_Ehdr *) hdr;

	if ( size < sizeof( Elf32_Ehdr ) )
		return 0;

	if (elf_header->e_type != ET_EXEC) {
		return 0;
	}
	//if (elf_header->e_machine != EM_386) {
//		return 0;
//	}
	if (elf_header->e_phnum == 0)
		return 0;
	return 1;

}

Elf32_Shdr *elf_get_shdr( Elf32_Shdr* first, size_t shentsz, int i )
{
	return ((void*)first) + shentsz * i;
}

int elf_load( inode_t *inode, const char *path )
{
	aoff_t rd_count;
	int status;
	int flags;
	int ph_ptr = 0;
	Elf32_Ehdr *elf_header;
	Elf32_Phdr *elf_pheader;
	uintptr_t image_base = 0xc0000000;
	uintptr_t image_top = 0;

	inode = vfs_inode_ref( inode );

	elf_header = (Elf32_Ehdr *) heapmm_alloc(sizeof(Elf32_Ehdr));

	if (!elf_header) {
		vfs_inode_release(inode);
		return ENOMEM;
	}

	status = vfs_read(inode, 0, elf_header, sizeof(Elf32_Ehdr), &rd_count, 0);
	if (status) {
		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
		vfs_inode_release(inode);
		return status;
	}
	if (rd_count != sizeof(Elf32_Ehdr)) {
		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
		vfs_inode_release(inode);
		return EIO;
	}

	if (elf_header->e_type != ET_EXEC) {
		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
		vfs_inode_release(inode);
		return ENOEXEC;
	}
	//if (elf_header->e_machine != EM_386) {
//		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
//		return ENOEXEC;
//	}
	if (elf_header->e_phnum == 0) {
		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
		return ENOEXEC;
	}

	//debugcon_printf("Loading ELF binary %s, magic: %c%c%c%c, shnum:%i, phnum:%i ...\n", path, elf_header->e_ident[0], elf_header->e_ident[1], elf_header->e_ident[2], elf_header->e_ident[3],
	//	elf_header->e_shnum, elf_header->e_phnum);

	elf_pheader = (Elf32_Phdr *) heapmm_alloc(elf_header->e_phentsize);
	if (!elf_pheader) {
		heapmm_free(elf_header, sizeof(Elf32_Ehdr));
		vfs_inode_release(inode);
		return ENOMEM;
	}

	for (ph_ptr = 0; ph_ptr < elf_header->e_phnum; ph_ptr++) {
		status = vfs_read(inode, (aoff_t)(elf_header->e_phoff+(elf_header->e_phentsize * ph_ptr)), elf_pheader, elf_header->e_phentsize, &rd_count, 0);
		if (status) {
			heapmm_free(elf_pheader, elf_header->e_phentsize);
			heapmm_free(elf_header, sizeof(Elf32_Ehdr));
			vfs_inode_release(inode);
			return status;
		}
		if (rd_count != elf_header->e_phentsize) {
			heapmm_free(elf_pheader, elf_header->e_phentsize);
			heapmm_free(elf_header, sizeof(Elf32_Ehdr));
			vfs_inode_release(inode);
			return EIO;
		}
		//debugcon_printf("Loading program section off:%x va:%x pa:%x sz:%x fsz:%x...\n", elf_pheader->p_offset, elf_pheader->p_vaddr, elf_pheader->p_paddr, elf_pheader->p_memsz, elf_pheader->p_filesz);

		switch (elf_pheader->p_type) {
			case PT_LOAD:
				if (elf_pheader->p_vaddr >= 0xC0000000) {
					printf(CON_ERROR, "tried to map kernel memory: %x", elf_pheader->p_vaddr);
					heapmm_free(elf_pheader, elf_header->e_phentsize);
					heapmm_free(elf_header, sizeof(Elf32_Ehdr));
					vfs_inode_release(inode);
					return ENOEXEC;
				}
				flags = 0;
				if (elf_pheader->p_flags & PF_W)
					flags |= PROCESS_MMAP_FLAG_WRITE;
				status = procvmm_mmap_file((void *)elf_pheader->p_vaddr, (size_t) elf_pheader->p_memsz,
							    inode, (off_t) elf_pheader->p_offset, (off_t) elf_pheader->p_filesz,
							    flags, path );
				if (status) {
					printf(CON_DEBUG, "could not map PH_LOAD: %x", status);
					heapmm_free(elf_pheader, elf_header->e_phentsize);
					heapmm_free(elf_header, sizeof(Elf32_Ehdr));
					vfs_inode_release(inode);
					return status;
				}
				if ((elf_pheader->p_vaddr + elf_pheader->p_memsz) > image_top)
					image_top = elf_pheader->p_vaddr + elf_pheader->p_memsz;
				if (elf_pheader->p_vaddr < image_base)
					image_base = elf_pheader->p_vaddr;
				break;
			case PT_TLS:
				break;
			default:
				printf(CON_ERROR, "unknown program header type: %x", elf_pheader->p_type);
				heapmm_free(elf_pheader, elf_header->e_phentsize);
				heapmm_free(elf_header, sizeof(Elf32_Ehdr));
				vfs_inode_release(inode);
				return ENOEXEC;
		}
	}
	image_top += PHYSMM_PAGE_SIZE;
	image_top &= ~PHYSMM_PAGE_ADDRESS_MASK;
	//debugcon_printf("Loaded ELF image between %x and %x\n",image_base, image_top);
	current_process->image_start = (void *) image_base;
	current_process->image_end   = (void *) image_top;
	current_process->entry_point = (void *) elf_header->e_entry;
	current_process->image_inode = vfs_inode_ref(inode);
	current_process->image_inode->open_count++;
	heapmm_free(elf_pheader, elf_header->e_phentsize);
	heapmm_free(elf_header, sizeof(Elf32_Ehdr));
	vfs_inode_release(inode);
	return 0;
}


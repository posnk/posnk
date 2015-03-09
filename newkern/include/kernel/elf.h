/*
 * kernel/elf.h
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

#ifndef __KERNEL_ELF_H__
#define __KERNEL_ELF_H__

#define EI_NIDENT 16

#define ET_NONE		0	/* No file type */
#define ET_REL		1	/* Relocatable file */
#define ET_EXEC		2	/* Executable file */
#define ET_DYN		3	/* Shared object file */
#define ET_CORE		4	/* Core file */
#define ET_LOOS		0xff00 	/* Start OS specific */
#define ET_HIOS		0xffff 	/* End OS specific */
#define ET_LOPROC	0xff00 	/* Start ARCH specific */
#define ET_HIPROC	0xffff 	/* End ARCH specific */

#define EM_386		3	/* 80386 (i386, x86) */
#define EM_ARM		0x28	/* ARM */

#define	EV_NONE		0	/* Invalid version */
#define EV_CURRENT	1	/* Current version */


#define EI_MAG0         0	/* e_ident[] indexes */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_OSABI        7
#define EI_PAD          8

#define ELFMAG          "\177ELF"
#define SELFMAG         4

#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2

#define ELFDATANONE	0
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

#define ELFOSABI_NONE   0
#define ELFOSABI_POS    17

/* special section indexes */
#define SHN_UNDEF       0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC      0xff00
#define SHN_HIPROC      0xff1f
#define SHN_ABS         0xfff1
#define SHN_COMMON      0xfff2
#define SHN_HIRESERVE   0xffff

/* sh_type */
#define SHT_NULL        0		/* inactive section */
#define SHT_PROGBITS    1		/* program defined data */
#define SHT_SYMTAB      2		/* symbol table, full */
#define SHT_STRTAB      3		/* string table */
#define SHT_RELA        4		/* relocation table, explicit */
#define SHT_HASH        5		/* symbol hash table */
#define SHT_DYNAMIC     6		/* dynamic linking info */
#define SHT_NOTE        7		/* misc... */
#define SHT_NOBITS      8		/* program defined data, not in file, zerofill */
#define SHT_REL         9		/* relocation table */
#define SHT_SHLIB       10		/* unspecified */
#define SHT_DYNSYM      11		/* symbol table, compact for dynlink */
#define SHT_NUM         12		
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

/* sh_flags */
#define SHF_WRITE       0x1		/* Writable section */
#define SHF_ALLOC       0x2		/* Section should be in RAM */
#define SHF_EXECINSTR   0x4		/* Executable instructions in section */
#define SHF_MASKPROC    0xf0000000	/* Processor specific */

/* These constants are for the segment types stored in the image headers */
#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_TLS		7               /* Thread local storage segment */
#define PT_LOOS		0x60000000      /* OS-specific */
#define PT_HIOS		0x6fffffff      /* OS-specific */
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff

#define PF_X		0x1
#define PF_W		0x2
#define PF_R		0x4

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef  int32_t	Elf32_Sword;
typedef uint32_t	Elf32_Word;

typedef struct {
        unsigned char e_ident[EI_NIDENT]; 
        Elf32_Half    e_type;			
        Elf32_Half    e_machine;		
        Elf32_Word    e_version;		
        Elf32_Addr    e_entry;			/* Entry point virt_addr */
        Elf32_Off     e_phoff;			/* Program header offset */
        Elf32_Off     e_shoff;			/* Section header offset */
        Elf32_Word    e_flags;			/* Processor specific flags */
        Elf32_Half    e_ehsize;			/* ELF header size */
        Elf32_Half    e_phentsize;		/* Program header entry size */
        Elf32_Half    e_phnum;			/* Program header entry count */
        Elf32_Half    e_shentsize;		/* Section header entry size */
        Elf32_Half    e_shnum;			/* Section header entry count */
        Elf32_Half    e_shstrndx;		/* Section header string table idx */
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	sh_name;		/* Name string table index */
	Elf32_Word	sh_type;		/* Section type */
	Elf32_Word	sh_flags;		/* Section flags */
	Elf32_Addr	sh_addr;		/* Section virt_addr */
	Elf32_Off	sh_offset;		/* Section data file offset */
	Elf32_Word	sh_size;		/* Section size */
	Elf32_Word	sh_link;		/* Section header table link */
	Elf32_Word	sh_info;	
	Elf32_Word	sh_addralign;		
	Elf32_Word	sh_entsize;		/* Entry size */
} Elf32_Shdr;

typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;
#endif


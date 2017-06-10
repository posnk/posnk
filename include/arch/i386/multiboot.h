/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
     
        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.
     
        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
     
        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */
     
     /* Macros. */
     
     /* The magic number for the Multiboot header. */
     #define MULTIBOOT_HEADER_MAGIC          0x1BADB002
     
     /* The flags for the Multiboot header. */
     #ifdef __ELF__
     # define MULTIBOOT_HEADER_FLAGS         0x00000003
     #else
     # define MULTIBOOT_HEADER_FLAGS         0x00010003
     #endif
     
     /* The magic number passed by a Multiboot-compliant boot loader. */
     #define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002
     
     /* The size of our stack (16KB). */
     #define STACK_SIZE                      0x4000
     
     /* C symbol format. HAVE_ASM_USCORE is defined by configure. */
     #ifdef HAVE_ASM_USCORE
     # define EXT_C(sym)                     _ ## sym
     #else
     # define EXT_C(sym)                     sym
     #endif
     
     #define MULTIBOOT_INFO_VIDEO_INFO               0x00000800
     #define MULTIBOOT_INFO_BOOT_LOADER_NAME         0x00000200
     #define MULTIBOOT_INFO_CMDLINE                  0x00000004
     
     #ifndef ASM
     /* Do not include here in boot.S. */
     
     /* Types. */
     
     /* The Multiboot header. */
     typedef struct multiboot_header
     {
       unsigned long magic;
       unsigned long flags;
       unsigned long checksum;
       unsigned long header_addr;
       unsigned long load_addr;
       unsigned long load_end_addr;
       unsigned long bss_end_addr;
       unsigned long entry_addr;
     } multiboot_header_t;
     
     /* The symbol table for a.out. */
     typedef struct aout_symbol_table
     {
       unsigned long tabsize;//28
       unsigned long strsize;//32
       unsigned long addr;//36
       unsigned long reserved;//40
     } aout_symbol_table_t;
     
     /* The section header table for ELF. */
     typedef struct elf_section_header_table
     {
       unsigned long num;
       unsigned long size;
       unsigned long addr;
       unsigned long shndx;
     } elf_section_header_table_t;
     
     /* The Multiboot information. */
     typedef struct multiboot_info
     {
       unsigned long flags;//0
       unsigned long mem_lower;//4
       unsigned long mem_upper;//8
       unsigned long boot_device;//12
       unsigned long cmdline;//16
       unsigned long mods_count;//20
       unsigned long mods_addr;//24
       union
       {
         aout_symbol_table_t aout_sym;
         elf_section_header_table_t elf_sec;
       } u;
       unsigned long mmap_length;//44
       unsigned long mmap_addr;//48
       unsigned long drives_length;//52
       unsigned long drives_addr;//56
       unsigned long config_table;//60
       unsigned long boot_loader_name;//64
       unsigned long apm_table;//68
       unsigned long vbe_control_info;//72
       unsigned long vbe_mode_info;//76
       unsigned short vbe_mode;//80
       unsigned short vbe_interface_seg;//82
       unsigned short vbe_interface_off;//84
       unsigned short vbe_interface_len;//86
     } multiboot_info_t;
     
     /* The module structure. */
     typedef struct multiboot_module
     {
       unsigned long mod_start;
       unsigned long mod_end;
       unsigned long string;
       unsigned long reserved;
     } multiboot_module_t;
     
     /* The memory map. Be careful that the offset 0 is base_addr_low
        but no size. */
     typedef struct multiboot_memory_map
     {
       unsigned long size;//0
       unsigned long base_addr_low;//4
       unsigned long base_addr_high;//8
       unsigned long length_low;//12
       unsigned long length_high;//16
       unsigned long type;//20
     } multiboot_memory_map_t;
     
     #endif /* ! ASM */

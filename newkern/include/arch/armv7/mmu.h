/**
 * arch/armv7/mmu.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-03-2015 - Created
 */

#ifndef __ARCH_ARMV7_MMU_H__
#define __ARCH_ARMV7_MMU_H__

#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "util/llist.h"

void armv7_mmu_set_dacr (uint32_t dacr);
void armv7_mmu_set_ttbr0(uint32_t ttbr0);
void armv7_mmu_set_ttbr1(uint32_t ttbr1);
void armv7_mmu_set_ttbcr(uint32_t ttbcr);
void armv7_mmu_enable	( void );
void armv7_mmu_enable	( void );
void armv7_mmu_flush_tlb( void );
void armv7_mmu_flush_tlb_single( uint32_t mva );

#define ARMV7_L1_TYPE_FAULT		(0x00000000)
#define ARMV7_L1_TYPE_PAGE_TABLE	(0x00000001)
#define ARMV7_L1_TYPE_SECTION		(0x00000002)
#define ARMV7_L1_DOMAIN(Dn)		((Dn & 0xF) << 5)
#define ARMV7_L1_PAGE_TABLE_PA(Pa)	(Pa & 0xFFFFFC00)
#define ARMV7_L1_SECTION_PA(Pa)		(Pa & 0xFFF00000)
#define ARMV7_L1_SECTION_NOT_GLOBAL	(0x00020000)
#define ARMV7_L1_SECTION_EXECUTE_NEVER	(0x00000010)
#define ARMV7_L1_SECTION_C		(0x00000008)
#define ARMV7_L1_SECTION_B		(0x00000004)
#define ARMV7_L1_SECTION_TEX(TeX)	((TeX & 0x7)<<12)
#define ARMV7_L1_SECTION_AP(Ap)		(((Ap & 0x4)<<15)|((Ap & 0x3)<<10))

#define ARMV7_L2_TYPE_FAULT		(0x00000000)
#define ARMV7_L2_TYPE_PAGE64_XN		(0x00008001)
#define ARMV7_L2_TYPE_PAGE4		(0x00000002)
#define ARMV7_L2_TYPE_PAGE4_XN		(0x00000003)
#define ARMV7_L2_PAGE64_PA(Pa)		(Pa & 0xFFFF0000)
#define ARMV7_L2_PAGE4_PA(Pa)		(Pa & 0xFFFFF000)
#define ARMV7_L2_PAGE_NOT_GLOBAL	(0x00000800)
#define ARMV7_L2_PAGE_S			(0x00000400)
#define ARMV7_L2_PAGE_AP(Ap)		(((Ap & 0x4)<<9)|((Ap & 0x3)<<4))
#define ARMV7_L2_PAGE_C			(0x00000008)
#define ARMV7_L2_PAGE_B			(0x00000004)
#define ARMV7_L2_PAGE64_TEX(TeX)	((TeX & 0x7)<<12)
#define ARMV7_L2_PAGE4_TEX(TeX)		((TeX & 0x7)<<6)

#define ARMV7_SECTION_SIZE		(0x00100000)

#define ARMV7_L1_TABLE_SIZE		(4096)
#define ARMV7_L2_TABLE_SIZE		(256)

typedef struct armv7_l2_table		armv7_l2_table_t;
typedef struct armv7_l1_table		armv7_l1_table_t;
typedef struct armv7_l1_table_list	armv7_l1_table_list_t;

struct armv7_l1_table {
	uint32_t		entries[ARMV7_L1_TABLE_SIZE];
};

struct armv7_l2_table {
	uint32_t		pages[ARMV7_L2_TABLE_SIZE];
};

struct armv7_l1_table_list {
	llist_t		node;
	page_dir_t *dir;
};

#define ARMV7_TO_L1_IDX(a)		( (((uintptr_t)a) & 0xFFF00000) >> 20 )
#define ARMV7_TO_L2_IDX(a)		( (((uintptr_t)a) & 0x000FF000) >> 12 )

#endif

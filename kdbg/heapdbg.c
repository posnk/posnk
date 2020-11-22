/*
 * kdbg/heapdbg.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#include <stdint.h>
#include <stddef.h>
#include "util/llist.h"
#include "util/debug.h"

#include "kdbg/stacktrc.h"
#include "kdbg/kdbgmm.h"
#include "kdbg/kdbgio.h"
#include "kdbg/heapdbg.h"
#include "kdbg/dbgapi.h"

llist_t kdbg_heap_use_list;
int heapdbg_up =0;

void kdbg_enable_memuse()
{
	heapdbg_up = 1;
}

void kdbg_init_memuse(){
	llist_create(&kdbg_heap_use_list);
	//heapdbg_up = 1;
}

void dbgapi_register_memuse(void *addr, size_t size)
{
	//kdbg_printf("A|%x|%x\n",addr,size);
	if (!heapdbg_up)
		return;
	kdbg_heap_use_t *use = kdbgmm_alloc(sizeof(kdbg_heap_use_t));
	use->start = (uintptr_t) addr;
	use->size = size;
	use->calltrace = kdbg_do_calltrace();
	//kdbg_p_calltrace();
	llist_add_end(&kdbg_heap_use_list, (llist_t *) use);
}

int kdbg_find_bf_iterator(llist_t *node, void *param) {
	return (((kdbg_heap_use_t *)node)->start + ((kdbg_heap_use_t *)node)->size) == (uintptr_t) param;
}

int kdbg_find_in_iterator(llist_t *node, __attribute__((unused)) void *param) {
	kdbg_heap_use_t *use = (kdbg_heap_use_t *) node;
	uintptr_t p = (uintptr_t) param;
	return (p >= use->start) && (p < (use->start + use->size));
}

int kdbg_find_memuse_iterator(llist_t *node, void *param) {
	return ((kdbg_heap_use_t *)node)->start == (uintptr_t) param;
}

void kdbg_print_memuse_brdr(void *addr)
{
	kdbg_heap_use_t *use = (kdbg_heap_use_t *) llist_iterate_select(&kdbg_heap_use_list, &kdbg_find_memuse_iterator, addr);
	if (!use) {
		kdbg_printf("Address not in use: 0x%x\n",addr);
		use = (kdbg_heap_use_t *) llist_iterate_select(&kdbg_heap_use_list, &kdbg_find_bf_iterator, addr);
		//return;
	}
	kdbg_printf("  Memory region 0x%x - 0x%x used by:\n", use->start, use->start + use->size);
	kdbg_print_calltrace(use->calltrace);
	use = (kdbg_heap_use_t *) llist_iterate_select(&kdbg_heap_use_list, &kdbg_find_bf_iterator, (void*) use->start);
	if (!use) {
		kdbg_printf("No bordering region: 0x%x\n",addr);
		return;
	}
	kdbg_printf("  Border region 0x%x - 0x%x used by:\n", use->start, use->start + use->size);
	kdbg_print_calltrace(use->calltrace);
}

void dbgapi_unreg_memuse(void *addr, size_t size)
{
	//kdbg_printf("F|%x|%x\n",addr,size);

	if (!heapdbg_up)
		return;
	kdbg_heap_use_t *use = (kdbg_heap_use_t *) llist_iterate_select(&kdbg_heap_use_list, &kdbg_find_memuse_iterator, addr);
	if (!use) {
		kdbg_printf("Detected double-free of address 0x%x\n",addr);
		//dbgapi_invoke_kdbg(1);
		return;
	} else if (use->size != size) {
		kdbg_printf("Size mismatch at address 0x%x  %u != %u\n",addr,use->size, size);
		dbgapi_invoke_kdbg(1);
	}
	llist_unlink((llist_t*)use);
	kdbg_free_calltrace(use->calltrace);
	kdbgmm_free(use, sizeof(kdbg_heap_use_t));
}

/**
 * util/llist.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2011       - Original version
 * 28-03-2014 - Changed names, added comments
 */

#include "llist.h"

#include <stddef.h>
#include <assert.h>

/**
 * Gets the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_get_first(llist_t *list)
{
	assert(list != NULL);
	return list->next;
}

/**
 * Gets the last entry in a doubly linked list
 * @param list Pointer top the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_get_last(llist_t *list)
{
	assert(list != NULL);
	return list->prev;
}

/**
 * Gets and removes the last entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The last node in the linked list
 */
llist_t *llist_remove_last(llist_t *list)
{
	llist_t *result;
	assert(list != NULL);
	assert(list->prev != NULL);
	assert(list->prev->prev != NULL);
	result = list->prev;
	list->prev = list->prev->prev;
	list->prev->next = list;
	return result;
}

/**
 * Gets and removes the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
llist_t *llist_remove_first(llist_t *list)
{
	llist_t *result;
	assert(list != NULL);
	assert(list->next != NULL);
	assert(list->next->next != NULL);
	result = list->next;
	list->next = list->next->next;
	list->next->prev = list;
	return result;
}

/**
 * Adds a node to the end of the linked list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void llist_add_end(llist_t *list,llist_t *entry)
{
	assert(list != NULL);
	assert(entry != NULL);
	entry->prev = list->prev;
	entry->next = list;
	list->prev->next = entry;
	list->prev = entry;
}

/**
 * Unlinks a node from the list it is in.
 * @param entry The node to unlink
 */
void llist_unlink(llist_t *entry)
{
	assert(entry != NULL);
	assert((entry->prev && entry->next) || ((!entry->prev) && (!entry->next)));
	if (entry->prev)
		entry->prev->next = entry->next;
	if (entry->next)
		entry->next->prev = entry->prev;
	entry->next = 0;
	entry->prev = 0;
}

/**
 * Initializes a new head node (list)
 * @param list Pointer to the linked list head node
 */
void llist_create(llist_t *list)
{
	assert(list != NULL);
	list->next = list;
	list->prev = list;
}

/**
 * Counts the number of nodes in a linked list
 * @param list Pointer to the linked list head node
 * @return The amount of nodes (excluding the head node)
 */
int llist_size(llist_t *list)
{
	llist_t *work;
	int size = 0;
	assert(list != NULL);
	work = list->next;
	for (;;){
		assert (work != NULL);
		if (list == work)
			break;
		size++;
		work = work->next;
	}
	return size;
}

/**
 * Iterates over the list and calls <b>iterator</b> for
 * each node, if <b>iterator</b> returns a true condition
 * iteration will stop and the current node will be returned
 *
 * @param list		Pointer to the linked list head node
 * @param iterator	Function pointer to an iterator int func(llist_t *, void *).
 * @param param		Parameter to the iterator. (may be NULL)
 * @return The node for which <b>iterator</b> returned true.
 */
llist_t *llist_iterate_select(llist_t *list, llist_iterator_t iterator, void *param)
{
	llist_t *work;
	assert(iterator != NULL);
	assert(list != NULL);
	work = list->next;
	for (;;) {
		assert (work != NULL);
		if (list == work)
			break;
		if (iterator(work, param))
			return work;
		work = work->next;
	}
	return NULL;
}


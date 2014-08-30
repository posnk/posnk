/**
 * clara/ccllist.h
 *
 * Part of libclara
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2011       - Original version
 * 28-03-2014 - Changed names, added comments
 * 28-03-2014 - Removed indexed linked lists
 * 28-03-2014 - Added iterator function
 */

#ifndef __UTIL_LINKED_LIST_H__
#define __UTIL_LINKED_LIST_H__

/**
 * typedef for cllist:
 * Linked list header structure prototype
 * To use linked lists, create a structure
 * containing a cllist_t as its first member.
 */
typedef struct cllist	   cllist_t;

/**
 * Iterator function prototype
 * Usage: int function_iterator(cllist_t *node, void *param);
 * node is the node for which the iterator is called,
 * param is the parameter passed to cllist_iterate_select
 */
typedef int (*cllist_iterator_t)(cllist_t *, void *);

/**
 * Linked list header structure prototype
 * To use linked lists, create a structure
 * containing a cllist_t as its first member.
 */
struct cllist {
	cllist_t *next;
	cllist_t *prev;
};

/**
 * Gets the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
cllist_t *cllist_get_first(cllist_t *list);

/**
 * Gets the last entry in a doubly linked list
 * @param list Pointer top the linked list head node
 * @return The last node in the linked list
 */
cllist_t *cllist_get_last(cllist_t *list);
/**
 * Gets and removes the last entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The last node in the linked list
 */
cllist_t *cllist_remove_last(cllist_t *list);

/**
 * Gets and removes the first entry in a doubly linked list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
cllist_t *cllist_remove_first(cllist_t *list);

/**
 * Adds a node to the end of the linked list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void cllist_add_end(cllist_t *list,cllist_t *entry);

/**
 * Adds a node to the start of the linked list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void cllist_add_start(cllist_t *list,cllist_t *entry);

/**
 * Unlinks a node from the list it is in.
 * @param entry The node to unlink
 */
void cllist_unlink(cllist_t *entry);

/**
 * Initializes a new head node (list)
 * @param list Pointer to the linked list head node
 */
void cllist_create(cllist_t *list);

/**
 * Counts the number of nodes in a linked list
 * @param list Pointer to the linked list head node
 * @return The amount of nodes (excluding the head node)
 */
int cllist_size(cllist_t *list);

/**
 * Iterates over the list and calls <b>iterator</b> for
 * each node, if <b>iterator</b> returns a true condition
 * iteration will stop and the current node will be returned
 *
 * @param list		Pointer to the linked list head node
 * @param iterator	Function pointer to an iterator int func(cllist _t *, void *).
 * @param param		Parameter to the iterator. (may be NULL)
 * @return The node for which <b>iterator</b> returned true.
 */
cllist_t *cllist_iterate_select(cllist_t *list, cllist_iterator_t iterator, void *param);

#endif

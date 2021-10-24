#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


typedef void* _ui_node_p;
typedef struct {
    size_t node_size;
    _ui_node_p head;
    _ui_node_p tail;
} _ui_llist_t;


/**
 * Initialize list
 * @param list Pointer to list that will be initialized
 * @param node_size Size in bytes of each list node
 */
void _ui_llist_init(_ui_llist_t* list, size_t node_size);

/**
 * Get the first node (head) of the list
 * @param list Pointer to list
 * @return Pointer to first node of the list
 */
_ui_node_p _ui_llist_get_first(const _ui_llist_t* list);

/**
 * Get the last node (tail) of the list
 * @param list Pointer to list
 * @return Pointer to last node of the list
 */
_ui_node_p _ui_llist_get_last(const _ui_llist_t* list);

/**
 * Get the node after \p node in the list
 * @param list Pointer to list
 * @param node Reference node
 * @return Pointer to the node after the provided node
 */
_ui_node_p _ui_llist_get_next(const _ui_llist_t* list, const _ui_node_p node);

/**
 * Get the node before \p node in the list
 * @param list Pointer to list
 * @param node Reference node
 * @return Pointer to the node before the provided node
 */
_ui_node_p _ui_llist_get_prev(const _ui_llist_t* list, const _ui_node_p node);

/**
 * Get the length of the list
 * @param list Pointer to list
 * @return Length of the list
 */
size_t _ui_llist_get_length(const _ui_llist_t* list);

/**
 * Check if given list is empty
 * @param list Pointer to list
 * @return TRUE: the list is empty;
 *         FALSE: the list is not empty
 */
bool _ui_llist_is_empty(const _ui_llist_t* list);

/**
 * Add a new node to the end of list
 * @param list Pointer to list
 * @return Pointer to the new node
 */
_ui_node_p _ui_llist_add(_ui_llist_t* list);

/**
 * Add a new node to the beginning of the list (new head)
 * @param list Pointer to list
 * @return Pointer to the new node
 */
_ui_node_p _ui_llist_add_first(_ui_llist_t* list);

/**
 * Insert a new node before the given \p node
 * @param list Pointer to list
 * @param node Reference node
 * @return Pointer to the new node
 */
_ui_node_p _ui_llist_insert_before(_ui_llist_t* list, _ui_node_p node);

/**
 * Insert a new node after the given \p node
 * @param list Pointer to list
 * @param node Reference node
 * @return Pointer to the new node
 */
_ui_node_p _ui_llist_insert_after(_ui_llist_t* list, _ui_node_p node);

/**
 * Remove the node \p node from the list
 * @note The node memory is not freed.
 * @param list Pointer to list
 * @param node Pointer to removed node
 */
void _ui_llist_remove(_ui_llist_t* list, _ui_node_p node);

/**
 * Remove and free all nodes from a list. The list becomes empty.
 * @param list Pointer to list
 */
void _ui_llist_clear(_ui_llist_t* list);

#define _UI_LLIST_READ(list, node) for(node = _ui_llist_get_first(list); node != NULL; node = _ui_llist_get_next(list, node))
#define _UI_LLIST_READ_BACK(list, node) for(node = _ui_llist_get_last(list); node != NULL; node = _ui_llist_get_prev(list, node))

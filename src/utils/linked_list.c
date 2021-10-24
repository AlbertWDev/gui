#include "utils/linked_list.h"

// List node:
// [     (node), (pointer to prev node), (pointer to next node) ]
// [ _ui_node_p,            _ui_node_p*,            _ui_node_p* ]

#define LIST_NODE_SIZE(list) (list->node_size + sizeof(_ui_node_p) + sizeof(_ui_node_p))
#define LIST_PREV_OFFSET(list) (list->node_size)
#define LIST_NEXT_OFFSET(list) (list->node_size + sizeof(_ui_node_p))

void _ui_llist_init(_ui_llist_t* list, size_t node_size) {
    list->head = NULL;
    list->tail = NULL;
    list->node_size = node_size;
}

_ui_node_p _ui_llist_get_first(const _ui_llist_t* list) {
    if(list == NULL) return NULL;
    return list->head;
}

_ui_node_p _ui_llist_get_last(const _ui_llist_t* list) {
    if(list == NULL) return NULL;
    return list->tail;
}

_ui_node_p _ui_llist_get_next(const _ui_llist_t* list, const _ui_node_p node) {
    if(list == NULL) return NULL;
    return *(_ui_node_p*)(node + LIST_NEXT_OFFSET(list));
}

_ui_node_p _ui_llist_get_prev(const _ui_llist_t* list, const _ui_node_p node) {
    if(list == NULL) return NULL;
    return *(_ui_node_p*)(node + LIST_PREV_OFFSET(list));
}

size_t _ui_llist_get_length(const _ui_llist_t* list) {
    size_t length = 0;

    for(_ui_node_p node = list->head; node != NULL; node = _ui_llist_get_next(list, node))
        length++;

    return length;
}

bool _ui_llist_is_empty(const _ui_llist_t* list) {
    if(list == NULL) return true;
    return list->head == NULL && list->tail == NULL;
}

static void _node_set_next(const _ui_llist_t* list, _ui_node_p node, const _ui_node_p next) {
    if(node == NULL) return;
    *(_ui_node_p*)(node + LIST_NEXT_OFFSET(list)) = next;
}

static void _node_set_prev(const _ui_llist_t* list, _ui_node_p node, const _ui_node_p prev) {
    if(node == NULL) return;
    *(_ui_node_p*)(node + LIST_PREV_OFFSET(list)) = prev;
}

_ui_node_p _ui_llist_add(_ui_llist_t* list) {
    _ui_node_p node = malloc(LIST_NODE_SIZE(list));
    if(node == NULL) return NULL;

    _node_set_prev(list, node, list->tail);
    _node_set_next(list, node, NULL);
    if(list->head == NULL) {
        list->head = node;
    } else {
        _node_set_next(list, list->tail, node);
    }
    list->tail = node;

    return node;
}

_ui_node_p _ui_llist_add_first(_ui_llist_t* list) {
    _ui_node_p node = malloc(LIST_NODE_SIZE(list));
    if(node == NULL) return NULL;

    _node_set_prev(list, node, NULL);
    _node_set_next(list, node, list->head);
    if(list->tail == NULL) {
        list->tail = node;
    } else {
        _node_set_prev(list, list->head, node);
    }
    list->head = node;

    return node;
}

_ui_node_p _ui_llist_insert_before(_ui_llist_t* list, _ui_node_p node) {
    if(node == list->head)
        return _ui_llist_add_first(list);
    
    _ui_node_p _node = malloc(LIST_NODE_SIZE(list));
    if(_node == NULL) return NULL;

    _ui_node_p prev = _ui_llist_get_prev(list, node);
    _node_set_next(list, prev, _node);
    _node_set_prev(list, _node, prev);
    _node_set_next(list, _node, node);
    _node_set_prev(list, node, _node);

    return _node;
}

_ui_node_p _ui_llist_insert_after(_ui_llist_t* list, _ui_node_p node) {
    if(node == list->tail)
        return _ui_llist_add(list);
    
    _ui_node_p _node = malloc(LIST_NODE_SIZE(list));
    if(_node == NULL) return NULL;

    _ui_node_p next = _ui_llist_get_next(list, node);
    _node_set_next(list, node, _node);
    _node_set_prev(list, _node, node);
    _node_set_next(list, _node, next);
    _node_set_prev(list, next, _node);
    
    return _node;
}

void _ui_llist_remove(_ui_llist_t* list, _ui_node_p node){
    if(node == list->head) {
        list->head = _ui_llist_get_next(list, node);
        if(list->head == NULL)
            list->tail = NULL;
        else
            _node_set_prev(list, list->head, NULL);
    }
    else if(node == list->tail) {
        list->tail = _ui_llist_get_prev(list, node);
        if(list->tail == NULL)
            list->head = NULL;
        else
            _node_set_next(list, list->tail, NULL);
    }
    else {
        _ui_node_p prev = _ui_llist_get_prev(list, node);
        _ui_node_p next = _ui_llist_get_next(list, node);

        _node_set_next(list, prev, next);
        _node_set_prev(list, next, prev);
    }
}

void _ui_llist_clear(_ui_llist_t* list) {
    for(_ui_node_p node = list->head, next = NULL; node != NULL; node = next) {
        next = _ui_llist_get_next(list, node);
        free(node);
    }
    list->head = NULL;
    list->tail = NULL;
}

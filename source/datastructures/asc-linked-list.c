#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key-value.h"
#include "linked.h"

struct AscLinkedList
{
    struct KeyValueSize size;

    void *head;
    void *tail;

    size_t length;
};
typedef struct AscLinkedList AscLinkedList;

size_t Asc_GetNodeSize(size_t key_size, size_t value_size)
{
    return sizeof(void*) + key_size + value_size;
}

Node *get_next(Node *node)
{
    return *(void**)node;
}

Node set_next(Node *node, Node *next)
{
    *(void**)node = next;
}

Node *new_node(struct KeyValueSize *size)
{
    Node *node = malloc(Asc_GetNodeSize(size->key, size->value));

    set_next(node, 0);

    return node;
}

void *get_key(Node *node)
{
    return node + sizeof(void*);
}

void *get_value(struct KeyValueSize *size, Node *node)
{
    return node + sizeof(void*) + size->key;
}

struct KeyValuePtr get_ptrs(struct KeyValueSize *size, Node *node)
{
    return (struct KeyValuePtr){
        .key = get_key(node),
        .value = get_value(size, node)
    };
}

void set_ptrs(struct KeyValueSize *size, struct KeyValuePtr ptrs, void *key, void *value)
{
    memcpy(ptrs.key, key, size->key);
    memcpy(ptrs.value, value, size->value);
}

void set_node(struct KeyValueSize *size, void *node, void *key, void *value)
{
    struct KeyValuePtr ptrs = get_ptrs(size, node);

    set_ptrs(size, ptrs, key, value);
}

struct KeyValueSize *get_size(AscLinkedList *list)
{
    return (struct KeyValueSize*)list;
}

AscLinkedList NewAssociativeLinkedList(size_t key_size, size_t value_size)
{
    return (AscLinkedList){
        .size = { .key = key_size, .value = value_size },

        .head = 0, .tail = 0
    };
}

Node *find_node(struct KeyValueSize *size, Node *head, void *key)
{
    Node *next = get_next(head);

    if(next == head)
    {
        if(memcmp(get_key(head), key, size->key) == 0)
            return head;

        return 0;
    }

    for(; next != 0; next = get_next(next))
    {
        if(memcmp(get_key(next), key, size->key) == 0)
            return next;
    }

    return 0;
}

Node *find_node_parent(struct KeyValueSize *size, Node *head, void *key)
{
    if(memcmp(get_key(head), key, size->key) == 0)
        return head;
    
    Node *child = get_next(head);

    if(child == head)
        return 0;

    Node *parent = head;

    for(; child != 0; child = get_next(child))
    {
        if(memcmp(get_key(child), key, size->key) == 0)
            return parent;

        parent = child;
    }

    return 0;
}

void *AscNode_Find(struct KeyValueSize *size, void *head, void *key)
{
    Node *node = find_node(size, head, key);

    if(node == 0)
        return 0;
    
    return get_value(size, node);
}

void *Asc_Find(AscLinkedList *list, void *key)
{
    return AscNode_Find(&list->size, list->head, key);
}

Node *AscNode_AddFirst(struct KeyValueSize *size, Node *head, void *key, void *value)
{
    if(AscNode_Find(size, head, key))
        return 0;

    void *next_node = get_next(head);

    struct KeyValuePtr head_ptrs = get_ptrs(size, head);

    void *insert;

    if(next_node == 0)
    {
        set_next(head, head);
        insert = head;
    }
    else
    {
        insert = new_node(size);

        if(next_node == head)
        {
            set_next(head, insert);
        }
        else
        {
            set_next(insert, next_node);
            set_next(head, insert);
        }

        set_node(size, insert, head_ptrs.key, head_ptrs.value);
    }
    
    set_ptrs(size, head_ptrs, key, value);

    return insert;
}

bool Asc_AddFirst(AscLinkedList *list, void *key, void *value)
{
    struct KeyValueSize *size = get_size(list);

    if(list->head == 0)
    {
        list->head = new_node(size);
        list->tail = list->head;
    }

    void *insert = AscNode_AddFirst(size, list->head, key, value);

    if(!insert)
        return false;
    
    list->length++;

    if(list->length == 2)
        list->tail = insert;

    return true;
}

void *AscNode_Index(struct KeyValueSize *size, void *head, size_t index)
{
    if(index == 0)
        return head;

    void *next = head;

    for(size_t i = 0; i < index; i++)
    {
        next = get_next(next);

        if(next == 0)
            return 0;
    }

    return next;
}

struct KeyValuePtr Asc_Index(AscLinkedList *list, size_t index)
{
    if(index >= list->length)
    {
        printf("AscGet: Index %llu out of bounds for length %llu", index, list->length);
        exit(1);
    }

    struct KeyValueSize *size = get_size(list);

    void *node = AscNode_Index(size, list->head, index);

    return get_ptrs(size, node);
}

bool AscNode_Remove(struct KeyValueSize *size, Node *head, void *key)
{
    Node *parent = find_node_parent(size, head, key);

    if(parent == 0)
        return false;

    Node *child = get_next(parent);

    if(parent == head)
    {
        size_t node_size = Asc_GetNodeSize(size->key, size->value);

        if(child == head)
        {
            memset(head, 0, node_size);
        }
        else
        {
            memcpy(head, child, node_size);
            free(child);
        }
    }
    else
    {
        set_next(parent, get_next(child));
        free(child);
    }

    return true;
}

bool Asc_Remove(AscLinkedList *list, void *key)
{
    struct KeyValueSize *size = get_size(list);

    bool success = AscNode_Remove(size, list->head, key);

    if(success)
        list->length--;

    return success;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "linked.h"

struct LinkedList
{
    size_t value_size;

    Node *head;
    Node *tail;

    size_t length;
};
typedef struct LinkedList LinkedList;

LinkedList NewLinkedList(size_t value_size)
{
    return (LinkedList){
        .value_size = value_size,

        .head = 0,

        .length = 0
    };
}

static Node *get_next(Node *node)
{
    return *(void**)node;
}

static void set_next(Node *node, Node *next)
{
    *(void**)node = next;
}

static void *get_value(Node *node)
{
    return node + sizeof(void*);
}

static Node *new_node(size_t value_size, void *value)
{
    Node *node = malloc(sizeof(void*) + value_size);
    
    set_next(node, 0);
    memcpy(node + sizeof(void*), value, value_size);

    return node;
}

void LinkedList_AddLast(LinkedList *list, void *value)
{
    Node *node = new_node(list->value_size, value);
    list->length++;

    if(list->head == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        set_next(list->tail, node);
    }
}

void LinkedList_AddFirst(LinkedList *list, void *value)
{
    Node *node = new_node(list->value_size, value);
    list->length++;

    if(list->head == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        set_next(node, list->head);
        list->head = node;
    }
}

static Node *index_node(LinkedList *list, size_t index)
{
    if(index > list->length)
    {
        printf("LinkedList: Index %llu out of bounds for list of length %llu\n", index, list->length);
        exit(1);
    }

    Node *next = list->head;

    for(size_t i = 0; i < index; i++)
    {
        next = get_next(next);
    }

    return next;
}

void *LinkedList_Index(LinkedList *list, size_t index)
{
    Node *node = index_node(list, index);

    return get_value(node);
}

void LinkedList_Insert(LinkedList *list, size_t index, void *value)
{
    if(index == 0)
    {
        LinkedList_AddFirst(list, value);
        return;
    }

    if(index == list->length)
    {
        LinkedList_AddLast(list, value);
        return;
    }

    Node *node = new_node(list->value_size, value);
    list->length++;

    Node *parent = index_node(list, index - 1);

    set_next(node, get_next(parent));
    set_next(parent, node);
}

void LinkedList_Remove(LinkedList *list, size_t index)
{
    if(index == 0)
    {
        Node *node = list->head;

        list->head = get_next(node);

        free(node);
    }
    else
    {
        Node *parent = index_node(list, index - 1);

        Node *child = get_next(parent);

        set_next(parent, get_next(child));

        free(child);
    }

    list->length--;
}
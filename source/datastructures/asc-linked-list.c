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

Node *new_node(struct KeyValueSize *size);

Node *get_next(Node *node);
void set_next(Node *node, Node *next);

struct KeyValuePtr get_ptrs(struct KeyValueSize *size, Node *node);

void set_ptrs(struct KeyValueSize *size, struct KeyValuePtr ptrs, void *key, void *value);
void set_node(struct KeyValueSize *size, void *node, void *key, void *value);

struct KeyValueSize *get_size(AscLinkedList *list);

AscLinkedList NewAssociativeLinkedList(size_t key_size, size_t value_size)
{
    return (AscLinkedList){
        .size = { .key = key_size, .value = value_size },

        .head = 0, .tail = 0
    };
}

size_t Asc_GetNodeSize(size_t key_size, size_t value_size)
{
    return sizeof(void*) + key_size + value_size;
}

void *Asc_Find(struct KeyValueSize *size, void *head, void *key)
{
    Node *node = head;

    while(node != 0 && node != head)
    {
        struct KeyValuePtr node_ptrs = get_ptrs(size, node);

        if(memcmp(node_ptrs.key, key, size->key) == 0)
            return node_ptrs.value;

        node = get_next(node);
    }

    return 0;
}

void *AscNode_AddFirst(struct KeyValueSize *size, Node *head, void *key, void *value)
{
    if(Asc_Find(size, head, key))
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

bool Asc_AddLast(AscLinkedList *list, void *key, void *value)
{
    struct KeyValueSize *size = get_size(list);

    void *last = new_node(size);

    set_node(size, last, key, value);
    set_next(list->tail, last);
    list->tail = last;
}

void *Asc_IndexNode(struct KeyValueSize *size, void *head, size_t index)
{
    if(index == 0)
        return head;

    void *next_node = head;

    for(int i = 0; i < index; i++)
    {
        next_node = get_next(next_node);

        if(next_node == 0)
            return 0;
    }

    return next_node;
}

struct KeyValuePtr Asc_Index(AscLinkedList *list, size_t index)
{
    if(index >= list->length)
    {
        printf("AscGet: Index %u out of bounds for length %u", index, list->length);
        exit(1);
    }

    struct KeyValueSize *size = get_size(list);

    void *node = Asc_IndexNode(size, list->head, index);

    return get_ptrs(size, node);
}

uint8_t Asc_Remove(struct KeyValueSize *size, void *key)
{
    
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

struct KeyValuePtr get_ptrs(struct KeyValueSize *size, Node *node)
{
    void *key = node + sizeof(void*);
    void *value = node + sizeof(void*) + size->key;

    return (struct KeyValuePtr){
        .key = key,
        .value = value
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

int main()
{
    AscLinkedList list = NewAssociativeLinkedList(1, 1);

    Asc_AddFirst(&list, "a", "1");
    Asc_AddFirst(&list, "b", "2");
    Asc_AddFirst(&list, "c", "3");
    Asc_AddFirst(&list, "d", "4");

    char k = *(char*)Asc_Index(&list, 0).key;
    char v = *(char*)Asc_Index(&list, 0).value;
    printf("%c: %c\n", k, v);

    k = *(char*)Asc_Index(&list, 1).key;
    v = *(char*)Asc_Index(&list, 1).value;
    printf("%c: %c\n", k, v);

    k = *(char*)Asc_Index(&list, 2).key;
    v = *(char*)Asc_Index(&list, 2).value;
    printf("%c: %c\n", k, v);

    k = *(char*)Asc_Index(&list, 3).key;
    v = *(char*)Asc_Index(&list, 3).value;
    printf("%c: %c\n", k, v);

    return 0;
}
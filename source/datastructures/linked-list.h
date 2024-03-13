#pragma once
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

LinkedList NewLinkedList(size_t value_size);

void LinkedList_AddLast(LinkedList *list, void *value);

void LinkedList_AddFirst(LinkedList *list, void *value);

void *LinkedList_Index(LinkedList *list, size_t index);

void LinkedList_Insert(LinkedList *list, size_t index, void *value);

void LinkedList_Remove(LinkedList *list, size_t index);
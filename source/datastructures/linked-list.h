#pragma once
#include <stdint.h>

struct LinkedList
{
    int valueSize;
    void **head;
    unsigned int length;
};

struct LinkedList *NewLinkedList(int valueSize);

void *LinkedListGet(struct LinkedList *list, int index);

void LinkedListAdd(struct LinkedList *list, void *value);

void LinkedListInsert(struct LinkedList *list, void *value, int index);

void LinkedListRemove(struct LinkedList *list, int index);
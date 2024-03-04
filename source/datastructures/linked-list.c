#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct LinkedList
{
    int valueSize;
    void **head;
    unsigned int length;
};

struct LinkedList *NewLinkedList(int valueSize)
{
    struct LinkedList *list = malloc(sizeof(struct LinkedList));

    list->valueSize = valueSize;
    list->head = 0;
    list->length = 0;

    return list;
}

void **linkedListGetNode(struct LinkedList* list, int index)
{
    void **current = list->head;

    for(int i = 0; i < index; i++)
    {
        current = *current;
    }

    return current;
}

void **newLinkedListNode(struct LinkedList *list, char *value)
{
    char *node = malloc(sizeof(void*) + list->valueSize);
    for(int i = 0; i < list->valueSize; i++)
    {
        node[sizeof(void*) + i] = value[i];
    }
    *(void**)node = 0;

    return (void**)node;
}

void *LinkedListGet(struct LinkedList *list, int index)
{
    return (char*)linkedListGetNode(list, index) + sizeof(void*);
}

void LinkedListAdd(struct LinkedList *list, void *value)
{
    if(list->head == 0)
    {
        list->head = newLinkedListNode(list, value);
    }
    else
    {
        void **parentNode = linkedListGetNode(list, list->length-1);
        *parentNode = newLinkedListNode(list, value);
    }
    list->length++;
}

void LinkedListInsert(struct LinkedList *list, void *value, int index)
{
    void **newNode = newLinkedListNode(list, value);

    if(index == 0)
    {
        *newNode = list->head;
        list->head = newNode;
    }
    else
    {
        void **parentNode = linkedListGetNode(list, index-1);
        *newNode = *parentNode;
        *parentNode = newNode;
    }
    list->length++;
}

void LinkedListRemove(struct LinkedList *list, int index)
{
    if(index == 0)
    {
        void **newHeadNode = *list->head;
        free(list->head);
        list->head = newHeadNode;
    }
    else
    {
        void **parentNode = linkedListGetNode(list, index-1);
        void **targetNode = (void**)*parentNode;
        *parentNode = *targetNode;
        free(targetNode);
    }
    list->length--;
}
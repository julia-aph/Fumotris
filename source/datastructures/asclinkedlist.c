#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "keyvalue.h"

struct AscLinkedList
{
    KeyValueSize size;
    void *head;

    size_t length;
};

typedef struct AscLinkedList AscLinkedList;

size_t AscNodeSize(size_t keySize, size_t valueSize)
{
    return sizeof(void*) + keySize + valueSize;
}

AscLinkedList NewAscLinkedList(size_t keySize, size_t valueSize)
{
    return (AscLinkedList){
        .size = { .key = keySize, .value = valueSize },

        .head = 0
    };
}

void *getNext(void *node)
{
    return *(void**)node;
}

void setNext(void *node, void *next)
{
    *(void**)node = next;
}

void *NewAscLinkedListNode(KeyValueSize *size)
{
    void *node = malloc(AscNodeSize(size->key, size->value));

    setNext(node, 0);

    return node;
}

KeyValuePtr getKeyValuePtr(KeyValueSize *size, void *node)
{
    void *key = node + sizeof(void*);
    void *value = node + sizeof(void*) + size->key;

    return (KeyValuePtr){
        .key = key,
        .value = value
    };
}

KeyValueResult AscFind(KeyValueSize *size, void *head, void *key)
{
    void *node = head;

    while(node != 0 && node != head)
    {
        KeyValuePtr nodePtr = getKeyValuePtr(size, node);

        if(memcmp(nodePtr.key, key, size->key) == 0)
            return KeyValueResultSuccess(nodePtr);

        node = getNext(node);
    }

    return KeyValueResultFail();
}

uint8_t AscPushFrontAt(KeyValueSize *size, void *head, void *key, void *value)
{
    if(AscFind(size, head, key).success)
        return 0;

    void *nextNode = getNext(head);

    KeyValuePtr headPtr = getKeyValuePtr(size, head);

    if(nextNode == 0)
    {
        setNext(head, head);
    }
    else
    {
        void *insert = NewAscLinkedListNode(size);

        if(nextNode == head)
        {
            setNext(head, insert);
        }
        else
        {
            setNext(insert, nextNode);
            setNext(head, insert);
        }

        KeyValuePtr insertPtr = getKeyValuePtr(size, insert);

        memcpy(insertPtr.key, headPtr.key, size->key);
        memcpy(insertPtr.value, headPtr.value, size->value);
    }
    
    memcpy(headPtr.key, key, size->key);
    memcpy(headPtr.value, value, size->value);
}

uint8_t AscPushFront(AscLinkedList *list, void *key, void *value)
{
    KeyValueSize *size = (KeyValueSize*)list;

    if(list->head == 0)
        list->head = NewAscLinkedListNode(size);

    uint8_t success = AscPushFrontAt(size, list->head, key, value);
    if(success)
        list->length++;

    return success;
}

void *AscGetNode(KeyValueSize *size, void *head, size_t index)
{
    if(index == 0)
        return head;

    void *nextNode = head;

    for(int i = 0; i < index; i++)
    {
        nextNode = getNext(nextNode);

        if(nextNode == 0)
            return 0;
    }

    return nextNode;
}

KeyValueResult AscGetKeyValue(KeyValueSize *size, void *head, size_t index)
{
    void *node = AscGetNode(size, head, index);

    if(node == 0)
        return KeyValueResultFail();

    return KeyValueResultSuccess(getKeyValuePtr(size, node));
}

KeyValuePtr AscGet(AscLinkedList *list, size_t index)
{
    if(index >= list->length)
    {
        printf("AscGet: Index %u out of bounds for length %u", index, list->length);
        exit(1);
    }

    KeyValueSize *size = (KeyValueSize*)list;

    return AscGetKeyValue(size, list->head, index).ptrs;
}

uint8_t AscRemove()
{
    
}

int main()
{
    AscLinkedList list = NewAscLinkedList(1, 1);

    AscPushFront(&list, "a", "1");
    AscPushFront(&list, "b", "2");
    AscPushFront(&list, "c", "3");
    AscPushFront(&list, "d", "4");

    char k = *(char*)AscGet(&list, 0).key;
    char v = *(char*)AscGet(&list, 0).value;
    printf("%c: %c\n", k, v);

    k = *(char*)AscGet(&list, 1).key;
    v = *(char*)AscGet(&list, 1).value;
    printf("%c: %c\n", k, v);

    k = *(char*)AscGet(&list, 2).key;
    v = *(char*)AscGet(&list, 2).value;
    printf("%c: %c\n", k, v);

    k = *(char*)AscGet(&list, 3).key;
    v = *(char*)AscGet(&list, 3).value;
    printf("%c: %c\n", k, v);

    return 0;
}
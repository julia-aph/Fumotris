#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* ext code - add to another lib later */
struct KeyValueSize
{
    size_t key;
    size_t value;
};

typedef struct KeyValueSize KeyValueSize;

struct AscLinkedList
{
    struct KeyValueSize size;
    void *head;
};

typedef struct AscLinkedList AscLinkedList;

struct KeyValuePtr
{
    void *key;
    void *value;
};

typedef struct KeyValuePtr KeyValuePtr;

size_t AscLinkedListNodeSize(size_t keySize, size_t valueSize)
{
    return sizeof(void*) + keySize + valueSize;
}

AscLinkedList NewAscLinkedList(size_t keySize, size_t valueSize)
{
    return (AscLinkedList){
        .size = (KeyValueSize){
            .key = keySize, .value = valueSize
        },

        .head = 0
    };
}

KeyValuePtr getKeyValuePair(KeyValueSize *size, uint8_t *node)
{
    return (KeyValuePtr){
        .key = node + sizeof(void*),
        .value = node + sizeof(void*) + size->key
    };
}

uint8_t AscLinkedListAddFirstToNode(KeyValueSize *size, void *node, void *key, void *value)
{
    void *next = *(void**)node;

    if(next == 0)
    {
        KeyValuePtr pair = getKeyValuePair(size, node);

        next = node;
        memcpy(pair.key, key, size->key);
        memcpy(pair.value, value, size->value);
    }
    else if(next == node)
    {
        next = malloc(sizeof(void*) + size->key + size->value);

        uint8_t *node = next;

        KeyValuePtr pair = getKeyValuePair(size, node);

        *(void**)node = 0;
        memcpy(pair.key, key, size->key);
        memcpy(pair.value, value, size->value);
    }
}

uint8_t AscLinkedListAddFirst(AscLinkedList *list, void *key, void *value)
{
    if(list->head == 0)
    {
        list->head = malloc(AscLinkedListNodeSize(list->size.key, list->size.value));
    }

    return AscLinkedListAddFirstToNode((KeyValueSize*)list, list->head, key, value);
}

KeyValuePtr AscLinkedListGetFromNode(KeyValueSize *size, void *node, size_t index)
{
    if(index == 0)
        return getKeyValuePair(size, node);

    void *next;

    for(int i = 0; i < index; i++)
    {
        next = *(void**)node;

        if(next == 0)
            return (KeyValuePtr){ 0, 0 };
    }

    return getKeyValuePair(size, next);
}

KeyValuePtr AscLinkedListGet(AscLinkedList *list, size_t index)
{
    return AscLinkedListGetFromNode((KeyValueSize*)list, list->head, index);
}

int main()
{
    AscLinkedList list = NewAscLinkedList(1, 1);

    AscLinkedListAddFirst(&list, "a", "1");

    for(int i = 0; i < 20; i++)
    {
        printf("%u ", ((char*)list.head)[i]);
    }

    char *whar = AscLinkedListGet(&list, 0).value;


    return 0;
}
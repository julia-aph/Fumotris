#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct KeyValueSize
{
    size_t key;
    size_t value;
};

typedef struct KeyValueSize KeyValueSize;

void *getKeyValuePtr(KeyValueSize *size, uint8_t node, uint8_t **key, uint8_t **value)
{
    *key = node + sizeof(void*);
    *value = node + sizeof(void*) + size->key;
}

void *NewAscLinkedList(size_t keySize, size_t valueSize)
{
    uint8_t *object = malloc(sizeof(void*) + keySize + valueSize);

    void *next = object;
    *(void**)next = 0;

    return object;
}

uint8_t AscLinkedListAddFirst(KeyValueSize *size, void *node, uint8_t *key, uint8_t *value)
{
    void *next = *(void**)node;

    if(next == 0)
    {
        uint8_t *currentKey;
        uint8_t *currentValue;
        getKeyValuePtr(size, node, &currentKey, &currentValue);

        next = node;
        memcpy(currentKey, key, size->key);
        memcpy(currentValue, value, size->value);
    }
    else if(next == node)
    {
        next = malloc(sizeof(void*) + size->key + size->value);

        uint8_t *node = next;

        uint8_t *currentKey;
        uint8_t *currentValue;
        getKeyValuePtr(size, node, &currentKey, &currentValue);

        *(void**)node = 0;
        memcpy(currentKey, key, size->key);
        memcpy(currentValue, value, size->value);
    }
}

uint8_t 
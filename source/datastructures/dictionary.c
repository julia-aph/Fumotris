#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct KeyValueSize
{
    size_t key;
    size_t value;
};

struct Dictionary
{
    struct KeyValueSize size;

    size_t bucketSize;

    size_t capacity;
    size_t used;

    uint8_t buckets[];
};

typedef struct Dictionary Dictionary;

Dictionary NewDictionary(size_t keySize, size_t valueSize)
{
    static size_t initialCapacity = 16;

    size_t bucketSize = keySize + valueSize;

    Dictionary *dict = malloc(sizeof(Dictionary) + initialCapacity * bucketSize);

    *dict = (Dictionary){
        .size.key = keySize, .size.value = valueSize,

        .bucketSize = bucketSize,

        .capacity = initialCapacity, .used = 0
    };

    memset(dict->buckets, 0, dict->capacity * bucketSize);
}

void *getBucket(Dictionary *dict, size_t index)
{
    return dict->buckets + index * dict->bucketSize;
}

void *getList(Dictionary *dict, uint32_t hash)
{
    size_t index = hash % dict->capacity;

    return getBucket(dict, index);
}

uint8_t DictAdd(Dictionary *dict, void *key, void *value)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *list = getList(dict, hash);
    
    uint8_t pair[dict->size.key + dict->size.value];

    memcpy(pair, key, dict->size.key);
    memcpy(pair + dict->size.key, value, dict->size.value);

    return ALLAddFirst(list, &hash, &pair);
}

void *DictGet(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *list = getList(dict, hash);
    
    uint8_t *pair = AscLinkedListFind(list, &hash);

    if(pair == 0)
        return 0;

    return pair + dict->size.key;
}

uint8_t DictContainsKey(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *list = getBucket(dict, hash);

    return AscLinkedListFind(list, &hash) != 0;
}

uint8_t DictRemove(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *list = getBucket(dict, hash);

    return AscLinkedListRemove(list, &hash);
}
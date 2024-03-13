#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "asc-linked-list.h"
#include "hash.h"

struct Dictionary
{
    struct KeyValueSize size;
    struct KeyValueSize asc_size;

    size_t bucket_size;

    size_t capacity;
    size_t used;

    uint8_t buckets[];
};

typedef struct Dictionary Dictionary;

Dictionary *NewDictionary(size_t key_size, size_t value_size)
{
    static size_t initial_capacity = 16;

    size_t bucket_size = Asc_GetNodeSize(sizeof(uint32_t), key_size + value_size);

    Dictionary *dict = malloc(sizeof(Dictionary) + initial_capacity * bucket_size);

    *dict = (Dictionary){
        .size.key = key_size, .size.value = value_size,
        .asc_size.key = sizeof(uint32_t), .asc_size.value = key_size + value_size,

        .bucket_size = bucket_size,

        .capacity = initial_capacity, .used = 0
    };

    memset(dict->buckets, 0, dict->capacity * bucket_size);

    return dict;
}

void *get_bucket(Dictionary *dict, uint32_t hash)
{
    size_t index = hash % dict->capacity;

    return dict->buckets + index * dict->bucket_size;
}

static void *get_value(Dictionary *dict, uint8_t *pair)
{
    return pair + dict->size.key;
}

bool DictAdd(Dictionary *dict, void *key, void *value)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *bucket = get_bucket(dict, hash);
    
    uint8_t pair[dict->size.key + dict->size.value];

    memcpy(pair, key, dict->size.key);
    memcpy(get_value(dict, pair), value, dict->size.value);

    return AscNode_AddFirst(&dict->asc_size, bucket, &hash, &pair) != 0;
}

void *DictGet(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *bucket = get_bucket(dict, hash);
    
    uint8_t *pair = AscNode_Find(&dict->asc_size, bucket, &hash);

    if(pair == 0)
        return 0;
        
    return get_value(dict, pair);
}

bool DictContainsKey(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *bucket = get_bucket(dict, hash);

    return AscNode_Find(&dict->asc_size, bucket, &hash) != 0;
}

bool DictRemove(Dictionary *dict, void *key)
{
    uint32_t hash = Hash(key, dict->size.key);

    void *bucket = get_bucket(dict, hash);

    return AscNode_Remove(&dict->asc_size, bucket, &hash);
}
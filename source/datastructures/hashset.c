#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct HashSet
{
    size_t bucketSize;
    size_t itemSize;
    size_t linkOffset;

    size_t capacity;
    size_t used;

    // unsigned int: hash, itemSize: item, void *: link
    void *buckets;
};

struct HashSet *NewHashSet(size_t itemSize)
{
    struct HashSet *set = malloc(sizeof(struct HashSet));

    set->bucketSize = sizeof(uint32_t) + itemSize + sizeof(void*);
    set->itemSize = itemSize;
    set->linkOffset = sizeof(uint32_t) + set->itemSize;

    set->capacity = 16;
    set->used = 0;

    set->buckets = calloc(set->capacity, set->bucketSize);

    return set;
}

void hashSetLink(struct HashSet *set, void *item, void *bucket, uint32_t hash)
{
    void *node = malloc(set->bucketSize);
    
    void **next = bucket + set->linkOffset;
    while(*next != 0)
    {
        next = *next + set->linkOffset;
    }
    *next = node;

    *(uint32_t*)node = hash;
    memcpy(node + sizeof(uint32_t), item, set->itemSize);
    void **link = node + set->linkOffset;
    *link = 0;
}

void HashSetAdd(struct HashSet *set, void *item)
{
    uint32_t hash = Hash(item, set->itemSize);
    size_t index = hash % set->capacity;

    void *bucket = set->buckets + set->bucketSize * index;
    uint32_t *bucketHash = bucket;

    if(hash == *bucketHash) // Might not work 100% of the time depending on the hash function
        return;

    if(*bucketHash == 0)
    {
        *bucketHash = hash;
        memcpy(bucket + sizeof(uint32_t), item, set->itemSize);
    }
    else // Collision
    {
        printf("collision\n");
        hashSetLink(set, item, bucket, hash);
    }
}

struct HashSet *NewHashSetFrom(void *items, size_t length, size_t itemSize)
{
    struct HashSet *set = NewHashSet(itemSize);
    
    for(size_t i = 0; i < length; i++)
    {
        HashSetAdd(set, items + itemSize * i);
    }

    return set;
}

uint8_t HashSetContains(struct HashSet *set, void *item)
{
    uint32_t hash = Hash(item, set->itemSize);
    size_t index = hash % set->capacity;

    void *bucket = set->buckets + set->bucketSize * index;
    uint32_t bucketHash = *(uint32_t*)bucket;

    if(hash == bucketHash)
        return 1;

    void **next = bucket + set->linkOffset;
    while(*next != 0)
    {
        uint32_t nodeHash = *(uint32_t*)*next;
        if(hash == nodeHash)
            return 1;

        next = *next + set->linkOffset;
    }

    return 0;
}
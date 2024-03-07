#pragma once
#include <stdint.h>

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

struct HashSet *NewHashSet(size_t itemSize);

void HashSetAdd(struct HashSet *set, void *item);

struct HashSet *NewHashSetFrom(void *items, size_t length, size_t itemSize);

uint8_t HashSetContains(struct HashSet *set, void *item);
#pragma once
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

struct Dictionary *NewDictionary(size_t key_size, size_t value_size);

bool DictAdd(struct Dictionary *dict, void *key, void *value);

void *DictGet(struct Dictionary *dict, void *key);

bool DictContainsKey(struct Dictionary *dict, void *key);

bool DictRemove(struct Dictionary *dict, void *key);
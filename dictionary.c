#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct dictBucket
{
    void *key;
    unsigned int hash;
    void *value;
};

struct Dictionary
{
    struct dictBucket *buckets;
    unsigned int capacity;
    unsigned int used;

    unsigned int keySize;
};

struct Dictionary *NewDictionaryAllocate(unsigned int capacity, unsigned int keySize)
{
    struct Dictionary *dict = malloc(sizeof(struct Dictionary));
    dict->buckets = calloc(capacity, sizeof(struct dictBucket));
    dict->capacity = capacity;
    dict->used = 0;

    dict->keySize = keySize;
}

struct Dictionary *NewDictionary(unsigned int keySize)
{
    return NewDictionaryAllocate(16, keySize);
}

int dictLinearProbe(struct Dictionary *dict, int index, int search)
{
    if(dict->buckets[index].hash == search)
        return index;

    for(int i = 1; i < dict->capacity; i++)
    {
        unsigned int check = (index + i / 2 - (i % 2) * i) % dict->capacity;
        if(dict->buckets[check].hash == search)
            return check;
    }

    return -1;
}

void DictAdd(struct Dictionary *dict, void *key, void *value)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, 0);
    if(probe == -1)
        return; // Dictionary is full

    dict->buckets[probe] = (struct dictBucket){ key, hash, value };
    dict->used++;
}

/*void DictRemove(struct Dictionary *dict, void *key) // Make compatible with linear probe
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;
    dict->buckets[index].value = 0;
}*/

void *DictGet(struct Dictionary *dict, void *key)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, hash);
    if(probe == -1)
        return 0; // Not found

    return dict->buckets[probe].value;
}

unsigned char DictContainsKey(struct Dictionary *dict, void *key)
{
    unsigned int hash = Hash(key, dict->keySize);
    unsigned int index = hash % dict->capacity;

    int probe = dictLinearProbe(dict, index, hash);
    return probe == -1;
}
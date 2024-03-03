#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Dictionary
{
    struct dictBucket *buckets;
    unsigned int capacity;
    unsigned int used;

    unsigned int keySize;
};

struct Dictionary *NewDictionaryAllocate(unsigned int capacity, unsigned int keySize);

struct Dictionary *NewDictionary(unsigned int keySize);

void DictAdd(struct Dictionary *dict, void *key, void *value);

void *DictGet(struct Dictionary *dict, void *key);

unsigned char DictContainsKey(struct Dictionary *dict, void *key);
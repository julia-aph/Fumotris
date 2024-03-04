#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct KeyValueSize
{
    size_t key;
    size_t value;
};

typedef struct KeyValueSize KeyValueSize;

struct KeyValuePtr
{
    void *key;
    void *value;
};

typedef struct KeyValuePtr KeyValuePtr;

struct KeyValueResult
{
    uint8_t success;
    KeyValuePtr ptrs;
};

typedef struct KeyValueResult KeyValueResult;

KeyValueResult KeyValueResultSuccess(KeyValuePtr ptrs)
{
    return (KeyValueResult){
        .success = 1, .ptrs = ptrs
    };
}

KeyValueResult KeyValueResultFail()
{
    return (KeyValueResult){
        .success = 0
    };
}
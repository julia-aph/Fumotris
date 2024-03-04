#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct KeyValueSize
{
    size_t key;
    size_t value;
};

struct KeyValuePtr
{
    void *key;
    void *value;
};

struct KeyValueResult
{
    uint8_t success;
    struct KeyValuePtr ptrs;
};

struct KeyValueResult KeyValueResultSuccess(struct KeyValuePtr ptrs)
{
    return (struct KeyValueResult){
        .success = 1, .ptrs = ptrs
    };
}

struct KeyValueResult KeyValueResultFail()
{
    return (struct KeyValueResult){
        .success = 0
    };
}
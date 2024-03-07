#pragma once
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
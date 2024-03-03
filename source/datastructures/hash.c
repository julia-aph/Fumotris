#include <stdint.h>

uint32_t Hash(void *item, size_t size)
{
    uint8_t* data = (uint8_t*)item;

    uint32_t h = 98317;

    for(size_t i = 0; i < size; i++)
    {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }

    return h;
}
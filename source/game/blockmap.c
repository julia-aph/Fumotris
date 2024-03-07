#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct BlockMap
{
    size_t width;
    size_t height;
    size_t area;

    int x;
    int y;

    uint8_t rotation;
    uint8_t blocks[];
};

struct BlockMap *NewBlockMap(size_t width, size_t height)
{
    size_t area = width * height;

    struct BlockMap *map = malloc(sizeof(struct BlockMap) + area * sizeof(uint8_t));
    memset(map->blocks, 0, area);

    *map = (struct BlockMap)
    {
        .width = width, .height = height, .area = area,

        .x = 0, .y = 0,
        
        .rotation = 0
    };

    return map;
}

struct BlockMap *NewBlockMapFrom(size_t width, size_t height, uint8_t *blocks)
{
    size_t area = width * height;

    struct BlockMap *map = malloc(sizeof(struct BlockMap) + area * sizeof(uint8_t));
    memcpy(map->blocks, blocks, area);

    *map = (struct BlockMap)
    {
        .width = width, .height = height, .area = area,

        .x = 0, .y = 0,
        
        .rotation = 0
    };

    return map;
}

void DestroyBlockMap(struct BlockMap *map)
{
    free(map);
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct BlockMap
{
    size_t width;
    size_t height;
    size_t area;

    int x;
    int y;

    uint8_t rotation;
    uint8_t *blocks;
};

struct BlockMap *NewBlockMap(size_t width, size_t height)
{
    size_t area = width * height;

    uint8_t *blocks = malloc(area * sizeof(unsigned char));
    
    // Fill with default value
    for(size_t i = 0; i < area; i++)
    {
        blocks[i] = 0;
    }

    struct BlockMap *blockMap = malloc(sizeof(struct BlockMap));
    *blockMap = (struct BlockMap)
    {
        .width = width, .height = height, .area = area,

        .x = 0, .y = 0,
        
        .rotation = 0, .blocks = blocks
    };

    return blockMap;
}

struct BlockMap *NewBlockMapFrom(size_t width, size_t height, uint8_t *blocks)
{
    struct BlockMap *blockMap = malloc(sizeof(struct BlockMap));
    *blockMap = (struct BlockMap)
    {
        .width = width, .height = height, .area = width * height,

        .x = 0, .y = 0,
        
        .rotation = 0, .blocks = blocks
    };

    return blockMap;
}

void DestroyBlockMap(struct BlockMap *blockMap)
{
    free(blockMap->blocks);
    free(blockMap);
}
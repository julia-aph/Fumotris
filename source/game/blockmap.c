#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"

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

static uint8_t *BLOCK_COLORS = 0;

void BlockColorsInit(uint8_t *colors, size_t length)
{
    BLOCK_COLORS = NewVectorAllocate(1, length);
    memcpy(BLOCK_COLORS, colors, length);
}

struct BlockMap *NewBlockMap(size_t width, size_t height)
{
    size_t area = width * height;

    struct BlockMap *map = malloc(sizeof(struct BlockMap) + area * sizeof(uint8_t));
    *map = (struct BlockMap)
    {
        .width = width, .height = height, .area = area,

        .x = 0, .y = 0,
        
        .rotation = 0
    };
    memset(map->blocks, 0, area);

    return map;
}

struct BlockMap *NewBlockMapFrom(size_t width, size_t height, uint8_t *blocks)
{
    size_t area = width * height;

    struct BlockMap *map = malloc(sizeof(struct BlockMap) + area * sizeof(uint8_t));
    *map = (struct BlockMap)
    {
        .width = width, .height = height, .area = area,

        .x = 0, .y = 0,
        
        .rotation = 0
    };
    memcpy(map->blocks, blocks, area);

    return map;
}

void DestroyBlockMap(struct BlockMap *map)
{
    free(map);
}

size_t rotate_index(size_t i, size_t width, size_t height, uint8_t rotation)
{
    if(rotation == 0)
        return i;

    size_t row = i / width;
    size_t col = i % width;
    
    switch(rotation)
    {
        case 1:
            return (width - col - 1) * height + row;
        case 2:
            return (height - row - 1) * width + (width - col - 1);
        default:
            return col * height + (height - row - 1);
    }
}

size_t transform_index(size_t i, size_t child_w, size_t parent_w, size_t local_x, size_t local_y)
{
    return (i / child_w) * parent_w + (i % child_w) + (local_y * parent_w) + local_x;
}

void DrawBlockMapToBuffer(struct BlockMap *map, struct Buffer *buf, size_t buf_width)
{
    for(size_t i = 0; i < map->area; i++)
    {
        size_t map_i = rotate_index(i, map->width, map->height, map->rotation);
        size_t buf_i = transform_index(i, map->width, buf_width, map->x, map->y);

        struct Block *a = &buf->blocks[buf_i * 2];
        struct Block *b = &buf->blocks[buf_i * 2 + 1];

        if(map->blocks[map_i] == 0)
        {
            a->ch = '(';
            b->ch = ')';
        }
        else
        {
            a->ch = '[';
            b->ch = ']';
        }

        uint8_t fg = BLOCK_COLORS[map->blocks[map_i]];
        a->fg = fg;
        b->fg = fg;
    }
}
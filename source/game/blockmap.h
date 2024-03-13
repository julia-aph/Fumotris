#pragma once
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

void BlockColorsInit(uint8_t *colors, size_t length);

struct BlockMap *NewBlockMap(size_t width, size_t height);

struct BlockMap *NewBlockMapFrom(size_t width, size_t height, uint8_t *blocks);

void DestroyBlockMap(struct BlockMap *map);

void DrawBlockMapToBuffer(struct BlockMap *map, struct Buffer *buf, size_t buf_width);
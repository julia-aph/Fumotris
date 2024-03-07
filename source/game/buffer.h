#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Pixel
{
    char ch;
    uint8_t *codes;
    uint8_t code_count;
};

struct Buffer
{
    size_t width;
    size_t height;
    size_t area;
    
    struct Pixel pixels[];
};

struct Buffer *NewBuffer(size_t width, size_t height);

void DestroyBuffer(struct Buffer *buffer);
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Block
{
    char ch;
    uint8_t fg;
    uint8_t bg;
};

struct Buffer
{
    size_t width;
    size_t height;
    size_t area;
    
    struct Block blocks[];
};

struct Buffer *NewBuffer(size_t width, size_t height);

void DestroyBuffer(struct Buffer *buffer);

char *DrawBufferToString(struct Buffer *buffer);
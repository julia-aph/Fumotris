#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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

struct Buffer *NewBuffer(size_t width, size_t height)
{
    static struct Block default_block = { '#', 37, 0 };

    size_t area = width * height;
    struct Buffer *buffer = malloc(sizeof(struct Buffer) + sizeof(struct Block) * area);

    *buffer = (struct Buffer)
    {
        .width = width, .height = height, .area = area,
    };

    memset(buffer->blocks, 0, sizeof(struct Block) * area);
    for(size_t i = 0; i < area; i++)
    {
        buffer->blocks[i] = default_block;
    }

    return buffer;
}

void DestroyBuffer(struct Buffer *buffer)
{
    free(buffer);
}

static size_t find_digits(uint8_t x)
{
    if(x < 10) return 1;
    if(x < 100) return 2;
    return 3;
}

static size_t color_code_size(uint8_t fg, uint8_t bg)
{
    return find_digits(fg) + find_digits(bg) + 4;
}

size_t gen_color_code(char *color_buf, uint8_t fg, uint8_t bg)
{
    if(bg == 0)
        return snprintf(color_buf, 10, "\x1b[%um", fg);

    return snprintf(color_buf, 10, "\x1b[%u;%um", fg, bg);
}

char *DrawBufferToString(struct Buffer *buffer)
{
    size_t len = 3 + buffer->area + buffer->height;

    for(size_t i = 0; i < buffer->area; i++)
    {
        struct Block *block = &buffer->blocks[i];

        len += color_code_size(block->fg, block->bg);
    }

    char *str_buf = malloc(len + 1);
    str_buf[len] = 0;

    snprintf(str_buf, 4, "\x1b[H");
    size_t buf_filled = 3;

    char color_buf[10];

    for(size_t i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0)
        {
            str_buf[buf_filled] = '\n';
            buf_filled += 1;
        }

        struct Block *block = &buffer->blocks[i];

        size_t color_len = gen_color_code(color_buf, block->fg, block->bg);
        memcpy(str_buf + buf_filled, color_buf, color_len);

        buf_filled += color_len;
        str_buf[buf_filled] = block->ch;
        buf_filled += 1;
    }

    return str_buf;
}
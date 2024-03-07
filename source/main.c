#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>

#include "buffer.h"
#include "blockmap.h"
#include "input.h"
#include "str.h"

int FindDigits(uint8_t x)
{
    if(x < 10) return 1;
    if(x < 100) return 2;
    return 3;
}

char *GenerateColorCode(uint8_t *codes, size_t count)
{
    int digits = 0;
    
    for(size_t i = 0; i < count; i++)
    {
        digits += FindDigits(codes[i]);
    }

    char *color_string = NewStringAlloc(
        2 
        + digits 
        + count-1
        + 1
    );

    ConcatChars(&color_string, "\x1b[");

    for(size_t i = 0; i < count; i++)
    {
        char numbers[4] = { 0 };
        sprintf(numbers, "%u", codes[i]);
        ConcatChars(&color_string, numbers);

        if(i != count-1)
        {
            ConcatChars(&color_string, ";");
        }
    }
    ConcatChars(&color_string, "m");

    return color_string;
}

char *DrawBufferToString(struct Buffer *buffer)
{
    static uint8_t *reset_code = { 0 };

    size_t string_size = buffer->area + buffer->height - 1;
    char *string = NewStringAlloc(string_size);

    ConcatChars(&string, "\x1b[H");
                printf("woag");

    ConcatStr(&string, GenerateColorCode(reset_code, 1));

    uint8_t last_color = 0;

    for(size_t i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0 && i != 0 && i != buffer->area - 1)
            ConcatChar(&string, '\n');

        uint8_t new_color = *buffer->pixels[i].codes;

        if(new_color != last_color)
        {
            char *color_string = GenerateColorCode(buffer->pixels[i].codes, buffer->pixels[i].code_count);
            ConcatStr(&string, color_string);
            DestroyString(color_string);
            last_color = new_color;
        }

        ConcatChar(&string, buffer->pixels[i].ch);
    }

    return string;
}

size_t RotateIndex(size_t i, size_t width, size_t height, uint8_t rotation)
{
    if(rotation == 0) return i;

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

void DrawBlockMapToBuffer(struct BlockMap *map, struct Buffer *buffer)
{
    size_t buffer_block_width = buffer->width / 2;

    for(size_t i = 0; i < map->area; i++)
    {
        size_t map_index = RotateIndex(i, map->width, map->height, map->rotation);
        size_t buffer_index = 
            i / map->width * buffer_block_width
            + i % map->width
            + map->y * buffer_block_width
            + map->x;

        if(buffer_index < 0 || buffer_index >= buffer->area / 2)
            continue;
        
        size_t a = buffer_index * 2;
        size_t b = buffer_index * 2 + 1;

        if(map->blocks[map_index] == 0)
        {
            buffer->pixels[a].ch = '(';
            buffer->pixels[b].ch = ')';
        }
        else
        {
            buffer->pixels[a].ch = '[';
            buffer->pixels[b].ch = ']';
        }

        buffer->pixels[a].codes[0] = 90;
        buffer->pixels[b].codes[0] = 90;
    }
}

int main()
{
    struct BlockMap *board = NewBlockMap(10, 10);
    struct Buffer *display_buffer = NewBuffer(20, 10);

    DrawBlockMapToBuffer(board, display_buffer);

    char *out = DrawBufferToString(display_buffer);
    puts(out);
    printf("FUCK");

    return 0;
}
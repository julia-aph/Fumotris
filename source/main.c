#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "buffer.h"
#include "blockmap.h"
#include "input.h"


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

    char *colorString = NewStringAllocate(
        2 
        + digits 
        + count-1
        + 1
    );

    ConcatChars(&colorString, "\x1b[");
    for(size_t i = 0; i < count; i++)
    {
        size_t newLength = sprintf(colorString, "%s%d", colorString, codes[i]);
        SetStringLength(colorString, newLength);

        if(i != count-1)
        {
            ConcatChars(&colorString, ";");
        }
    }
    ConcatChars(&colorString, "m");

    return colorString;
}

char *DrawBufferToString(struct Buffer *buffer)
{
    size_t stringSize = buffer->area + buffer->height - 1;
    char *string = NewStringAllocate(stringSize);

    ConcatChars(&string, "\x1b[H");
    ConcatStr(&string, GenerateColorCode(&(uint8_t){0}, 1));

    uint8_t lastColor = 0;

    for(size_t i = 0; i < buffer->area; i++)
    {
        if(i % buffer->width == 0 && i != 0 && i != buffer->area-1)
        {
            ConcatChar(&string, '\n');
        }

        uint8_t newColor = buffer->codeBuffer[i][0];

        if(newColor != lastColor)
        {
            char *colorString = GenerateColorCode(&(unsigned char) {newColor}, 1);
            ConcatStr(&string, colorString);
            DestroyString(colorString);
            lastColor = newColor;
        }

        ConcatChar(&string, buffer->chars[i]);
    }

    ConcatStr(&string, GenerateColorCode(&(unsigned char){0}, 1));

    printf("%u\n", StringLength(string));

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
        case 3:
            return col * height + (height - row - 1);
    }
}

void DrawBlockMapToBuffer(struct BlockMap *blockMap, struct Buffer *buffer)
{
    size_t bufferBlockWidth = buffer->width / 2;

    for(size_t i = 0; i < blockMap->area; i++)
    {
        size_t mapIndex = RotateIndex(i, blockMap->width, blockMap->height, blockMap->rotation);
        size_t bufferIndex = 
            i / blockMap->width * bufferBlockWidth
            + i % blockMap->width
            + blockMap->y * bufferBlockWidth
            + blockMap->x;

        if(bufferIndex < 0 || bufferIndex >= buffer->area / 2)
            continue;

        size_t a = bufferIndex * 2;
        size_t b = bufferIndex * 2 + 1;

        if(blockMap->blocks[mapIndex] == 0)
        {
            buffer->chars[a] = '(';
            buffer->chars[b] = ')';
        }
        else
        {
            buffer->chars[a] = '[';
            buffer->chars[b] = ']';
        }

        uint8_t color = 0;

        color = 90;

        buffer->codeBuffer[a][0] = color;
        buffer->codeBuffer[b][0] = color;
    }
}

void OverlayBlockMap(struct BlockMap *dest, struct BlockMap *source)
{
    for(size_t i = 0; i < source->area; i++)
    {
        size_t sourceIndex = RotateIndex(i, source->width, source->height, source->rotation);

        if(source->blocks[sourceIndex] == 0)
            continue;

        size_t destIndex = 
            i / source->width * dest->width
            + i % source->width
            + source->y * dest->width
            + source->x;

        dest->blocks[destIndex] = source->blocks[sourceIndex];
    }
}

int main()
{
    struct BlockMap *board = NewBlockMap(10, 10);
    struct Buffer *displayBuffer = NewBuffer(20, 10);

    DrawBlockMapToBuffer(board, displayBuffer);
    char *out = DrawBufferToString(displayBuffer);

    puts(out);

    printf("done");
    return 0;
}
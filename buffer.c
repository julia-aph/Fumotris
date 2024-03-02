#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct Buffer
{
    size_t width;
    size_t height;
    size_t area;
    
    char *chars;
    uint8_t **codeBuffer;
    uint8_t *codeBufferLengths;
};

struct Buffer *NewBuffer(size_t width, size_t height)
{
    size_t area = width * height;

    char *chars = malloc(area * sizeof(char));

    // Initialize each character as #
    for(size_t i = 0; i < area; i++)
    {
        chars[i] = '#';
    }

    uint8_t **codeBuffer = malloc(area * sizeof(void*));
    uint8_t *codeBufferLengths = malloc(area * sizeof(uint8_t));

    // Initialize each character with 1 code
    for(size_t i = 0; i < area; i++)
    {
        codeBuffer[i] = calloc(1, sizeof(unsigned char));
        codeBufferLengths[i] = 1;
    }

    struct Buffer *buffer = malloc(sizeof(struct Buffer));
    *buffer = (struct Buffer)
    {
        .width = width, .height = height, .area = area,

        .chars = chars, .codeBuffer = codeBuffer, .codeBufferLengths = codeBufferLengths
    };

    return buffer;
}

void DestroyBuffer(struct Buffer *buffer)
{
    free(buffer->chars);
    for(size_t i = 0; i < buffer->area; i++)
    {
        free(buffer->codeBuffer[i]);
    }
    free(buffer->codeBuffer);
    free(buffer->codeBufferLengths);

    free(buffer);
}